library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL; -- for arithmetic functions with Signed or Unsigned values

package multiplier_pkg is
    type MULT_STATE is (ST_START, ST_EVAL1, ST_EVAL2, ST_CHECK, ST_ELAB, ST_UNDERF, ST_NORM1, ST_ROUND, ST_NORM2, ST_OVERF, ST_FINISH);
    constant T_ZER : std_logic_vector(1 downto 0) := "00";
    constant T_INF : std_logic_vector(1 downto 0) := "01";
    constant T_NAN : std_logic_vector(1 downto 0) := "10";
    constant T_NUM : std_logic_vector(1 downto 0) := "11";
end multiplier_pkg;

library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL; -- for arithmetic functions with Signed or Unsigned values
use WORK.multiplier_pkg.all;

entity vhdl_multiplier is
    Port (  clk, rst, ready: in std_logic;
            op1, op2: in std_logic_vector(31 downto 0);
            res: out std_logic_vector(31 downto 0);
            done: out std_logic);
end vhdl_multiplier;


architecture Behavioral of vhdl_multiplier is
    signal STATE, NEXT_STATE: MULT_STATE;
    signal special, norm_again : std_logic;
    signal res_type : std_logic_vector(1 downto 0);
begin

    FSM: process(STATE, ready, special, norm_again)
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
                NEXT_STATE <= ST_CHECK;
        
            when ST_CHECK =>
                if special = '1' then
                    NEXT_STATE <= ST_FINISH;
                else
                    NEXT_STATE <= ST_ELAB;
                end if;
        
            when ST_ELAB =>
                if special = '1' then
                    NEXT_STATE <= ST_FINISH;
                else
                    NEXT_STATE <= ST_UNDERF;
                end if;
        
            when ST_UNDERF =>
                if special = '1' then
                    NEXT_STATE <= ST_FINISH;
                else
                    NEXT_STATE <= ST_NORM1;
                end if;
        
            when ST_NORM1 =>
                NEXT_STATE <= ST_ROUND;
        
            when ST_ROUND =>
                if norm_again = '1' then
                    NEXT_STATE <= ST_NORM2;
                else
                    NEXT_STATE <= ST_OVERF;
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
        variable sign1, sign2 : std_logic;
        variable esp1, esp2 : std_logic_vector(7 downto 0);
        variable mant1, mant2 : std_logic_vector(23 downto 0);
        variable esp_tmp : std_logic_vector(9 downto 0);
        variable mant_tmp : std_logic_vector(47 downto 0);
        variable op1_type, op2_type : std_logic_vector(1 downto 0);
        variable p3 : std_logic_vector(7 downto 0);
        variable p1 : std_logic_vector(3 downto 0);
        variable p2 : std_logic_vector(3 downto 0);
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
                    -- Reset internal comunication signals
                    special <= '0';             
                    norm_again <= '0';
                    -- Get informations of op1
                    sign1 := op1(31);             
                    esp1:= op1(30 downto 23);
                    mant1(23) := '1';
                    mant1(22 downto 0) := op1(22 downto 0);
                    -- Get informations of op2
                    sign2 := op2(31);             
                    esp2 := op2(30 downto 23);
                    mant2(23) := '1';
                    mant2(22 downto 0) := op2(22 downto 0);
                
                -- Special case op1 check            
                when ST_EVAL1 =>
                    if (esp1 = "11111111") then
                        if (mant1(22 downto 0) = "00000000000000000000000") then
                            op1_type := T_INF;
                        else
                            op1_type := T_NAN;
                        end if;
                    else
                        if (esp1 = "00000000" and mant1(22 downto 0) = "00000000000000000000000") then
                            op1_type := T_ZER;
                        else
                            op1_type := T_NUM;
                        end if;
                    end if;
                            
            
                -- Special case op2 check
                when ST_EVAL2 =>
                    if (esp2 = "11111111") then
                        if (mant2(22 downto 0) = "00000000000000000000000") then
                            op2_type := T_INF;
                        else
                            op2_type := T_NAN;
                        end if;
                    else
                        if (esp2 = "00000000" and mant2(22 downto 0) = "00000000000000000000000") then
                            op2_type := T_ZER;
                        else
                            op2_type := T_NUM;
                        end if;
                    end if;
                
                -- Check special case for res
                when ST_CHECK =>
                    if (op1_type = T_NAN OR op2_type = T_NAN OR
                        (op1_type = T_ZER AND op2_type = T_INF) OR
                        (op2_type = T_ZER AND op1_type = T_INF)) then
                        res_type <= T_NAN;
                        special <= '1';
                            
                    elsif (op1_type = T_ZER OR op2_type = T_ZER) then
                        res_type <= T_ZER;
                        special <= '1';
                        
                    elsif (op1_type = T_INF OR op2_type = T_INF) then
                        res_type <= T_INF;
                        special <= '1';
                        
                    else
                        res_type <= T_NUM;
                        special <= '0';
                    end if;
                
                -- Process esp and mant
                when ST_ELAB =>
                    mant_tmp := std_logic_vector(unsigned(mant1) + unsigned(mant2));
                    esp_tmp := std_logic_vector(unsigned(esp1) + unsigned(esp2) - 127);  
                
                -- Underflow check
                when ST_UNDERF =>
                    if (esp_tmp(9) = '1') then           --undeflow check
                        res_type <= T_ZER;
                        special <= '1';
                    else 
                        res_type <= T_NUM;
                        special <= '0';
                    end if;
                
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
                
                -- Normalize result after rounding
                when ST_NORM2 =>
                    if (mant_tmp(46 downto 24) = "11111111111111111111111") then
                        esp_tmp := std_logic_vector(unsigned(esp_tmp) + 1);     --increment esp
                        mant_tmp(46 downto 24) := "00000000000000000000000";    --reset mant
                    else
                        mant_tmp(46 downto 24) := std_logic_vector(unsigned(mant_tmp(46 downto 24)) + 1);
                    end if;
                
                -- Overflow check and store mant
                when ST_OVERF =>
                    if (esp_tmp(8) = '1') then                      --overflow check
                        res_type <= T_INF;
                        special <= '1';
                    else
                        res_type <= T_NUM;
                        special <= '0';
                        res(30 downto 23) <= esp_tmp(7 downto 0);   --store esp
                        res(22 downto 0) <= mant_tmp(46 downto 24); --store mant_tmp
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
