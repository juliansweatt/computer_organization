#!/bin/bash
gcc ./proj1.c
for i in {1..1}
do
    echo "Differences for Test $i:"
    diff <(./a.out < tests/test$i.asm) <(./Proj1.exe < tests/test$i.asm)
done