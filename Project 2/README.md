# Project 2 - Mips Pipeline Simulator
__CDA3101 - Julian Sweatt__  

---
## About
MIPS Pipeline Simulator created from [CDA3101 Project 2 Specifications](http://www.cs.fsu.edu/~jayarama/org2sp19/Homeworks/Project2.pdf). 

## Contents
* [README.md](./README.md) _This File_ | Project Details & Information
* [proj2_sweatt.c](./proj2_sweatt.c) | MIPS Pipeline Simulator
* [tests/](./tests) | Contains Assembly & Object Level Tests
    * [tests/asm](./tests/asm) | Directory Contains Assembly Level Tests of Project 2
    * [tests/o](./tests/asm) | Directory Contains Object Level Tests of Project 2

## Known Issues
* Branching (`bne` instruction) is not implemented beyond the instruction being parsed.
* In the `EX/MEM` stage, the `writeDataReg` value may not be populated for instructions that do not use it. This does not effect the end result, but may contradict the provided executable. 

## Supported Instructions
* Implemented
    * `add` _(Addition)_
    * `sub` _(Subtraction)_
    * `lw` _(Load Word)_
    * `sw` _(Store Word)_
    * `ori` _(Bitwise Or Immediate)_
    * `andi` _(Bitwise And Immediate)_
    * `sll` _(Shift Left Logical)_
    * `noop` _(No Operation [Stall])_
    * `halt` _(Halt or End of Program)_
* Not Complete
    * `bne` _(Branch if Not Equal)_ **Not Implemented, Only Parsed**

## Test Details
* [no_branch_no_hazards](./tests/asm/no_branch_no_hazards.asm) | Tests all instructions (except `bne`) with `noop`s inserted to avoid any potential hazards.
* [single_forwarding](./tests/asm/single_forwarding.asm) | Tests all instructions (except `bne`) where each instruction is dependent upon the instruction ahead of it's write. Does not test `sub` and `add`, as they take in two registers.
* [double_forwarding](./tests/asm/double_forwarding.asm) | Tests all instructions (except `bne`) where each instruction is dependent upon the instruction two instructions ahead of it's write.
* [stalling_forwarding](./tests/asm/stalling_forwarding.asm) | Tests all instructions (except `bne`) where hazards occur that must be solved by stalling, single forwarding, and double forwarding.
* [no_branch](./tests/asm/no_branch.asm) | Tests all instructions (except `bne`) for any potential hazards with several different scenarios.
* [branching](./tests/asm/branching.asm) | Tests branching (`bne`). Note, this test is meant to show that the simulation was rigorously tested and to show an intent to continue development in a test-driven environment. However, this test **will not succeed** as branching has not been implemented.