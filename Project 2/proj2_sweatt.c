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
#define DEBUG_MODE 0
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
 * @brief Serialized structure for an instruction.
 */
typedef struct
{
    char* name;          // Instruction Code Representation
    char type;           // Instruction Type
    int rs;              // RS Register
    int rt;              // RT Register
    int rd;              // RD Register
    int16_t imm;         // Immediate
    int16_t bt;          // Branch Target
    int opCode;          // Operation Code
    int func;            // Function Code
    int shamt;           // Shift
    int raw;             // Full Unprocessed Instruction
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
    int16_t bt;                 // Branch Target
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
 * @brief Set a new instruction.
 * @return Instruction Returns an empty NOOP instruction.
 */
Instruction newInstruction(void);

/**
 * @brief Extract opcode from an unserialized instruction.
 * @param int ins Unserialized instruction.
 * @return int Opcode bits.
 */
int getOpCode(int ins);

/**
 * @brief Extract RS register from an unserialized instruction.
 * @param int ins Unserialized instruction.
 * @return int RS register bits.
 */
int getRS(int ins);

/**
 * @brief Extract RT register from an unserialized instruction.
 * @param int ins Unserialized instruction.
 * @return int RT register bits.
 */
int getRT(int ins);

/**
 * @brief Extract RD register from an unserialized instruction.
 * @param int ins Unserialized instruction.
 * @return int RD register bits.
 */
int getRD(int ins);

/**
 * @brief Extract shamt/shift from an unserialized instruction.
 * @param int ins Unserialized instruction.
 * @return int shamt/shift bits.
 */
int getShamt(int ins);

/**
 * @brief Extract func code from an unserialized instruction.
 * @param int ins Unserialized instruction.
 * @return int Func code bits.
 */
int getFunc(int ins);

/**
 * @brief Extract immediate from an unserialized instruction.
 * @param int ins Unserialized instruction.
 * @return int Immediate bits.
 */
int getImmediate(int ins);

/**
 * @brief Deduce what type an unserialized instruction is.
 * @param int ins Unserialized instruction.
 * @return char Returns the instruction type, or X if NOOP/HALT.
 */
char getType(int ins);

/**
 * @brief Deduce the name of an unserialized instruction.
 * @param int ins Unserialized instruction.
 * @return char* Returns the name of the instruction as a C-String.
 */
char* getName(int ins);

/**
 * @brief Serialize the integer representation of an instruction into an Instruction structure.
 * @param int ins Unserialized instruction.
 * @return Instruction Serialized Instruction.
 */
Instruction serializeInstruction(int ins);

/**
 * @brief Deduce the readable version of a register code.
 * @param int reg Register integer representation. 
 * @return char* Returns the name of the register as a c-string.
 */
char* translateRegister(int reg);

/**
 * @brief Deduce the which register is the write register of the instruction.
 * @param Instruction i
 * @return int Returns integer representation of whatever register will be written to.
 */
int getWriteRegister(Instruction i);

/**
 * @brief Get the read1/read2 data for stage 2.
 * @param Instruction ins The instruction running.
 * @param int n Read (1) or Read (2) specification. 
 * @return int Read data.
 */
int getReadData(Instruction ins, int n);

/**
 * @brief Copy the contents of one instruction to another.
 * @param Instruction *c Instruction targeted for (c)opy.
 * @param Instruction o Original instruction being copied.
 * @return void
 */
void deepCopyInstruction(Instruction *c, Instruction o); // (c)lone, (o)riginal;

/**
 * @brief Print the contents of an instruction in MIPS format.
 * @param Instruction ins
 * @return void
 */
void printInstructionFormatted(Instruction ins);

/**
 * @brief Print the contents of an instruction in full debug format.
 * @param Instruction ins
 * @return void
 */
void printInstruction(Instruction);

/**
 * @brief Print the contents of all parsed instructions.
 * @param Instruction ins
 * @return void
 */
void printInstructionList(Instruction ins[MAX_INSTRUCTIONS]);

// ---------- Pipeline Functions ---------- //
/**
 * @brief Add an instruction to the global INS list
 * @param Instruction i Instruction to be added to the list.
 * @return void
 */
void addInstruction(Instruction i);

/**
 * @brief Execute the ALU op for this instruction.
 * @param Instruction i Instruction to be executed.
 * @return int Returns the result of the instruction's ALU operation. 
 */
int aluOp(Instruction i);

/**
 * @brief Read a register value.
 * @param int reg Register number to read.
 * @return int Contents of the register read.
 */
int readRegister(int reg);

/**
 * @brief Read a memory value.
 * @param int address Memory address to read.
 * @return int Retrieved/read memory value.
 */
int readMemory(int address);

/**
 * @brief Write to memory.
 * @param int address Address in memory to write to.
 * @return int content Data to write to memory.
 */
void writeToMemory(int address, int content);

/**
 * @brief Write to a particular register.
 * @param P_Mem_Wb MEM/WB stage structure to provide write information.
 * @return void
 */
void writeToRegister(P_Mem_Wb s);

/**
 * @brief Read the appropriate information if LW.
 * @param P_Ex_Mem s Information from the read stage.
 * @return int Read data or 0 if no read is done.
 */
int getWriteMem(P_Ex_Mem s);

/**
 * @brief Run a stalled cycle, injecting a NOOP operation into stage 2.
 * @return void
 */
void stall(void);

/**
 * @brief Execute a single cycle.
 * @return void.
 */
void cycle(void);

/**
 * @brief Run a parsed program in INS.
 * @return void.
 */
void runProgram(void);

/**
 * @brief Initialize an IF/ID stage struct.
 * @param P_If_Id *s IF/ID Stage 1 struct to be initialized.
 * @return void
 */
void initStage1(P_If_Id *s);

/**
 * @brief Initialize an ID/EX stage struct.
 * @param P_Id_Ex *s ID/EX Stage 2 struct to be initialized.
 * @return void
 */
void initStage2(P_Id_Ex *s);

/**
 * @brief Initialize an EX/MEM stage struct.
 * @param P_Ex_Mem *s EX/MEM Stage 3 struct to be initialized.
 * @return void
 */
void initStage3(P_Ex_Mem *s);

/**
 * @brief Initialize an MEM/WB stage struct.
 * @param P_Mem_Wb *s MEM/WB Stage 4 struct to be initialized.
 * @return void
 */
void initStage4(P_Mem_Wb *s);

/**
 * @brief Initialize a state (all stages) struct.
 * @param State *s State to initialize.
 * @return void
 */
void initState(State *s);

/**
 * @brief Deep copy a stage 1 structure to another stage 1 structure.
 * @param P_If_Id *c Structure targeted to be (c)opied to.
 * @param P_If_Id o Structure being copied
 * @return void
 */
void deepCopyStage1(P_If_Id *c, P_If_Id o);

/**
 * @brief Deep copy a stage 2 structure to another stage 2 structure.
 * @param P_Id_Ex *c Structure targeted to be (c)opied to.
 * @param P_Id_Ex o Structure being copied
 * @return void
 */
void deepCopyStage2(P_Id_Ex *c, P_Id_Ex o);

/**
 * @brief Deep copy a stage 3 structure to another stage 3 structure.
 * @param P_Ex_Mem *c Structure targeted to be (c)opied to.
 * @param P_Ex_Mem o Structure being copied
 * @return void
 */
void deepCopyStage3(P_Ex_Mem *c, P_Ex_Mem o);

/**
 * @brief Deep copy a stage 4 structure to another stage 4 structure.
 * @param P_Mem_Wb *c Structure targeted to be (c)opied to.
 * @param P_Mem_Wb o Structure being copied
 * @return void
 */
void deepCopyStage4(P_Mem_Wb *c, P_Mem_Wb o);

/**
 * @brief Deep copy all stages of a state to another state.
 * @param State *c State targeted to be (c)opied to.
 * @param State o (O)riginal state targeted to be (c)opied to.
 * @return void
 */
void deepCopyState(State *c, State o);

/**
 * @brief Print a state in a formatted manner.
 * @param State s State to print.
 * @return void
 */
void printState(State s);

// ---------- Tool Functions ---------- //
/**
 * @brief Initialize the simulator to run a program.
 * @return void
 */
void init();

/**
 * @brief Get a specified number of right-most bits.
 * @param int orig Number to get data from.
 * @param int numBits Number of bits to extract.
 * @return int The rightmost (numbits) of (orig).
 */
int rightMostBits(int orig, int numBits);

/**
 * @brief Parse standard input into an INS instruction array.
 * @return void
 */
void parseInput();

// ---------- Debug Functions ---------- //
/**
 * @brief Debug function used to display an int in binary.
 * @param unsigned n Unsigned integer to display.
 * @return void.
 * @author G4G
 */
void bin(unsigned n);

/*----------------------------------*
 *              GLOBALS             *
 *----------------------------------*/
Instruction INS[MAX_INSTRUCTIONS];
BranchPredictor BRANCHES[MAX_INSTRUCTIONS];
State currentState;
State newState;
char HALTING;
int REGFILE[NUM_REGISTERS];
int DATAMEM[DATA_MEM];
int PC;
int NUM_INSTRUCTIONS;
int CYCLE_COUNT;
int STALL_COUNT;
int FORWARD_A;
int FORWARD_B;

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
    ins.raw = 0;

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
    if(ins < 2 && getOpCode(ins) == 0 && (getFunc(ins) == OP_NOOP || getFunc(ins) == OP_HALT))
    {
        // Project Unique Op-Codes
        return 'X';
    }
    else if(getOpCode(ins) == 0)
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
    
    SerIns.raw = ins;

    SerIns.name = getName(ins);
    SerIns.type = getType(ins);
    SerIns.opCode = getOpCode(ins);
    SerIns.rs = getRS(ins);
    SerIns.rt = getRT(ins);
    SerIns.imm = getImmediate(ins);

    if(SerIns.type == 'R')
    {
        SerIns.rd = getRD(ins);
        SerIns.shamt = getShamt(ins);
        SerIns.func = getFunc(ins);
    }
    else if(SerIns.type == 'I')
    {
        // Remaining Nulls
        SerIns.rd = 0;
        SerIns.shamt = 0;
        SerIns.func = 0;
    }
    else if(SerIns.type == 'X')
    {
        SerIns.func = getFunc(ins);
        SerIns.imm = getFunc(ins);
        SerIns.rs = 0;
        SerIns.rt = 0;
        SerIns.rd = 0;
        SerIns.shamt = 0;
    }

    return SerIns;
}

