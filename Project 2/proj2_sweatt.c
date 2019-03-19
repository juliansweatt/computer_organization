/* @file proj2_sweatt.c
 * @brief Main program for the MIPS pipeline simulator.
 * @author Julian L. Sweatt
 */

/*----------------------------------*
 *             IMPORTS              *
 *----------------------------------*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

/*----------------------------------*
 *            OPCODES               *
 *----------------------------------*/
// R-Types
#define OP_ADD 32
#define OP_SUB 34
#define OP_SLL 0

// I-Types
#define OP_LW 35
#define OP_SW 43
#define OP_ANDI 12
#define OP_ORI 13
#define OP_BNE 5

// X-Types
#define OP_NOOP 0
#define OP_HALT 1

/*----------------------------------*
 *             CONFIG               *
 *----------------------------------*/
#define MAX_INSTRUCTIONS 100
#define MAX_INS_NAME_LENGTH 5
#define NUM_REGISTERS 32
#define DATA_MEM 32

/*----------------------------------*
 *              HEADER              *
 *----------------------------------*/
// ---------- Structures ---------- //

/**
 * @struct Instruction
 * @brief ...
 */
typedef struct
{
    char* name;          // Instruction Code Representation
    char type;           // Instruction Type
    int rs;              // RS Register
    int rt;              // RT Register
    int rd;              // RD Register
    int imm;             // Immediate
    int bt;              // Branch Target
    int opCode;
    int func;
    int shamt;
} Instruction;

// ---------- Instruction Functions ---------- //
void printPath(void);
void printInstruction(Instruction);

// ---------- Tool Functions ---------- //

// ---------- Debug Functions ---------- //

/*----------------------------------*
 *              GLOBALS             *
 *----------------------------------*/
Instruction INS[100];
int REGFILE[NUM_REGISTERS];
int DATAMEM[DATA_MEM];
int ProgCounter;

/*----------------------------------*
 *          IMPLEMENTATIONS         *
 *----------------------------------*/

void bin(unsigned n) 
{ 
    unsigned i; 
    for (i = 1 << 31; i > 0; i = i / 2) 
        (n & i)? printf("1"): printf("0"); 
    printf("\n");
} // DEBUG FUNCTION FROM G4G

int rightMostBits(int orig, int numBits)
{
    int i;
    int andOp = 0;

    for(i = numBits-1; i >= 0; i--)
    {
        andOp = andOp + pow(2,i);
    }

    return orig & andOp;
}

int getOpCode(int ins)
{
    return rightMostBits((ins >> 26),6);
}

int getRS(int ins)
{
    return rightMostBits((ins >> 21),5);
}

int getRT(int ins)
{
    return rightMostBits((ins >> 16),5);
}

int getRD(int ins)
{
    return rightMostBits((ins >> 11),5);
}

int getShamt(int ins)
{
    return rightMostBits((ins >> 6),5);
}

int getFunc(int ins)
{
    return rightMostBits(ins,6);
}

int getImmediate(int ins)
{
    return rightMostBits(ins,16);
}

char getType(int ins)
{
    int opCode = getOpCode(ins);
    if(opCode == 0 && getFunc(ins)>0)
    {   
        // R has opCode 0 because opCode is saved in Func segment
        return 'R';
    }
    else if(opCode == OP_NOOP || opCode == OP_HALT)
    {
        // Project Unique Op-Codes
        return 'X';
    }
    else
    {
        // J-Types are not supported, so a Non-R-Type is an I-Type
        return 'I';
    }
}

char* getName(int ins)
{
    if(getType(ins) == 'R')
    {
        // R-Types
        switch(getFunc(ins))
        {
            case OP_ADD: return "add";
            case OP_SUB: return "sub";
            case OP_SLL: return "sll";
        }
    }
    else if(getType(ins) == 'X')
    {
        // X-Types
        switch(getFunc(ins))
        {
            case OP_NOOP: return "noop";
            case OP_HALT: return "halt";
        }
    }
    else
    {
        // I-Types
        switch(getOpCode(ins))
        {
            case OP_LW: return "lw";
            case OP_SW: return "sw";
            case OP_ANDI: return "andi";
            case OP_ORI: return "ori";
            case OP_BNE: return "bne";
        }
    }

    return "???"; // Invalid Input
}

Instruction serializeInstruction(int ins)
{
    Instruction SerIns;

    SerIns.name = getName(ins);
    SerIns.type = getType(ins);
    SerIns.opCode = getOpCode(ins);
    SerIns.rs = getRS(ins);
    SerIns.rt = getRT(ins);

    if(SerIns.type == 'R')
    {
        SerIns.rd = getRD(ins);
        SerIns.shamt = getShamt(ins);
        SerIns.func = getFunc(ins);

        // Remaining Null
        SerIns.imm = 0;
    }
    else if(SerIns.type == 'I')
    {
        SerIns.imm = getImmediate(ins);

        // Remaining Nulls
        SerIns.rd = 0;
        SerIns.shamt = 0;
        SerIns.func = 0;
    }

    return SerIns;
}

int main()
{
    printPath();
    int orig = 17387552;
    Instruction s = serializeInstruction(orig);
    printInstruction(s);
}

void printPath()
{
    // Iterator Declaration
    int i;

    // Program Counter
    printf("\tPC = %d\n", ProgCounter);

    // Data Memory
    printf("\tData Memory:\n");
    for( i = 0; i < DATA_MEM/2; i++ )
    {
        printf("\t\tdataMem[%d] = %d\t\tdataMem[%d] = %d\n", i, DATAMEM[i], (i+(DATA_MEM/2)),DATAMEM[i+(DATA_MEM/2)]);
    }

    // Print Registers
    printf("\tRegisters:\n");
    for( i = 0; i < NUM_REGISTERS/2; i++ )
    {
        printf("\t\tregFile[%d] = %d\t\tregFile[%d] = %d\n", i, REGFILE[i], (i+(NUM_REGISTERS/2)),REGFILE[i+(NUM_REGISTERS/2)]);
    }
}

void printInstruction(Instruction ins)
{
    printf("Name: %s\nType: %c\nRS: %d\nRT: %d\nRD: %d\nImmediate: %d\nBranch Target: %d\
    \nOpCode: %d\nFunc: %d\nShamt: %d\n", ins.name,ins.type, ins.rs, ins.rt, ins.rd, ins.imm,
    ins.bt, ins.opCode, ins.func, ins.shamt);
}