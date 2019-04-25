# Project 3 - Cache Simulator
__CDA3101 - Julian Sweatt__  

---

## About
Cache Simulator created from [CDA3101 Project 3 Specifications](http://www.cs.fsu.edu/~jayarama/org2sp19/Homeworks/Project3.pdf). 

## Contents
* [README.md](./README.md) _This File_ | Project Details & Information
* [proj3_sweatt.c](./proj3_sweatt.c) | Cache Simulator
* [tests/](./tests) | Contains Tests

## Features
* Read in and parse a series of read and write instructions
* Calculate bits for tag, index, and offset from given address
* Dynamic caching
* Least Recently Used (LRU) cache replacement
* Multi-associativity support
* Write-Through, No-Write Allocate cache simulation
* Write-Back, Write Allocate cache simulation
* Detailed test suite
_All Features from Spec are Completed_

## Test Details
* [2_way_given](./tests/2_way_given.txt) | Tests LRU replacement, 2-way set associative caching, and dirty bit support. _Given in Project 3 Specifications_
* [2_way_hw](./tests/2_way_hw.txt) | Tests 2-way set associative caching, LRU replacement, and dirty bit support. _Process Derived from Homework 3 with Altered Associativity._
* [2_way_long](./tests/2_way_long.txt) | Tests 2-way set associative caching, LRU replacement, dirty bit support, and longer instruction sets. _Custom Written Process_
* [2_way_very_long](./tests/2_way_very_long.txt) | Tests 2-way set associative caching, LRU replacement, dirty bit support, and maximal length instruction sets. _Custom Written Process_
* [4_way_hw](./tests/4_way_hw.txt) | Tests 4-way set associative caching without replacement. _Process from Homework 3._
* [8_way_long](./tests/8_way_long.txt) | Tests 8-way set associative caching and longer instruction sets. _Custom Written Process_
