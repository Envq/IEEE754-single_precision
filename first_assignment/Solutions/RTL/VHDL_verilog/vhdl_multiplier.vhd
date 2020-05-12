package multiplier_pkg is
    type MULT_STATE is (ST_START, ST_INIT, ST_SNAN1, ST_SNAN2, ST_QNAN, ST_ZERO, ST_INF, ST_ADJ3, ST_ADJ2, ST_ADJ1, ST_ELAB, ST_SHIFTR, ST_SHIFTL, ST_NORM, ST_CHECK, ST_SUBNORM, ST_ROUND, ST_WRITE, ST_FINISH);
end multiplier_pkg;

library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL; -- for arithmetic functions with Signed or Unsigned values
use IEEE.STD_LOGIC_UNSIGNED.ALL;
use WORK.multiplier_pkg.all;

entity vhdl_multiplier is
    Port (  clk:                in  bit;
            rst, ready:         in  std_logic;
            op1, op2:           in  std_logic_vector(31 downto 0);
            res:                out std_logic_vector(31 downto 0);
            done:               out std_logic);
end vhdl_multiplier;


architecture Behavioral of vhdl_multiplier is
    -- states
    signal STATE, NEXT_STATE:   MULT_STATE;
    -- op1 and op2 informations
    signal sign1:               std_logic;
    signal exp1:                std_logic_vector(7 downto 0);
    signal mant1:               std_logic_vector(23 downto 0);
    signal sign2:               std_logic;
    signal exp2:                std_logic_vector(7 downto 0);
    signal mant2:               std_logic_vector(23 downto 0);
    -- intermediate parts
    signal exp_tmp:             std_logic_vector(9 downto 0);
    signal mant_tmp:            std_logic_vector(47 downto 0);
