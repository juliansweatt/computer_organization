#!/bin/bash
# ABOUT:       Run all tests using the student's version of the assmbler against the given binary
#              solution. Output difference between the two solutions.
# ASSUMPTIONS: Executed from a directory containing the student's sourcecode and the
#              provided executable, where the tests directory is a subdirectory.
gcc ./proj1.c
for i in {1..5}
do
    echo "---------- Differences for Test $i ----------"
    diff <(./a.out < tests/test$i.asm) <(./Proj1.exe < tests/test$i.asm)
done
