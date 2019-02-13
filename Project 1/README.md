# Project 1
__CDA3101 - Julian Sweatt__  

---

## Contents
* [README.md](./README.md) _This File_ | Project Details & Information
* [proj1.c](./proj1.c) | MIPS Assembler Implementation
* [tests](./tests) | Directory Contains Tests of Project 1
  * [runTests.bash](./tests/runTests.bash) | Run All Tests Against Provided Executable
  * [test1.asm](./tests/test1.asm) | Test of All Required Instructions
  * [test2.asm](./tests/test2.asm) | Test of All R-Type Instructions
  * [test3.asm](./tests/test3.asm) | Test of All I-Type Instructions (Includes LA)
  * [test4.asm](./tests/test4.asm) | Test of J-Type Instruction
  * [test5.asm](./tests/test5.asm) | Demo MIPS Program (Tests All Supported Instructions)

## Supported Instructions
* SLL _(Shift Left Logical)_
* J _(Jump)_
* **BEQ** _(Branch Equal)_
* BNE _(Branch Not Equal)_
* ADDI _(Add Immediate)_
* ORI _(Or Immediate)_
* LUI _(Load Upper Immediate)_
* ADD _(Addition)_
* **SUB** _(Subtraction)_
* LW _(Load Word)_
* NOR _(Neither)_
* SW _(Save Word)_
* LA _(Load Address - Pseudo-Mips Instruction)_

## Test Details
* [Test Script](./tests/runTests.bash)
  * Bash script which compiles `proj1.c` and runs all tests in the [`tests`](./tests) directory on `a.out` against the known correct executable file `Proj1.exe`.
  * Assumed to be executed in a directory containing `proj1.c` and `Proj1.exe` where the [`tests`](./tests) directory is a subdirectory. 
* [Test 1](./tests/test1.asm) | Brute Force Test Which Assesses All Required Commands Within Their Argument Bounds Including Edge Cases
  * **NOTE:** This test does not follow the logical flow of MIPS. Jumps to memory are used, for example. This is a test of the functionality of all supported instructions in the assembler, not of proper usage or syntax.
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
* [Test 2](./tests/test2.asm) | Test of All Supported R-Type Instructions (Required & Extra) Within Their Bounds
  * `SUB` | Tests the "subtract" operation.
  * `SLL` | Tests the "shift left logical" operation where the shamt is within the range `0<=shamt< 32`.
  * `NOR` | Tests the "neither or" operation.
  * `ADD` | Tests the "addition" operation.
* [Test 3](./tests/test3.asm) | Test of All Supported (Required & Extra) I-Type Instructions Within Their Bounds
  * `ADDI` | Tests the "addition immediate" operation for positive and negative immediates.
  * `ORI` | Tests the "bitwise or immediate" operation on several valid register values, positive and negative immediates.
  * `LUI` | Tests the "load upper immediate" operation on **all** valid register values, positive and negative immediates.
  * `SW` | Tests the "store word" operation on several register values, positive and negative immediates. Uses addresses loaded from `la`.
  * `LW` | Tests the "load word" operation on several register values, positive and negative offset immediates. Uses addresses loaded from `la`.
  * `LA` | Tests the "load address" operation on several register values, with all label types, preceding and succeeding the load. Loaded addresses are used by `lw` and `sw` in this test.
  * `BNE` | Tests the "branch not equal to" operation in a loop which will evaluate equal and non-equal conditions, using preceding and succeeding labels. 
  * `BEQ` | Tests the "branch equal to" operation in a loop which will evaluate equal and non-equal conditions, using preceding and succeeding labels.
* [Test 4](./tests/test4.asm) | Test of The J-Type Instruction
  * `J` | Tests the "jump" operation using preceding and succeeding labels.
  * `ADDI` | Tests the "add immediate" operation by using it as the final function to escape the jumps.
* [Test 5](./tests/test5.asm) | Program With a Logical Flow Which Tests Realistic Usability of the Assembler and Uses All Required Supported Instructions