begin

    FSM: process(STATE, ready, exp1, mant1, exp2, mant2, exp_tmp, mant_tmp)
    begin
        case STATE is
            when ST_START =>
                if ready = '1' then
                    NEXT_STATE <= ST_INIT;
                else
                    NEXT_STATE <= STATE;
                end if;
                        
            when ST_INIT =>      
                if (((exp1 = 0 AND mant1(22 downto 0) = 0) AND (exp2 = 255 AND mant2(22 downto 0) = 0)) OR 
                   ((exp1 = 255 AND mant1(22 downto 0) = 0) AND (exp2 = 0 AND mant2(22 downto 0) = 0))) then
                    NEXT_STATE <= ST_QNAN;
                    
                elsif (exp2 = 255 AND mant2(22 downto 0) /= 0) then
                    NEXT_STATE <= ST_SNAN2;
                    
                elsif (exp1 = 255 AND mant1(22 downto 0) /= 0) then
                    NEXT_STATE <= ST_SNAN1;
                    
                elsif ((exp1 = 0 AND mant1(22 downto 0) = 0) OR (exp2 = 0 AND mant2(22 downto 0) = 0)) then
                    NEXT_STATE <= ST_ZERO;
                    
                elsif ((exp1 = 255 AND mant1(22 downto 0) = 0) OR (exp2 = 255 AND mant2(22 downto 0) = 0)) then
                    NEXT_STATE <= ST_INF;
                    
                elsif ((exp1 = 0 AND mant1(22 downto 0) /= 0 AND exp2 = 0 AND mant2(22 downto 0) /= 0)) then
                    NEXT_STATE <= ST_ADJ3;
                    
                elsif (exp2 = 0 AND mant2(22 downto 0) /= 0) then
                    NEXT_STATE <= ST_ADJ2;
                    
                elsif (exp1 = 0 AND mant1(22 downto 0) /= 0) then
                    NEXT_STATE <= ST_ADJ1;  
                                  
                else
                    NEXT_STATE <= ST_ELAB;                    
                end if;
        
            when ST_QNAN =>
                NEXT_STATE <= ST_FINISH;
        
            when ST_SNAN2 =>
                NEXT_STATE <= ST_FINISH;
        
            when ST_SNAN1 =>
                NEXT_STATE <= ST_FINISH;
        
            when ST_ZERO =>
                NEXT_STATE <= ST_FINISH;
        
            when ST_INF =>
                NEXT_STATE <= ST_FINISH;
        
            when ST_ADJ3 =>
                NEXT_STATE <= ST_ELAB;
        
            when ST_ADJ2 =>
                NEXT_STATE <= ST_ELAB;
        
            when ST_ADJ1 =>
                NEXT_STATE <= ST_ELAB;
            
            when ST_ELAB =>
                if (mant_tmp(47) = '1') then
                    NEXT_STATE <= ST_SHIFTR;   
                                 
                elsif (mant_tmp(46) = '1') then
                    NEXT_STATE <= ST_CHECK; 
                                   
                else
                    NEXT_STATE <= ST_SHIFTL;                    
                end if;
        
            when ST_SHIFTR =>
                NEXT_STATE <= ST_CHECK;
        
            when ST_SHIFTL =>
                NEXT_STATE <= ST_NORM;
            
            when ST_NORM =>
                if (exp_tmp(9) = '1' OR exp_tmp = "0000000000") then
                    NEXT_STATE <= ST_CHECK;   
                                 
                elsif (mant_tmp(46) = '1') then
                    NEXT_STATE <= ST_CHECK; 
                                   
                else
                    NEXT_STATE <= ST_SHIFTL;                    
                end if;
                
            when ST_CHECK =>       
                if (exp_tmp(9 downto 8) = "01") then
                    NEXT_STATE <= ST_INF;
                    
                elsif (exp_tmp = "0000000000") then
                    NEXT_STATE <= ST_SUBNORM;
                    
                elsif ((exp_tmp(9 downto 8) = "00") AND (mant_tmp(22) = '0')) then
                    NEXT_STATE <= ST_WRITE;
                    
                elsif ((exp_tmp(9 downto 8) = "00") AND (mant_tmp(22) = '1')) then
                    NEXT_STATE <= ST_ROUND;
                    
                elsif (((exp_tmp + 48) < "1000000000")) then
                    NEXT_STATE <= ST_SHIFTR;
            
                else
                    NEXT_STATE <= ST_ZERO;
                end if;
        
            when ST_SUBNORM =>
                NEXT_STATE <= ST_WRITE;
        
            when ST_ROUND =>
                if (mant_tmp(47) = '1') then
                    NEXT_STATE <= ST_SHIFTR;
                else
                    NEXT_STATE <= ST_WRITE;
                end if;
        
            when ST_WRITE =>
                NEXT_STATE <= ST_FINISH;
        
            when ST_FINISH =>
                NEXT_STATE <= ST_START;
            
            when OTHERS =>
                NEXT_STATE <= STATE;                
        end case;
    end process FSM;


    DATAPATH: process(clk, rst)
    begin       
        if (rst = '1') then
        STATE <= ST_START;
        done <= '0';
        res <= (OTHERS => '0');
        
        exp_tmp <= (OTHERS => '0');           
        mant_tmp <= (OTHERS => '0');
                       
        sign1 <= '0';            
        exp1 <= (OTHERS => '0');
        mant1 <= (OTHERS => '0');
               
        sign2 <= '0';           
        exp2 <= (OTHERS => '0');
        mant2 <= (OTHERS => '0');
                    
        elsif clk'event and clk='1' then        
            STATE <= NEXT_STATE;
            
            case NEXT_STATE is
                when ST_START =>
                    done <= '0';                    
                    exp_tmp <= (OTHERS => '0');           
                    mant_tmp <= (OTHERS => '0');
                                   
                    sign1 <= '0';            
                    exp1 <= (OTHERS => '0');
                    mant1 <= (OTHERS => '0');
                           
                    sign2 <= '0';           
                    exp2 <= (OTHERS => '0');
                    mant2 <= (OTHERS => '0'); 
            
                when ST_INIT =>                    
                    sign1 <= op1(31);
                    exp1  <=  op1(30 downto 23);
                    mant1(23) <=  '1';
                    mant1(22 downto 0) <=  op1(22 downto 0);
                    
                    sign2 <= op2(31);             
                    exp2  <=  op2(30 downto 23);
                    mant2(23) <=  '1';
                    mant2(22 downto 0) <=  op2(22 downto 0);
                          
                when ST_QNAN =>
                    res(31) <= '1';
                    res(30 downto 23) <= (OTHERS => '1');
                    res(22) <= '1';
                    res(21 downto 0) <= (OTHERS => '0');
                          
                when ST_SNAN1 =>
                    res(31) <= sign1;
                    res(30 downto 23) <= (OTHERS => '1');
                    res(22) <= '1';
                    res(21 downto 0) <= mant1(21 downto 0);
                          
                when ST_SNAN2 =>
                    res(31) <= sign2;
                    res(30 downto 23) <= (OTHERS => '1');
                    res(22) <= '1';
                    res(21 downto 0) <= mant2(21 downto 0);
                          
                when ST_ZERO =>
                    res(31) <= (sign1 XOR sign2);
                    res(30 downto 23) <= (OTHERS => '0');
                    res(22 downto 0) <= (OTHERS => '0');
                          
                when ST_INF =>
                    res(31) <= (sign1 XOR sign2);
                    res(30 downto 23) <= (OTHERS => '1');
                    res(22 downto 0) <= (OTHERS => '0');
                          
                when ST_ADJ3 =>
                    mant1(23) <= '0';
                    exp1 <= "00000001";
                    mant2(23) <= '0';
                    exp2 <= "00000001";
                          
                when ST_ADJ2 =>
                    mant2(23) <= '0';
                    exp2 <= "00000001";
                          
                when ST_ADJ1 =>
                    mant1(23) <= '0';
                    exp1 <= "00000001";
                
                when ST_ELAB =>
                    exp_tmp <= ("00" & exp1) + ("00" & exp2) - 127;
                    mant_tmp <= mant1 * mant2;
                
                when ST_SHIFTR =>
                    exp_tmp <= exp_tmp + 1;
                    mant_tmp <= '0' & mant_tmp(47 downto 1); --shift right
                
                when ST_SHIFTL =>
                    mant_tmp <= mant_tmp(46 downto 0) & '0'; --shift left
                    exp_tmp <= exp_tmp - 1;
                    
                when ST_NORM =>
                    -- Do nothing
                    
                when ST_CHECK =>
                    -- Do nothing
                
                when ST_SUBNORM =>
                    mant_tmp <= '0' & mant_tmp(47 downto 1); --shift right
                
                when ST_ROUND =>
                    mant_tmp(47 downto 23) <= mant_tmp(47 downto 23) + 1;
                          
                when ST_WRITE =>
                    res(31) <= (sign1 XOR sign2);
                    res(30 downto 23) <= exp_tmp(7 downto 0);
                    res(22 downto 0) <= mant_tmp(45 downto 23);
                
                when ST_FINISH =>
                    done <= '1';                    
                
                when OTHERS =>
                    -- Do nothing                                
            end case;
        end if;
    end process DATAPATH;
end Behavioral;