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
#define DEBUG_MODE 1
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
    int opCode;          // Operation Code
    int func;            // Function Code
    int shamt;           // Shift
} Instruction;

/**
 * @struct P_If_Id
 * @brief Pipeline IF/ID
 */
typedef struct
{
    Instruction instruction;    // Instruction
    int pc4;                    // PC + 4
} P_If_Id;

/**
 * @struct P_Id_Ex
 * @brief Pipeline ID/EX
 */
typedef struct
{
    Instruction instruction;    // Instruction
    int pc4;                    // PC + 4
    int bt;                     // Branch Target
    int rs;                     // RS Register
    int rt;                     // RT Register
    int rd;                     // RD Register
    int read1;                  // Data Read 1
    int read2;                  // Data Read 2
    int imm;                    // Immediate
} P_Id_Ex;

/**
 * @struct P_Ex_Mem
 * @brief Pipeline EX/MEM
 */
typedef struct
{
    Instruction instruction;    // Instruction
    int aluRes;                 // ALU Result
    int wd;                     // Write Data
    int wr;                     // Write Register
} P_Ex_Mem;

/**
 * @struct P_Mem_Wb
 * @brief Pipeline MEM/WB
 */
typedef struct
{
    Instruction instruction;    // Instruction
    int writeFromMem;           // Write Data from Memory
    int writeFromAlu;           // Write Data from ALU
    int writeRegister;          // Target Register Address
} P_Mem_Wb;

/**
 * @struct State
 * @brief Pipeline State
 */
typedef struct
{
    P_If_Id stage1;
    P_Id_Ex stage2;
    P_Ex_Mem stage3;
    P_Mem_Wb stage4;
} State;

/**
 * @struct BranchPredictor
 * @brief Branch Predictor
 */
typedef struct
{
    int pc;         // Program Counter
    int bt;         // Branch Target
    State state;    // State of Branch
} BranchPredictor;

// ---------- Instruction Functions ---------- //
/**
 * @brief msg
 * @param type name msg
 * @return type msg
 */
Instruction newInstruction(void);

/**
 * @brief msg
 * @param type name msg
 * @return type msg
 */
int getOpCode(int ins);

/**
 * @brief msg
 * @param type name msg
 * @return type msg
 */
int getRS(int ins);

/**
 * @brief msg
 * @param type name msg
 * @return type msg
 */
int getRT(int ins);

/**
 * @brief msg
 * @param type name msg
 * @return type msg
 */
int getRD(int ins);

/**
 * @brief msg
 * @param type name msg
 * @return type msg
 */
int getShamt(int ins);

/**
 * @brief msg
 * @param type name msg
 * @return type msg
 */
int getFunc(int ins);

/**
 * @brief msg
 * @param type name msg
 * @return type msg
 */
int getImmediate(int ins);

/**
 * @brief msg
 * @param type name msg
 * @return type msg
 */
char getType(int ins);

/**
 * @brief msg
 * @param type name msg
 * @return type msg
 */
char* getName(int ins);

/**
 * @brief msg
 * @param type name msg
 * @return type msg
 */
Instruction serializeInstruction(int ins);

/**
 * @brief msg
 * @param type name msg
 * @return type msg
 */
char* translateRegister(int reg);

/**
 * @brief msg
 * @param type name msg
 * @return type msg
 */
void deepCopyInstruction(Instruction *c, Instruction o); // (c)lone, (o)riginal;

/**
 * @brief msg
 * @param type name msg
 * @return type msg
 */
void printInstructionFormatted(Instruction ins);

/**
 * @brief msg
 * @param type name msg
 * @return type msg
 */
void printInstruction(Instruction);

/**
 * @brief msg
 * @param type name msg
 * @return type msg
 */
void printInstructionList(Instruction ins[MAX_INSTRUCTIONS]);

// ---------- Pipeline Functions ---------- //
/**
 * @brief msg
 * @param type name msg
 * @return type msg
 */
void addInstruction(Instruction i);

/**
 * @brief msg
 * @param type name msg
 * @return type msg
 */
void cycle(void);

/**
 * @brief msg
 * @param type name msg
 * @return type msg
 */
void runProgram(void);

/**
 * @brief msg
 * @param type name msg
 * @return type msg
 */
void initStage1(P_If_Id *s);

/**
 * @brief msg
 * @param type name msg
 * @return type msg
 */
