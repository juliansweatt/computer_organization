# Project 1
__CDA3101 - Julian Sweatt__  

---

## Contents
* [README.md](./README.md) _This File_ | Project Details & Information
* [proj1.c](./proj1.c) | MIPS Assembler Implementation
* [tests](./tests) | Directory Contains Tests of Project 1
  * [test1.asm](./tests/test1.asm) | Test of All Required Instructions

## Supported Instructions
* SLL _(Shift Left Logical)_
* J _(Jump)_
* BEQ _(Branch Equal)_
* BNE _(Branch Not Equal)_
* ADDI _(Add Immediate)_
* ORI _(Or Immediate)_
* LUI _(Load Upper Immediate)_
* ADD _(Addition)_
* SUB _(Subtraction)_
* LW _(Load Word)_
* NOR _(Neither)_
* SW _(Save Word)_
* LA _(Load Address - Pseudo-Mips Instruction)_

## Test Details
* [Test 1](./tests/test1.asm)
  * `LA` | Tests loading an address found in `.text` or `.data` directives
  * `SLL` | Tests shifting. Note, `0<=shift< 32` is a requirement of the `SLL` instruction, therefore tests only occur within the bounds. 
  * `ADD` | Tests addition using temp/save/null/pseudo registers. 
  * `ADDI` | Tests addition with negative and positive immediates. Edge cases: 2147483647 (max int_32), -2147483648 (min int_32).
  * `NOR` | Tests "neither or" operation on registers. 
  * `ORI` | Tests "bitwise or immediate" operation on registers with negative and positive immediates. Edge cases: 2147483647 (max int_32), -2147483648 (min int_32).
  * `LUI` | Tests "load upper immediate" operation on registers with negative and positive immediates. Edge cases: 2147483647 (max int_32), -2147483648 (min int_32).
  * `SW` | Tests "store word" operation on registers with negative and positive immediates. Edge cases: 2147483647 (max int_32), -2147483648 (min int_32).
  * `LW` | Tests "load word" operation on registers with negative and positive immediates. Edge cases: 2147483647 (max int_32), -2147483648 (min int_32).
  * `BNE` | Tests the "branch not equal to" operation on registers with equal and nonequal values, with preceding and successive labels.
  * `J` | Tests the "jump" operation on labels preceding, succeeding, or equaling the jump address.