char* translateRegister(int reg)
{
    switch(reg)
    {
        case 0: return "0";        // Zero/Null Register Constant
        case 1: return "at";       // Assembler Temporary
        case 2: return "v0";       // Function Results (0)
        case 3: return "v1";       // Function Results (1)
        case 4: return "a0";       // Arguments (0)
        case 5: return "a1";       // Arguments (1)
        case 6: return "a2";       // Arguments (2)
        case 7: return "a3";       // Arguments (3)
        case 8: return "t0";       // Temporaries (0)
        case 9: return "t1";       // Temporaries (1)
        case 10: return "t2";      // Temporaries (2)
        case 11: return "t3";      // Temporaries (3)
        case 12: return "t4";      // Temporaries (4)
        case 13: return "t5";      // Temporaries (5)
        case 14: return "t6";      // Temporaries (6)
        case 15: return "t7";      // Temporaries (7)
        case 16: return "s0";      // Save (0)
        case 17: return "s1";      // Save (1)
        case 18: return "s2";      // Save (2)
        case 19: return "s3";      // Save (3)
        case 20: return "s4";      // Save (4)
        case 21: return "s5";      // Save (5)
        case 22: return "s6";      // Save (6)
        case 23: return "s7";      // Save (7)
        case 24: return "t8";      // Temporaries (8)
        case 25: return "t9";      // Temporaries (9)
        case 26: return "k0";      // Kernal (0)
        case 27: return "k1";      // Kernal (1)
        case 28: return "gp";      // Global Pointer
        case 29: return "sp";      // Stack Pointer
        case 30: return "fp";      // Frame Pointer
        case 31: return "ra";      // Return Address
        default: return "ERR";      // Invalid Register Error
    }
}

