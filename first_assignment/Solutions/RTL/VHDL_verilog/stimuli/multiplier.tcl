#!/usr/bin/tclsh


# source vivado_projects/PROJECT_PSE/first_assignment/Solutions/RTL/VHDL_verilog/stimuli/multiplier.tcl

# open_wave_config vivado_projects/PROJECT_PSE/first_assignment/Solutions/RTL/VHDL_verilog/waves/vhdl_multiplier_behav.wcfg
# open_wave_config vivado_projects/PROJECT_PSE/first_assignment/Solutions/RTL/VHDL_verilog/waves/verilog_multiplier_behav.wcfg


restart

set PERIOD 100ns;
add_force clk {1 0ns} {0 50ns} -repeat_every $PERIOD;

run $PERIOD;
add_force rst 1;

run $PERIOD;
add_force rst 0;
add_force op1 01000000000000000000000000000000; #2.0
add_force op2 01000000001000000000000000000000; #2.5

run $PERIOD;
add_force ready 1;

run $PERIOD;
add_force ready 0;

for { set a 0}  {$a < 12} {incr a} {
    run $PERIOD;
}

run $PERIOD;
add_force op1 00111111101000000000000000000000; #1.25
add_force op2 11111111100000000000000000000000; #-inf

run $PERIOD;
add_force ready 1;

run $PERIOD;
add_force ready 0;

for { set a 0}  {$a < 7} {incr a} {
    run $PERIOD;
}
