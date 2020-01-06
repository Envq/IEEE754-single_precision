#!/usr/bin/tclsh

# source ~/vivado_projects/double_multiplier/stimuli_float.tcl

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
#add_force op2 00000000000000000000000000000000; #0.0

run $PERIOD;
add_force ready 0;
add_force op1 01000000000000000000000000000000; #2.0
#add_force op2 01000000001000000000000000000000; #2.5
add_force op2 11111111100000000000000000000000; #-inf


for { set a 0}  {$a < 12} {incr a} {
    run $PERIOD;
}

add_force ready 1;
run $PERIOD;
add_force ready 0;


for { set a 0}  {$a < 12} {incr a} {
    run $PERIOD;
}