void initStage2(P_Id_Ex *s);

/**
 * @brief msg
 * @param type name msg
 * @return type msg
 */
void initStage3(P_Ex_Mem *s);

/**
 * @brief msg
 * @param type name msg
 * @return type msg
 */
void initStage4(P_Mem_Wb *s);

/**
 * @brief msg
 * @param type name msg
 * @return type msg
 */
void initState(State *s);

/**
 * @brief msg
 * @param type name msg
 * @return type msg
 */
void deepCopyStage1(P_If_Id *c, P_If_Id o);

/**
 * @brief msg
 * @param type name msg
 * @return type msg
 */
void deepCopyStage2(P_Id_Ex *c, P_Id_Ex o);

/**
 * @brief msg
 * @param type name msg
 * @return type msg
 */
void deepCopyStage3(P_Ex_Mem *c, P_Ex_Mem o);

/**
 * @brief msg
 * @param type name msg
 * @return type msg
 */
void deepCopyStage4(P_Mem_Wb *c, P_Mem_Wb o);

/**
 * @brief msg
 * @param type name msg
 * @return type msg
 */
void deepCopyState(State *c, State o);

/**
 * @brief msg
 * @param type name msg
 * @return type msg
 */
void printState(State s);

// ---------- Tool Functions ---------- //
void init();

/**
 * @brief msg
 * @param type name msg
 * @return type msg
 */
int rightMostBits(int orig, int numBits);

/**
 * @brief msg
 * @param type name msg
 * @return type msg
 */
void parseInput();

// ---------- Debug Functions ---------- //
/**
 * @brief msg
 * @param type name msg
 * @return type msg
 */
void bin(unsigned n);

/*----------------------------------*
 *              GLOBALS             *
 *----------------------------------*/
Instruction INS[MAX_INSTRUCTIONS];
BranchPredictor BRANCHES[MAX_INSTRUCTIONS];
State currentState;
State newState;
int REGFILE[NUM_REGISTERS];
int DATAMEM[DATA_MEM];
int PC;

/*----------------------------------*
 *          IMPLEMENTATIONS         *
 *----------------------------------*/
