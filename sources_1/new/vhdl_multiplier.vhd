package multiplier_pkg is
    type MULT_STATE is (ST_START, ST_EVAL1, ST_EVAL2, ST_EVAL3, ST_CHECK1, ST_ELAB, ST_UNDERF, ST_CHECK2, ST_NORM1, ST_ROUND, ST_CHECK3, ST_NORM2, ST_OVERF, ST_FINISH);
    type MULT_TYPE is (T_NUM, T_NAN, T_ZER, T_INF);
end multiplier_pkg;

library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL; -- for arithmetic functions with Signed or Unsigned values
use WORK.multiplier_pkg.all;

entity vhdl_multiplier is
    Port (  clk, rst, ready:    in  std_logic;
            op1, op2:           in  std_logic_vector(31 downto 0);
            res:                out std_logic_vector(31 downto 0);
            done:               out std_logic);
end vhdl_multiplier;


architecture Behavioral of vhdl_multiplier is
    signal STATE, NEXT_STATE:   MULT_STATE;
    signal res_type:            MULT_TYPE;
    signal norm_again:          std_logic;
    signal sign1, sign2 :         std_logic;
    signal esp1, esp2 :           std_logic_vector(9 downto 0);
    signal mant1, mant2 :         std_logic_vector(23 downto 0);