int getWriteRegister(Instruction i)
{
    if(strcmp(i.name, "add") == 0)
        return i.rd;
    else if(strcmp(i.name, "sub") == 0)
        return i.rd;
    else if(strcmp(i.name, "sll") == 0)
        return i.rd;
    else if(strcmp(i.name, "noop") == 0)
        return 0;
    else if(strcmp(i.name, "halt") == 0)
        return 0;
    else if(strcmp(i.name, "lw") == 0)
        return i.rt;
    else if(strcmp(i.name, "sw") == 0)
        return i.rt;
    else if(strcmp(i.name , "andi") == 0)
        return i.rt;
    else if(strcmp(i.name, "ori") == 0)
        return i.rt;
    else if(strcmp(i.name, "bne") == 0)
        return 0;
    else
        return 0;
}

int getReadData(Instruction ins, int n)
{
    if(strcmp(ins.name, "lw") == 0)
    {
        if(n == 1)
        {
            if(DEBUG_MODE){printf("[DEBUG]: Register Read %d\n",readRegister(ins.rs));}
            return readRegister(ins.rs);
        }
        else
            return 0;
    }
    else if (strcmp(ins.name, "sll") == 0)\
    {
        return readRegister(ins.rt);
    }
    else if(strcmp(ins.name, "bne") == 0)
        return 0;
    else
    {
        if(n == 1)
            return readRegister(ins.rs);
        else if(n == 2)
            return readRegister(ins.rt);
        else
            return 0;
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
    c->raw = o.raw;

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
        printf("%s $%s,$%s,$%s",ins.name, translateRegister(ins.rd),translateRegister(ins.rs), translateRegister(ins.rt));
    else if(strcmp(ins.name, "sub") == 0)
        printf("%s $%s,$%s,$%s", ins.name, translateRegister(ins.rd), translateRegister(ins.rs), translateRegister(ins.rt));
    else if(strcmp(ins.name, "sll") == 0)
        printf("%s $%s,$%s,%d", ins.name, translateRegister(ins.rd),translateRegister(ins.rt), ins.shamt);
    else if(strcmp(ins.name, "noop") == 0)
        printf("%s", "NOOP");
    else if(strcmp(ins.name, "halt") == 0)
        printf("%s", ins.name);
    else if(strcmp(ins.name, "lw") == 0)
        printf("%s $%s, %d($%s)", ins.name, translateRegister(ins.rt), ins.imm, translateRegister(ins.rs));
    else if(strcmp(ins.name, "sw") == 0)
        printf("%s $%s, %d($%s)", ins.name, translateRegister(ins.rt), ins.imm, translateRegister(ins.rs));
    else if(strcmp(ins.name, "andi") == 0)
        printf("%s $%s,$%s,%d", ins.name, translateRegister(ins.rt),translateRegister(ins.rs), ins.imm);
    else if(strcmp(ins.name, "ori") == 0)
        printf("%s $%s,$%s,%d", ins.name, translateRegister(ins.rt), translateRegister(ins.rs), ins.imm);
    else if(strcmp(ins.name, "bne") == 0)
        printf("%s $%s,$%s,%d", ins.name, translateRegister(ins.rs), translateRegister(ins.rt), ins.imm);
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
    if(!HALTING)
        newState.stage1.pc4 = PC + 4;
    else
        newState.stage1.pc4 = PC;

    if( DEBUG_MODE ) printf("Done Adding Instruction");
}

int aluOp(Instruction i)
{
    if(strcmp(i.name, "add") == 0)
    {
        int op1 = 0;
        int op2 = 0;
        if((FORWARD_A & 0b10) == 0b10 && (FORWARD_B & 0b10) != 0b10)
        {
            FORWARD_A = FORWARD_A & 0b01;
            op1 = currentState.stage3.aluRes;
            op2 = currentState.stage2.read2;
        }
        else if((FORWARD_A & 0b10) != 0b10 && (FORWARD_B & 0b10) == 0b10)
        {
            FORWARD_B = FORWARD_B & 0b01;
            op1 = currentState.stage2.read1;
            op2 = currentState.stage3.aluRes;
        }
        else
        {
            op1 = currentState.stage2.read1;
            op2 = currentState.stage2.read2;
        }
        
        // Double Look-Ahead
        if((FORWARD_A & 0b01) == 0b01 && (FORWARD_B & 0b01) != 0b01)
        {
            FORWARD_A = FORWARD_A & 0b10;
            if(strcmp(currentState.stage4.instruction.name, "lw")==0)
                op1 = currentState.stage4.writeFromMem;
            else
                op2 = currentState.stage4.writeFromAlu;
        }
        else if((FORWARD_A & 0b01) != 0b01 && (FORWARD_B & 0b01) == 0b01)
        {
            FORWARD_B = FORWARD_B & 0b10;
            if(strcmp(currentState.stage4.instruction.name, "lw")==0)
                op2 = currentState.stage4.writeFromMem;
            else
                op2 = currentState.stage4.writeFromAlu;
        }

        return op1 + op2;
    }
    else if(strcmp(i.name, "sub") == 0)
    {
        int op1 = 0;
        int op2 = 0;
        if((FORWARD_A & 0b10) == 0b10 && (FORWARD_B & 0b10) != 0b10)
        {
            FORWARD_A = FORWARD_A & 0b01;
            op1 = currentState.stage3.aluRes;
            op2 = currentState.stage2.read2;
        }
        else if((FORWARD_A & 0b10) != 0b10 && (FORWARD_B & 0b10) == 0b10)
        {
            FORWARD_B = FORWARD_B & 0b01;
            op1 = currentState.stage2.read1;
            op2 = currentState.stage3.aluRes;
        }
        else
        {
            op1 = currentState.stage2.read1;
            op2 = currentState.stage2.read2;
        }
        
        // Double Look-Ahead
        if((FORWARD_A & 0b01) == 0b01 && (FORWARD_B & 0b01) != 0b01)
        {
            FORWARD_A = FORWARD_A & 0b10;
            op1 = currentState.stage4.writeFromAlu;
        }
        else if((FORWARD_A & 0b01) != 0b01 && (FORWARD_B & 0b01) == 0b01)
        {
            FORWARD_B = FORWARD_B & 0b10;
            op2 = currentState.stage4.writeFromAlu;
        }

        return op1 - op2;
    }
    else if(strcmp(i.name, "sll") == 0)
    {
        if((FORWARD_B & 0b10) == 0b10)
        {
            FORWARD_B = FORWARD_B & 0b01;
            return currentState.stage3.aluRes << currentState.stage2.instruction.shamt;
        }

        // Double Look-Ahead
        if((FORWARD_B & 0b01) == 0b01)
        {
            FORWARD_B = FORWARD_B & 0b10;
            return currentState.stage4.writeFromAlu  << currentState.stage2.instruction.shamt;
        }
        return currentState.stage2.read1 << currentState.stage2.instruction.shamt;
    }
    else if(strcmp(i.name, "noop") == 0)
        return 0;
    else if(strcmp(i.name, "halt") == 0)
        return 0;
    else if(strcmp(i.name, "lw") == 0)
    {
        if((FORWARD_A & 0b10) == 0b10)
        {
            FORWARD_A = FORWARD_A & 0b01;
            return currentState.stage3.aluRes + currentState.stage2.imm;
        }

        // Double Look-Ahead
        if((FORWARD_A & 0b01) == 0b01)
        {
            FORWARD_A = FORWARD_A & 0b10;
            return currentState.stage4.writeFromAlu + currentState.stage2.imm;;
        }

        return currentState.stage2.read1 + currentState.stage2.imm;
    }
    else if(strcmp(i.name, "sw") == 0)
    {        
        // Save Word Only Ever Needs An ALU Forward_A Lookup
        if((FORWARD_A & 0b10) == 0b10)
        {
            FORWARD_A = FORWARD_A & 0b01;
            return currentState.stage3.aluRes + currentState.stage2.imm;
        }

        // Double Look-Ahead
        if((FORWARD_A & 0b01) == 0b01)
        {
            FORWARD_A = FORWARD_A & 0b10;
            return currentState.stage4.writeFromAlu + currentState.stage2.imm;;
        }

        return currentState.stage2.read1 + currentState.stage2.imm;
    }
    else if(strcmp(i.name , "andi") == 0)
    {
        // I-Types Only Ever Needs An ALU Forward_A Lookup
        if((FORWARD_A & 0b10) == 0b10)
        {
            FORWARD_A = FORWARD_A & 0b01;
            return currentState.stage3.aluRes & currentState.stage2.imm;
        }

        // Double Look-Ahead
        if((FORWARD_A & 0b01) == 0b01)
        {
            FORWARD_A = FORWARD_A & 0b10;
            return currentState.stage4.writeFromAlu & currentState.stage2.imm;
        }

        return currentState.stage2.read1 & currentState.stage2.imm;
    }
    else if(strcmp(i.name, "ori") == 0)
    {
        // I-Types Only Ever Needs An ALU Forward_A Lookup
        if((FORWARD_A & 0b10) == 0b10)
        {
            FORWARD_A = FORWARD_A & 0b01;
            return currentState.stage3.aluRes | currentState.stage2.imm;
        }

        // Double Look-Ahead
        if((FORWARD_A & 0b01) == 0b01)
        {
            FORWARD_A = FORWARD_A & 0b10;
            return currentState.stage4.writeFromAlu | currentState.stage2.imm;
        }
        return currentState.stage2.read1 | i.imm;
    }
    else if(strcmp(i.name, "bne") == 0)
        return 0;
    else
        return 0;
}

int readRegister(int reg)
{
    int target = reg;
    if(target > -1 && target < NUM_REGISTERS)
        return REGFILE[target];
    else
        return 0;
}

int readMemory(int address)
{
    int target = (address - (NUM_INSTRUCTIONS*4))/4;
    if(target > -1 && target < DATA_MEM)
        return DATAMEM[target];
    else
        return 0;
}

void writeToMemory(int address, int content)
{
    int target = (address - (NUM_INSTRUCTIONS*4))/4;
    if(target > -1 && target < DATA_MEM)
    {
        DATAMEM[target] = content;
    }
}

void writeToRegister(P_Mem_Wb s)
{
    if(strcmp(s.instruction.name, "add") == 0)
        REGFILE[s.writeRegister] = s.writeFromAlu;
    else if(strcmp(s.instruction.name, "sub") == 0)
        REGFILE[s.writeRegister] = s.writeFromAlu;
    else if(strcmp(s.instruction.name, "sll") == 0)
        REGFILE[s.writeRegister] = s.writeFromAlu;
    else if(strcmp(s.instruction.name, "noop") == 0)
        return;
    else if(strcmp(s.instruction.name, "halt") == 0)
        return;
    else if(strcmp(s.instruction.name, "lw") == 0)
        REGFILE[s.writeRegister] = s.writeFromMem;
    else if(strcmp(s.instruction.name, "sw") == 0)
        return;
    else if(strcmp(s.instruction.name , "andi") == 0)
        REGFILE[s.writeRegister] = s.writeFromAlu;
    else if(strcmp(s.instruction.name, "ori") == 0)
        REGFILE[s.writeRegister] = s.writeFromAlu;
    else if(strcmp(s.instruction.name, "bne") == 0)
        return;
    else
        return;
}

int getWriteMem(P_Ex_Mem s)
{
    if(strcmp(s.instruction.name, "lw") == 0)
        return readMemory(s.aluRes);
    else
        return 0;
}

void stall(void)
{
    // Write Registers
    writeToRegister(currentState.stage4);

    // Increment Analytics Counter
    STALL_COUNT++;
    CYCLE_COUNT++;

    // Push Instructions Onward
    deepCopyInstruction(&newState.stage1.instruction, currentState.stage1.instruction);
    deepCopyInstruction(&newState.stage2.instruction, newInstruction());
    deepCopyInstruction(&newState.stage3.instruction, currentState.stage2.instruction);
    deepCopyInstruction(&newState.stage4.instruction, currentState.stage3.instruction);

    // Increment PC
    newState.stage1.pc4 = PC;

    // Populate ID/EX Stage (Stage 2)
    newState.stage2.rs = newState.stage2.instruction.rs;
    newState.stage2.rt = newState.stage2.instruction.rt;
    newState.stage2.rd = newState.stage2.instruction.rd;
    newState.stage2.imm = newState.stage2.instruction.imm;
    newState.stage2.pc4 = currentState.stage1.pc4;
    newState.stage2.read1 = getReadData(newState.stage2.instruction,1);
    newState.stage2.read2 = getReadData(newState.stage2.instruction,2);
    newState.stage2.bt = currentState.stage1.pc4 + ((newState.stage2.instruction.raw << 2) & 0xFFFF);

    // Populate EX/MEM Stage (Stage 3)
    newState.stage3.aluRes = aluOp(newState.stage3.instruction);
    newState.stage3.wd = currentState.stage2.read2;
    newState.stage3.wr = getWriteRegister(newState.stage3.instruction); 

    // Populate MEM/WB Stage (Stage 4)
    newState.stage4.writeFromMem = getWriteMem(currentState.stage3);
    newState.stage4.writeFromAlu = currentState.stage3.aluRes;
    newState.stage4.writeRegister = currentState.stage3.wr;

    // Write Memory
    if(strcmp(newState.stage4.instruction.name, "sw") == 0)
        writeToMemory(newState.stage4.writeFromAlu, currentState.stage3.wd);
}

void cycle(void)
{
    // Write Registers
    writeToRegister(currentState.stage4);

    // Increment Analytics Counter
    CYCLE_COUNT++;

    // Push Instructions Onward
    deepCopyInstruction(&newState.stage2.instruction, currentState.stage1.instruction);
    deepCopyInstruction(&newState.stage3.instruction, currentState.stage2.instruction);
    deepCopyInstruction(&newState.stage4.instruction, currentState.stage3.instruction);

    // Increment PC
    PC += 4;

    if(!HALTING)
        newState.stage1.pc4 = currentState.stage1.pc4 + 4;
    else
        newState.stage1.pc4 = currentState.stage1.pc4;

    // Populate ID/EX Stage (Stage 2)
    newState.stage2.rs = newState.stage2.instruction.rs;
    newState.stage2.rt = newState.stage2.instruction.rt;
    newState.stage2.rd = newState.stage2.instruction.rd;
    newState.stage2.imm = newState.stage2.instruction.imm;
    newState.stage2.pc4 = currentState.stage1.pc4;
    newState.stage2.read1 = getReadData(newState.stage2.instruction,1);
    newState.stage2.read2 = getReadData(newState.stage2.instruction,2);
    newState.stage2.bt = currentState.stage1.pc4 + ((newState.stage2.instruction.raw << 2) & 0xFFFF);

    // Populate EX/MEM Stage (Stage 3)
    newState.stage3.aluRes = aluOp(newState.stage3.instruction);
    if((FORWARD_B & 0b10) == 0b10)
    {
        FORWARD_B = FORWARD_B & 0b01;
        newState.stage3.wd = currentState.stage3.aluRes;
    }
    else if((FORWARD_B & 0b01) == 0b01)
    {
        FORWARD_B = FORWARD_B & 0b10;
        newState.stage3.wd = currentState.stage4.writeFromAlu;
    }
    else
        newState.stage3.wd = currentState.stage2.read2;
    newState.stage3.wr = getWriteRegister(newState.stage3.instruction);

    // Check for Forwarding Hazards
    if(newState.stage3.wr && newState.stage3.wr == newState.stage2.rs)
    {
        FORWARD_A = FORWARD_A | 0b10;
        if(DEBUG_MODE){printf("\n\n\nForwarding (A) %d by %d to %s\n\n\n", newState.stage3.aluRes, FORWARD_A, newState.stage2.instruction.name);}
    }
    if(newState.stage3.wr && newState.stage3.wr == newState.stage2.rt)
    {
        if(DEBUG_MODE)printf("\n\n\nForwarding (B) %d to %s\n\n\n", newState.stage3.aluRes,newState.stage2.instruction.name);
        FORWARD_B = FORWARD_B | 0b10;
    }

    // Populate MEM/WB Stage (Stage 4)
    newState.stage4.writeFromMem = getWriteMem(currentState.stage3);
    newState.stage4.writeFromAlu = currentState.stage3.aluRes;
    newState.stage4.writeRegister = currentState.stage3.wr;

    // Check for Double Forwarding Hazards
    if(newState.stage4.writeFromAlu && newState.stage4.writeRegister == newState.stage2.rs)
    {
        if(DEBUG_MODE){printf("\n\n\n Double Forwarding (A) %d to %s\n\n\n", newState.stage4.writeFromAlu, currentState.stage2.instruction.name);}
        FORWARD_A = FORWARD_A | 0b01;
    }
    else if(newState.stage4.writeFromAlu && newState.stage4.writeRegister == newState.stage2.rt)
    {
        if(DEBUG_MODE)printf("\n\n\n Double Forwarding (B) %d to %s\n\n\n", newState.stage4.writeFromAlu,currentState.stage2.instruction.name);
        FORWARD_B = FORWARD_B | 0b01;
    }

    // Write Memory
    if(strcmp(newState.stage4.instruction.name, "sw") == 0)
        writeToMemory(newState.stage4.writeFromAlu, currentState.stage3.wd);
}

void runProgram(void)
{
    // Initialization Cycle
    printf("********************\n");
    printf("State at the beginning of cycle 1\n");
    printState(currentState);
    CYCLE_COUNT++;

    int i; 
    int prepareHalt = 0;

    // Iterate Through Instructions
    for( i = 0; currentState.stage4.instruction.func !=  OP_HALT; i++)
    {
        // Reset New State
        initState(&newState);

        // Add New instruction
        if(!prepareHalt)
        {
            if(INS[i].func ==  OP_HALT)
            {
                prepareHalt = 1;
            }
            addInstruction(INS[i]);
        }
        else
        {
            HALTING = 1;
            addInstruction(newInstruction());
        }

        // Check for Stalling Hazards
        if(strcmp(currentState.stage2.instruction.name, "lw")==0 && ((currentState.stage1.instruction.rs ==
        currentState.stage2.instruction.rt && currentState.stage2.instruction.rt != getWriteRegister(currentState.stage2.instruction)) 
        || (currentState.stage1.instruction.rt == currentState.stage2.instruction.rt  && currentState.stage2.instruction.rt
        != getWriteRegister(currentState.stage2.instruction))))
        {
            stall();    // Stall Process for Load Time
            i--;        // Prevent An Instructional Skip
        }
        else
        {
            // Execute One Pipeline Cycle
            cycle();
        }

        // Make the New State the Current State
        deepCopyState(&currentState, newState);

        // Print Cycle Header
        printf("********************\n");
        printf("State at the beginning of cycle %d\n", CYCLE_COUNT);

        // Print State
        printState(currentState);
    }

    // Print Analytics
    printf("********************\n");
    printf("Total number of cycles executed: %d\n", CYCLE_COUNT);
    printf("Total number of stalls: %d\n", STALL_COUNT);
    printf("Total number of branches: %d\n", 0); // @todo
    printf("Total number of mispredicted branches: %d\n", 0); //@todo
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
    printf("\tEX/MEM\n");
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
    printf("\t\twriteReg: %s\n", translateRegister(s.stage4.writeRegister));
}

// ---------- Tool Implementations ---------- //
void init(void)
{
    // Initialize Globals
    NUM_INSTRUCTIONS = 0;
    HALTING = 0;
    CYCLE_COUNT = 0;
    STALL_COUNT = 0;
    FORWARD_A = 0b00;
    FORWARD_B = 0b00;

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
        NUM_INSTRUCTIONS++;

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
