package multiplier_pkg is
    type MULT_STATE is (ST_START, ST_EVAL1, ST_EVAL2, ST_CHECK, ST_ELAB, ST_UNDERF, ST_NORM1, ST_ROUND, ST_NORM2, ST_OVERF, ST_FINISH);
    constant T_ZER : bit_vector(2 downto 0) := "00";
    constant T_INF : bit_vector(2 downto 0) := "01";
    constant T_NAN : bit_vector(2 downto 0) := "10";
    constant T_NUM : bit_vector(2 downto 0) := "11";
end multiplier_pkg;


library IEEE;
use WORK.multiplier_pkg.all;
use IEEE.STD_LOGIC_1164.ALL;

-- Uncomment the following library declaration if using
-- arithmetic functions with Signed or Unsigned values
--use IEEE.NUMERIC_STD.ALL;

-- Uncomment the following library declaration if instantiating
-- any Xilinx leaf cells in this code.
--library UNISIM;
--use UNISIM.VComponents.all;


entity vhdl_multiplier is
    Port (  clk, rst, ready: in bit;
            op1, op2: in bit_vector(31 downto 1);
            res: out bit_vector(31 downto 1);
            done: out bit);
end vhdl_multiplier;


architecture Behavioral of vhdl_multiplier is
    signal STATE, NEXT_STATE: MULT_STATE;
    signal special, norm_again : bit;
    signal res_type : bit_vector(2 downto 0);
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
        variable sign1, sign2 : bit;
        variable esp1, esp2 : bit_vector(7 downto 0);
        variable mant1, mant2 : bit_vector(23 downto 0);
        variable esp_tmp : bit_vector(9 downto 0);
        variable mant_tmp : bit_vector(47 downto 0);
        variable op1_type, op2_type : bit_vector(2 downto 0);
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
                            
                when ST_EVAL1 =>
                    if esp1 = "11111111" then
                        if mant1(22 downto 0) = "00000000000000000000000" then
                            op1_type := T_INF;
                        else
                            op1_type := T_NAN;
                        end if;
                    else
                        if esp1 = "00000000" and mant1(22 downto 0) = "00000000000000000000000" then
                            op1_type := T_ZER;
                        else
                            op1_type := T_NUM;
                        end if;
                    end if;
                            
            
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
        end if;
    end process DATAPATH;


end Behavioral;
