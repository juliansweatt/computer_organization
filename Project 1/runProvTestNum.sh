#!/bin/sh
mkdir -p _tout
./a.out < providedTests/test$1.asm > _tout/t$1o
./Proj1.exe < providedTests/test$1.asm > _tout/t$1exe
echo "Differences for Test $1:"
diff _tout/t$1o _tout/t$1exe