begin

    FSM: process(STATE, ready, res_type, norm_again)
    begin
        case STATE is
            when ST_START =>
                if ready = '1' then
                    NEXT_STATE <= ST_EVAL1;
                else
                    NEXT_STATE <= STATE;
                end if;
                        
            when ST_EVAL1 =>
                NEXT_STATE <= ST_EVAL2;
        
            when ST_EVAL2 =>
                NEXT_STATE <= ST_EVAL3;
        
            when ST_EVAL3 =>
                NEXT_STATE <= ST_CHECK1;
        
            when ST_CHECK1 =>
                if (res_type = T_NUM) then
                    NEXT_STATE <= ST_ELAB;
                else
                    NEXT_STATE <= ST_FINISH;
                end if;
        
            when ST_ELAB =>
                NEXT_STATE <= ST_UNDERF;
        
            when ST_UNDERF =>
                NEXT_STATE <= ST_CHECK2;
        
            when ST_CHECK2 =>
                if (res_type = T_NUM) then
                    NEXT_STATE <= ST_NORM1;
                else
                    NEXT_STATE <= ST_FINISH;
                end if;
        
            when ST_NORM1 =>
                NEXT_STATE <= ST_ROUND;
        
            when ST_ROUND =>
                NEXT_STATE <= ST_CHECK3;
        
            when ST_CHECK3 =>
                if (norm_again = '1') then
                    NEXT_STATE <= ST_NORM2;
                else
                    NEXT_STATE <= ST_FINISH;
                end if;
        
            when ST_NORM2 =>
                NEXT_STATE <= ST_OVERF;
        
            when ST_OVERF =>
                NEXT_STATE <= ST_FINISH;
        
            when ST_FINISH =>
                NEXT_STATE <= ST_START;
            
            when OTHERS =>
                NEXT_STATE <= STATE;                
        end case;
    end process FSM;

    DATAPATH: process(clk, rst)
        --variable sign1, sign2 :         std_logic;
        --variable esp1, esp2 :           std_logic_vector(9 downto 0);
        --variable mant1, mant2 :         std_logic_vector(23 downto 0);
        variable esp_tmp :              std_logic_vector(9 downto 0);
        variable mant_tmp :             std_logic_vector(47 downto 0);
        variable op1_type, op2_type :   MULT_TYPE;
    begin   
    
        if rst = '1' then
            STATE <= ST_START;
                    
        elsif clk'event and clk='1' then        
            STATE <= NEXT_STATE;
            
            case STATE is
                -- Reset register
                when ST_START =>
                    -- Reset signals
                    done <= '0';   
                    norm_again <= '0';
                    -- Get informations of op1
                    sign1 <= op1(31);             
                    esp1  <=  "00" & op1(30 downto 23);
                    mant1 <= "1" & op1(22 downto 0);
                    -- Get informations of op2
                    sign2 <= op2(31);             
                    esp2  <=  "00" & op2(30 downto 23);
                    mant2 <= "1" & op2(22 downto 0);
                
                -- Special case op1 check            
                when ST_EVAL1 =>
                    if (esp1(7 downto 0) = "11111111") then
                        if (mant1(22 downto 0) = "00000000000000000000000") then
                            op1_type := T_INF;
                        else
                            op1_type := T_NAN;
                        end if;
                    else
                        if (esp1(7 downto 0) = "00000000" and (mant1(22 downto 0) = "00000000000000000000000")) then
                            op1_type := T_ZER;
                        else
                            op1_type := T_NUM;
                        end if;
                    end if;
                            
            
                -- Special case op2 check
                when ST_EVAL2 =>
                    if (esp2(7 downto 0) = "11111111") then
                        if (mant2(22 downto 0) = "00000000000000000000000") then
                            op2_type := T_INF;
                        else
                            op2_type := T_NAN;
                        end if;
                    else
                        if (esp2(7 downto 0) = "00000000" and (mant2(22 downto 0) = "00000000000000000000000")) then
                            op2_type := T_ZER;
                        else
                            op2_type := T_NUM;
                        end if;
                    end if;
                
                -- Special case for res
                when ST_EVAL3 =>
                    if (op1_type = T_NAN OR op2_type = T_NAN OR
                        (op1_type = T_ZER AND op2_type = T_INF) OR
                        (op2_type = T_ZER AND op1_type = T_INF)) then
                        res_type <= T_NAN;
                            
                    elsif (op1_type = T_ZER OR op2_type = T_ZER) then
                        res_type <= T_ZER;
                        
                    elsif (op1_type = T_INF OR op2_type = T_INF) then
                        res_type <= T_INF;
                        
                    else
                        res_type <= T_NUM;
                    end if;
            
                -- Next status check
                when ST_CHECK1 =>
                    -- Do nothing
                
                -- Process esp and mant
                when ST_ELAB =>
                    esp_tmp := std_logic_vector(signed(esp1) + signed(esp2) - 127);  
                    mant_tmp := std_logic_vector(unsigned(mant1) * unsigned(mant2));
                
                -- Underflow check
                when ST_UNDERF =>
                    if (esp_tmp(9) = '1') then           --undeflow check
                        res_type <= T_ZER;
                    else 
                        res_type <= T_NUM;
                    end if;
            
                -- Next status check
                when ST_CHECK2 =>
                    -- Do nothing
                
                -- Normalize result
                when ST_NORM1 =>
                    if (mant_tmp(47) = '1') then
                        esp_tmp := std_logic_vector(unsigned(esp_tmp) + 1);     --increment esp
                    else
                        mant_tmp := std_logic_vector(unsigned(mant_tmp) sll 1); --norm mant_tmp
                    end if;
                
                -- Round result
                when ST_ROUND =>
                    if (mant_tmp(23) = '1' OR (mant_tmp(22 downto 0) = "01111111111111111111111")) then
                        norm_again <= '1';
                    else
                        norm_again <= '0';
                    end if;
            
                -- Next status check
                when ST_CHECK3 =>
                    -- Do nothing
                
                -- Normalize result after rounding
                when ST_NORM2 =>
                    if (mant_tmp(46 downto 24) = "11111111111111111111111") then
                        esp_tmp := std_logic_vector(unsigned(esp_tmp) + 1);     --increment esp
                    end if;
                    mant_tmp(46 downto 24) := std_logic_vector(unsigned(mant_tmp(46 downto 24)) + 1);
                    
                -- Overflow check and store mant
                when ST_OVERF =>
                    if (esp_tmp(8) = '1') then                      --overflow check
                        res_type <= T_INF;
                    else
                        res_type <= T_NUM;
                    end if;
                
                -- Finish
                when ST_FINISH =>
                    case res_type is
                        when T_ZER =>
                            res(30 downto 0) <= "0000000000000000000000000000000";
                        
                        when T_INF =>
                            res(30 downto 0) <= "1111111100000000000000000000000";
                        
                        when T_NAN =>
                            res(30 downto 0) <= "1111111111111111111111111111111";
                        
                        when T_NUM =>
                            res(30 downto 23) <= esp_tmp(7 downto 0);   --store esp
                            res(22 downto 0) <= mant_tmp(46 downto 24); --store mant_tmp
                        
                        when OTHERS =>
                            -- Do nothing
                    end case;                                        
                    res(31) <= sign1 XOR sign2;         --get sign
                    done <= '1';
                
                when OTHERS =>
                    -- Do nothing      
                           
            end case;
        end if;
    end process DATAPATH;
end Behavioral;
