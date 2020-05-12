#!/usr/bin/tclsh


# source vivado_projects/PROJECT_PSE/first_assignment/Solutions/RTL/VHDL_verilog/stimuli/double_multiplier.tcl

# open_wave_config vivado_projects/PROJECT_PSE/first_assignment/Solutions/RTL/VHDL_verilog/waves/double_multiplier_behav.wcfg


restart

set PERIOD 100ns
set HALF_PERIOD 50ns
add_force clk {1 0ns} {0 50ns} -repeat_every $PERIOD

run $PERIOD;
add_force rst 1;

run $HALF_PERIOD;
run $PERIOD;
add_force rst 0;

run $PERIOD;
add_force ready 1;

run $PERIOD;
add_force ready 0;
add_force op1 01000001010110000000000000000000; #13.5
add_force op2 00111111100000000000000000000000; #1.0

run $PERIOD;
add_force op1 01000000000000000000000000000000; #2.0
add_force op2 01000000001000000000000000000000; #2.5


for { set a 0}  {$a < 8} {incr a} {
    run $PERIOD;
}
