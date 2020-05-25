#!/bin/bash
CMD=$1
declare -a ABSTRACTION=("UT" "LT" "AT4" "RTL")



if [ "$CMD" == "make" ]; then
    echo "COMMAND: make"
    for i in "${ABSTRACTION[@]}"; do
        echo -e "\n >> make of $i"
        cd $i
        make
        cd ..
    done

elif [ "$CMD" == "clean" ]; then
    echo "COMMAND: make clean"
    for i in "${ABSTRACTION[@]}"; do
        echo -e "\n >> clean of $i"
        cd $i
        make clean
        cd ..
    done

elif [ "$CMD" == "time" ]; then
    echo "COMMAND: time"
    for i in "${ABSTRACTION[@]}"; do
        echo -e "\n >> time of $i"
        time ./$i/bin/multiplier_$i.x
    done

else
    echo "WRONG COMMAND"
fi