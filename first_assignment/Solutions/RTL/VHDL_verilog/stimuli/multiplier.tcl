#!/usr/bin/tclsh


# source ~/vivado_projects/PROJECT_PSE/first_assignment/Solutions/RTL/VHDL_verilog/stimuli/multiplier.tcl

# open_wave_config ~/vivado_projects/double_multiplier/vhdl_multiplier_behav.wcfg
# open_wave_config ~/vivado_projects/double_multiplier/verilog_multiplier_behav.wcfg


restart

set PERIOD 100ns;
#run 50ns;
add_force clk {1 0ns} {0 50ns} -repeat_every $PERIOD;

run $PERIOD;
add_force rst 1;

run $PERIOD;
add_force rst 0;
add_force ready 1;
add_force op1 00111111100000000000000000000000; #1.0
add_force op2 00111111101100110011001100110011; #1.4

run $PERIOD;
add_force ready 0;


for { set a 0}  {$a < 13} {incr a} {
    run $PERIOD;
}

add_force ready 1;
add_force op1 01000000000000000000000000000000; #2.0
add_force op2 11111111100000000000000000000000; #-inf

run $PERIOD;
add_force ready 0;


for { set a 0}  {$a < 7} {incr a} {
    run $PERIOD;
}