// ---------- Instruction Implementations ---------- //
Instruction newInstruction(void)
{
    Instruction ins;

    ins.bt = 0;
    ins.func = 0;
    ins.imm = 0;
    ins.name = "noop";
    ins.opCode = 0;
    ins.rd = 0;
    ins.rs = 0;
    ins.rt = 0;
    ins.shamt = 0;
    ins.type = 0;
    
    return ins;
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
    if(getOpCode(ins) == 0 && (getFunc(ins) == OP_NOOP || getFunc(ins) == OP_HALT))
    {
        // Project Unique Op-Codes
        return 'X';
    }
    else if(getOpCode(ins) == 0 && getFunc(ins)>0)
    {   
        // R has opCode 0 because opCode is saved in Func segment
        return 'R';
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

    return "ERR"; // Invalid Input
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
    else if(SerIns.type == 'X')
    {
        SerIns.func = getFunc(ins);
    }

    return SerIns;
}

char* translateRegister(int reg)
{
    switch(reg)
    {
        case 0: return "$0";        // Zero/Null Register Constant
        case 1: return "$at";       // Assembler Temporary
        case 2: return "$v0";       // Function Results (0)
        case 3: return "$v1";       // Function Results (1)
        case 4: return "$a0";       // Arguments (0)
        case 5: return "$a1";       // Arguments (1)
        case 6: return "$a2";       // Arguments (2)
        case 7: return "$a3";       // Arguments (3)
        case 8: return "$t0";       // Temporaries (0)
        case 9: return "$t1";       // Temporaries (1)
        case 10: return "$t2";      // Temporaries (2)
        case 11: return "$t3";      // Temporaries (3)
        case 12: return "$t4";      // Temporaries (4)
        case 13: return "$t5";      // Temporaries (5)
        case 14: return "$t6";      // Temporaries (6)
        case 15: return "$t7";      // Temporaries (7)
        case 16: return "$s0";      // Save (0)
        case 17: return "$s1";      // Save (1)
        case 18: return "$s2";      // Save (2)
        case 19: return "$s3";      // Save (3)
        case 20: return "$s4";      // Save (4)
        case 21: return "$s5";      // Save (5)
        case 22: return "$s6";      // Save (6)
        case 23: return "$s7";      // Save (7)
        case 24: return "$t8";      // Temporaries (8)
        case 25: return "$t9";      // Temporaries (9)
        case 26: return "$k0";      // Kernal (0)
        case 27: return "$k1";      // Kernal (1)
        case 28: return "$gp";      // Global Pointer
        case 29: return "$sp";      // Stack Pointer
        case 30: return "$fp";      // Frame Pointer
        case 31: return "$ra";      // Return Address
        default: return "ERR";      // Invalid Register Error
    }
}

void deepCopyInstruction(Instruction *c, Instruction o)
{
    c->bt = o.bt;
    c->func = o.func;
    c->imm = o.imm;
    c->opCode = o.opCode;
    c->rd = o.rd;
    c->rs = o.rs;
    c->rt = o.rt;
    c->shamt = o.shamt;
    c->type = o.type;

    if(strcmp(o.name, "add") == 0)
        c->name = "add";
    else if(strcmp(o.name, "sub") == 0)
        c->name = "sub";
    else if(strcmp(o.name, "sll") == 0)
        c->name = "sll";
    else if(strcmp(o.name, "noop") == 0)
        c->name = "noop";
    else if(strcmp(o.name, "halt") == 0)
        c->name = "halt";
    else if(strcmp(o.name, "lw") == 0)
        c->name = "lw";
    else if(strcmp(o.name, "sw") == 0)
        c->name = "sw";
    else if(strcmp(o.name , "andi") == 0)
        c->name = "andi";
    else if(strcmp(o.name, "ori") == 0)
        c->name = "ori";
    else if(strcmp(o.name, "bne") == 0)
        c->name = "bne";
    else
        c->name = "ERR";
}

void printInstructionFormatted(Instruction ins)
{
    if(strcmp(ins.name, "add") == 0)
        printf("%s %s,%s,%s",ins.name, translateRegister(ins.rd),translateRegister(ins.rs), translateRegister(ins.rt));
    else if(strcmp(ins.name, "sub") == 0)
        printf("%s %s,%s,%s", ins.name, translateRegister(ins.rd), translateRegister(ins.rs), translateRegister(ins.rt));
    else if(strcmp(ins.name, "sll") == 0)
        printf("%s %s,%s,%d", ins.name, translateRegister(ins.rd),translateRegister(ins.rt), ins.imm);
    else if(strcmp(ins.name, "noop") == 0)
        printf("%s", "NOOP");
    else if(strcmp(ins.name, "halt") == 0)
        printf("%s", ins.name);
    else if(strcmp(ins.name, "lw") == 0)
        printf("%s %s, %d(%s)", ins.name, translateRegister(ins.rt), ins.imm, translateRegister(ins.rs));
    else if(strcmp(ins.name, "sw") == 0)
        printf("%s %s, %d(%s)", ins.name, translateRegister(ins.rt), ins.imm, translateRegister(ins.rs));
    else if(strcmp(ins.name, "andi") == 0)
        printf("%s %s,%s,%d", ins.name, translateRegister(ins.rt),translateRegister(ins.rs), ins.imm);
    else if(strcmp(ins.name, "ori") == 0)
        printf("%s %s,%s,%d", ins.name, translateRegister(ins.rt), translateRegister(ins.rs), ins.imm);
    else if(strcmp(ins.name, "bne") == 0)
        printf("%s %s,%s,%d", ins.name, translateRegister(ins.rs), translateRegister(ins.rt), ins.imm);
    else
        printf("[Error: Unsupported Instruction]");
}

void printInstruction(Instruction ins)
{
    printf("Name: %s\tType: %c\tRS: %d\tRT: %d\tRD: %d\tImmediate: %d\tBranch Target: %d\
    \tOpCode: %d\tFunc: %d\tShamt: %d\n", ins.name,ins.type, ins.rs, ins.rt, ins.rd, ins.imm,
    ins.bt, ins.opCode, ins.func, ins.shamt);
}

void printInstructionList(Instruction ins[MAX_INSTRUCTIONS])
{
    int i; 

    for( i = 0; ins[i-1].func !=  OP_HALT; i++)
    {
        printInstruction(ins[i]);
        printInstructionFormatted(ins[i]);
        printf("\n");
    }
}

// ---------- Pipeline Implementations ---------- //
void addInstruction(Instruction i)
{
    if( DEBUG_MODE ) printf("Adding Instruction %s", i.name);

    deepCopyInstruction(&newState.stage1.instruction, i);
    newState.stage1.pc4 = currentState.stage1.pc4 + 4;

    if( DEBUG_MODE ) printf("Done Adding Instruction");
}

void cycle(void)
{
    // Push Instructions Onward
    deepCopyInstruction(&newState.stage2.instruction, currentState.stage1.instruction);
    deepCopyInstruction(&newState.stage3.instruction, currentState.stage2.instruction);
    deepCopyInstruction(&newState.stage4.instruction, currentState.stage3.instruction);

    // Increment PC
    PC += 4;
    newState.stage1.pc4 = currentState.stage1.pc4 + 4;

    // Populate ID/EX Stage (Stage 2)
    newState.stage2.rs = newState.stage2.instruction.rs;
    newState.stage2.rt = newState.stage2.instruction.rt;
    newState.stage2.rd = newState.stage2.instruction.rd;
    newState.stage2.imm = newState.stage2.instruction.imm;
    newState.stage2.pc4 = currentState.stage1.pc4;
    
    // @todo BT, RD1, RD2

    // Populate EX/MEM Stage (Stage 3) @todo
    // Populate MEM/WB Stage (Stage 4) @todo
}

void runProgram(void)
{
    // Initialization Cycle
    printf("********************\n");
    printf("State at the beginning of cycle 1\n");
    printState(currentState);

    int i; 
    int haltInstruction = 0;

    // Iterate Through Instructions
    for( i = 0; currentState.stage4.instruction.func !=  OP_HALT; i++)
    {
        // Reset New State
        initState(&newState);

        // Add New instruction
        if(haltInstruction == 0)
        {
            if(INS[i].func ==  OP_HALT)
            {
                haltInstruction = 1;
            }
            addInstruction(INS[i]);
        }
        else
        {
            addInstruction(newInstruction());
        }

        // Execute One Pipeline Cycle
        cycle();

        // Make the New State the Current State
        deepCopyState(&currentState, newState);

        // Print Cycle Header
        printf("********************\n");
        printf("State at the beginning of cycle %d\n", i+2);

        // Print State
        printState(currentState);
    }
}

void initStage1(P_If_Id *s)
{
    s->instruction = newInstruction();
    s->pc4 = 0;
}

void initStage2(P_Id_Ex *s)
{
    s->bt = 0;
    s->imm = 0;
    s->pc4 = 0;
    s->rd = 0;
    s->rs = 0;
    s->rt = 0;
    s->read1 = 0;
    s->read2 = 0;
    s->instruction = newInstruction();
}

void initStage3(P_Ex_Mem *s)
{
    s->aluRes = 0;
    s->wd = 0;
    s->wr = 0;
    s->instruction = newInstruction();
}

void initStage4(P_Mem_Wb *s)
{
    s->writeFromAlu = 0;
    s->writeFromMem = 0;
    s->writeRegister = 0;
    s->instruction = newInstruction();
}

void initState(State *s)
{
    initStage1(&s->stage1);
    initStage2(&s->stage2);
    initStage3(&s->stage3);
    initStage4(&s->stage4);
}

void deepCopyStage1(P_If_Id *c, P_If_Id o)
{
    deepCopyInstruction(&c->instruction, o.instruction);
    c->pc4 = o.pc4;
}

void deepCopyStage2(P_Id_Ex *c, P_Id_Ex o)
{
    c->bt = o.bt;
    c->imm = o.imm;
    c->pc4 = o.pc4;
    c->rd = o.rd;
    c->rs = o.rs;
    c->rt = o.rt;
    c->read1 = o.read1;
    c->read2 = o.read2;
    deepCopyInstruction(&c->instruction, o.instruction);
}

void deepCopyStage3(P_Ex_Mem *c, P_Ex_Mem o)
{
    c->aluRes = o.aluRes;
    c->wd = o.wd;
    c->wr = o.wr;
    deepCopyInstruction(&c->instruction, o.instruction);
}

void deepCopyStage4(P_Mem_Wb *c, P_Mem_Wb o)
{
    c->writeFromAlu = o.writeFromAlu;
    c->writeFromMem = o.writeFromMem;
    c->writeRegister = o.writeRegister;
    deepCopyInstruction(&c->instruction, o.instruction);
}

void deepCopyState(State *c, State o)
{
    deepCopyStage1(&c->stage1, o.stage1);
    deepCopyStage2(&c->stage2, o.stage2);
    deepCopyStage3(&c->stage3, o.stage3);
    deepCopyStage4(&c->stage4 ,o.stage4);
}

void printState(State s)
{
    // Iterator Declaration
    int i;

    // Program Counter
    printf("\tPC = %d\n", PC);

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

    // Print Pipe Stage 1 (IF/ID)
    printf("\tIF/ID:\n");
    printf("\t\tInstruction: ");
    printInstructionFormatted(s.stage1.instruction);
    printf("\n\t\tPCPlus4: %d\n", s.stage1.pc4);

    // Print Pipe Stage 2 (ID/EX)
    printf("\tID/EX:\n");
    printf("\t\tInstruction: ");
    printInstructionFormatted(s.stage2.instruction);
    printf("\n\t\tPCPlus4: %d\n", s.stage2.pc4);
    printf("\t\tbranchTarget: %d\n", s.stage2.bt);
    printf("\t\treadData1: %d\n", s.stage2.read1);
    printf("\t\treadData2: %d\n", s.stage2.read2);
    printf("\t\timmed: %d\n", s.stage2.imm);
    printf("\t\trs: %s\n", translateRegister(s.stage2.rs));
    printf("\t\trt: %s\n", translateRegister(s.stage2.rt));
    printf("\t\trd: %s\n", translateRegister(s.stage2.rd));

    // Print Pipe Stage 3 (EX/MEM)
    printf("\tEX/MEM:\n");
    printf("\t\tInstruction: ");
    printInstructionFormatted(s.stage3.instruction);
    printf("\n\t\taluResult: %d\n", s.stage3.aluRes);
    printf("\t\twriteDataReg: %d\n", s.stage3.wd);
    printf("\t\twriteReg: %s\n", translateRegister(s.stage3.wr));

    // Print Pipe Stage 4 (MEM/WB)
    printf("\tMEM/WB\n");
    printf("\t\tInstruction: ");
    printInstructionFormatted(s.stage4.instruction);
    printf("\n\t\twriteDataMem: %d\n", s.stage4.writeFromMem);
    printf("\t\twriteDataALU: %d\n", s.stage4.writeFromAlu);
    printf("\t\twriteReg: %d\n", s.stage4.writeFromMem);
}

// ---------- Tool Implementations ---------- //
void init(void)
{
    // Initialize Registers
    int i;
    for(i = 0; i < NUM_REGISTERS; i++)
    {
        REGFILE[i] = 0;
    }

    // Initialize Data Memory
    for(i = 0; i < DATA_MEM; i++)
    {
        DATAMEM[i] = 0;
    }

    // Initialize Program Counter
    PC = 0; 

    // Initialize States
    initState(&currentState);
    initState(&newState);
}

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

void parseInput(void)
{
    char lineBuffer[256];

    // Parse Instructions
    int i = 0;
    while(fgets(lineBuffer,MAX_INSTRUCTIONS, stdin))
    {
        INS[i] = serializeInstruction(atoi(lineBuffer));
        if(INS[i].func == OP_HALT)
        {
            break;
        }

        i++;
    }

    // Skip Blank Line
    fgets(lineBuffer,MAX_INSTRUCTIONS, stdin);

    // Parse Data Segments
    i = 0;
    while(fgets(lineBuffer,MAX_INSTRUCTIONS, stdin))
    {
        DATAMEM[i] = atoi(lineBuffer);
        i++;
    }
}

// ---------- Debug Functions ---------- //
void bin(unsigned n)
{ 
    unsigned i; 
    for (i = 1 << 31; i > 0; i = i / 2) 
        (n & i)? printf("1"): printf("0"); 
    printf("\n");
}

/*----------------------------------*
 *                MAIN              *
 *----------------------------------*/
int main()
{
    // Initialize Register Values
    init();

    // Parse MIPS Machine Code from STDIN to Global Arrays
    if( DEBUG_MODE ) printf("Input Parse Begining\n");
    parseInput();
    if( DEBUG_MODE ) printf("Input Parse Complete\n");

    // Print Instructions (Debug)
    if( DEBUG_MODE ) printInstructionList(INS);

    runProgram();
}
