/* @file main.c
 * @brief Main program for the MIPS assembler.
 * @author Julian Sweatt
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h> 

#define MAX_LABELS 25       // Maximum amount of labels
#define MAX_COMMANDS 100    // Maximum commands per file/input
#define DEBUG_MODE 1        // Enable debug mode with (1)
#define INST_SIZE 4         // Bytes Per Instruction (Default 4)

// Declarations 
typedef struct
{
    char** args;
    int address;
    int instruction;
} Command;

typedef struct 
{
    char* name;
    int address;
    int size;
} Label;

typedef struct
{
    Command** commandList;
    Label** labelList;
} ParseTable;

void destroyStringArray(char** arr);

Command* CommandConstructor(char** args, int address);
Command** InitCommandList();
void DestroyCommandList(Command** cmdList);
void pushCmdList(Command** cmdList, Command* newCmd);
int getNextCmdAddress(Command** cmdList);

Label* LabelConstructor(char* name, int address, int size);
Label** InitLabelList();
void DestroyLabelList(Label** labelList);
void pushLabelList(Label** labelList, Label* newLabel);
void printLabelList(Label** labelList);
int getSizeOfLabelsArray(Label** labelList);
int getNextLabAddress(Label** labList);
Label* queryLabel(Label** labelList, char* query);

ParseTable* ParseTableConstructor(Command** cmdList, Label** labList);
void DestroyParseTable(ParseTable* pt);
char** initStringsArray(int size);
void destroyStringArray(char** arr);
void pushArgsArray(char** args, char* newArg);
void printCommandsArray(Command** cmdList);
int getSizeOfCommandsArray(Command** cmdList);

int stringContains(char* str, char target);
char* removeRangeFromString(char* str, int indexLow, int indexHigh);
int registerToDecimal(char* regString);
ParseTable* parse();
char getType(char* cmd);
int getOpcode(char* cmd);
void printMachineCode(ParseTable* pt);

void evaluate(ParseTable* pt);

// ---------- Command Functions ---------- //
Command* CommandConstructor(char** args, int address)
{
    Command* newCommand = malloc(sizeof(Command));
    newCommand->args = args;
    newCommand->address = address;
    newCommand->instruction = 0;
    return newCommand;
}

Command** InitCommandList()
{
    Command** newCommandList = (Command**) calloc(MAX_COMMANDS + 1, sizeof(Command*));
    newCommandList[0] = NULL;
    return newCommandList;
}

void DestroyCommandList(Command** cmdList)
{
    int i = 0;
    while(cmdList[i]!=NULL)
    {
        destroyStringArray(cmdList[i]->args);
        free(cmdList[i]);
        i++;
    }
    free(cmdList);
}

void pushCmdList(Command** cmdList, Command* newCmd)
{
    int i = 0;
    while(cmdList[i]!=NULL)
    {
        i++;
    }

    if(i < MAX_COMMANDS)
    {
        cmdList[i] = newCmd;
        cmdList[i+1] = NULL;
    }
    else
    {
        printf("[ERROR]: Expected a maximum of %d commands. Unable to add another command `%s`.\n", MAX_COMMANDS, newCmd);
    }
}

int getNextCmdAddress(Command** cmdList)
{
    int address = 0;
    if(getSizeOfCommandsArray(cmdList) > 0)
    {
        address = cmdList[getSizeOfCommandsArray(cmdList)-1]->address + 4;
    }
    return address;
}

int getSizeOfCommandsArray(Command** cmdList)
{
    int i = 0;
    while(cmdList[i]!=NULL)
    {
        i++;
    }
    return i;
}

// ---------- Label Functions ---------- //
Label* LabelConstructor(char* name, int address, int size)
{
    Label* newLabel = malloc(sizeof(Label));
    newLabel->name = calloc(strlen(name)+1, sizeof(char));
    strcpy(newLabel->name, name);
    newLabel->address = address;
    newLabel->size = size;
    return newLabel;
}

Label** InitLabelList()
{
    Label** newLabelList = (Label**)calloc(MAX_LABELS+1, sizeof(Label*));
    newLabelList[0] = NULL;
    return newLabelList;
}

void DestroyLabelList(Label** labelList)
{
    int i = 0;
    while(labelList[i]!=NULL)
    {
        free(labelList[i]->name);
        free(labelList[i]);
        i++;
    }
    free(labelList);
}

void pushLabelList(Label** labelList, Label* newLabel)
{
    int i = 0;
    while(labelList[i]!=NULL)
    {
        i++;
    }

    if(i < MAX_LABELS)
    {
        labelList[i] = newLabel;
        labelList[i+1] = NULL;
    }
    else
    {
        printf("[ERROR]: Expected a maximum of %d labels. Unable to add another label.\n", MAX_LABELS);
    }
}

void printLabelList(Label** labelList)
{
    int i = 0;
    while(labelList[i]!=NULL && i < MAX_LABELS)
    {
        printf("%-15s%-15d\n",labelList[i]->name,labelList[i]->address);
        i++;
    }
}

int getSizeOfLabelsArray(Label** labelList)
{
    int i = 0;
    while(labelList[i]!=NULL)
    {
        i++;
    }
    return i;
}

int getNextLabAddress(Label** labList)
{
    int address = 0;
    if(getSizeOfLabelsArray(labList) > 0)
    {
        address = labList[getSizeOfLabelsArray(labList)-1]->address + labList[getSizeOfLabelsArray(labList)-1]->size;
    }
    return address;
}

Label* queryLabel(Label** labelList, char* query)
{
    int i = 0;
    while(labelList[i]!=NULL)
    {
        if(strcmp(query, labelList[i]->name)==0)
        {
            return labelList[i];
        }
        i++;
    }
    return NULL;
}

// ---------- Parse Functions ---------- //
ParseTable* ParseTableConstructor(Command** cmdList, Label** labList)
{
    //Label* newLabel = malloc(sizeof(Label));
    ParseTable* newPT = malloc(sizeof(ParseTable));
    newPT->commandList = cmdList;
    newPT->labelList = labList;
    return newPT;
}

void DestroyParseTable(ParseTable* pt)
{
    DestroyCommandList(pt->commandList);
    DestroyLabelList(pt->labelList);
    free(pt);
}

char** initStringsArray(int size)
{
    char** newArray = calloc(size+1,sizeof(char*));
    newArray[0] = NULL;
    return newArray;
}

void destroyStringArray(char** arr)
{
    int i = 0;
    while(arr[i]!=NULL)
    {
        free(arr[i]);
        i++;
    }

    free(arr);
}

void pushArgsArray(char** args, char* newArg)
{
    int i = 0;
    while(args[i]!=NULL)
    {
        i++;
    }

    if(i < 4)
    {
        args[i] = calloc(strlen(newArg)+1, sizeof(char));
        strcpy(args[i], newArg);
        args[i+1] = NULL;
    }
    else
    {
        printf("[ERROR]: Expected a maximum of %d args. Unable to add argument `%s` to `%s` command.\n", 4, newArg, args[0]);
    }
}

void printCommandsArray(Command** cmdList) // debug
{
    int i = 0;
    while(cmdList[i]!=NULL && i < MAX_COMMANDS)
    {
        int j = 0;
        while(cmdList[i]->args[j] != NULL)
        {
            if(j == 0)
            {
                printf("%s(%c - %d)  ", cmdList[i]->args[j], getType(cmdList[i]->args[j]),  getOpcode(cmdList[i]->args[j]));
            }
            else
            {
                printf("%s(%d)  ", cmdList[i]->args[j], registerToDecimal(cmdList[i]->args[j]));
            }
            
            j++;
        }
        printf(" %d\n",cmdList[i]->address);
        i++;
    }
}

int stringContains(char* str, char target)
{
    int j;
    for(j = 0; j < strlen(str); j++)
    {
        if(str[j] == target)
        {
            return j;
        }
    }
    return 0;
}

char* removeRangeFromString(char* str, int indexLow, int indexHigh)
{
    char* temp = (char*)calloc( strlen(str), sizeof(char));
    strcpy(temp, "\0");

    int i;
    int j = 0;
    for(i = 0; i < strlen(str); i ++)
    {
        if(i < indexLow || i > indexHigh)
        {
            temp[j] = str[i];
            j++;
        }
    }

    strcpy(str, temp);
    free(temp);

    return str;
}

ParseTable* parse()
{
    Label** labelList = InitLabelList();
    Command** commandList = InitCommandList();

    char lineBuffer[256];
    char directive;
    unsigned dataAllocations = 0;

    // Iterate Standard Input
    while(fgets(lineBuffer,100, stdin))
    {
        char temp[256];
        char tempDataDirective[256];
        char tempSize[256];

        // Directives (.text, .data)
        if(sscanf(lineBuffer, "\t.%s", temp))
        {
            if(strcmp(temp,"text")==0)
            {
                directive='t';
            }
            else if(strcmp(temp,"data")==0)
            {
                directive='d';
            }
            else if(strcmp(temp,"text")==0)
            {
                // Text
            }
            else if(strcmp(temp,"word")==0)
            {
                // Word
            }
            else
            {
                printf("[ERROR] An unknown directive `%s` was encountered.\n", temp);
            }
            

            if(DEBUG_MODE)
            {
                printf("Directive Swapped:%s\n",temp); // debug
            }
        }
        else
        {
            // Text Directive Parsing
            if(directive == 't')
            {
                // Label Handling
                if(stringContains(lineBuffer,':'))
                {
                    // Save Label & Remove From Command
                    int colonIndex = stringContains(lineBuffer,':');
                    strncpy(temp, lineBuffer, colonIndex);
                    temp[colonIndex] = '\0';
                    removeRangeFromString(lineBuffer,0,colonIndex);
                    pushLabelList(labelList, LabelConstructor(temp, getNextCmdAddress(commandList),INST_SIZE)); //todo: add address
                }

                // Command Handling
                char cmd[20];
                char params[50];
                if(sscanf(lineBuffer,"\t%s\t%s", cmd, params) == 2)
                {
                    if(strcmp(cmd, "la\0")==0)
                    {
                        // Load Address Expand to LUI/ORI Instructions
                        char argsBuffer[5];
                        char** laArgs = initStringsArray(4);

                        int i;
                        int prevI = 0;
                        for(i = 0; i < strlen(params)+1 ; i++)
                        {
                            if(params[i]==',' || params[i]=='(' || params[i]==')' || params[i]=='\0' || params[i]=='\n')
                            {
                                int j = 0;
                                while(prevI != i)
                                {
                                    argsBuffer[j] = params[prevI];
                                    prevI++;
                                    j++;
                                }
                                argsBuffer[j] = '\0';
                                prevI++;
                                pushArgsArray(laArgs, argsBuffer);
                            }
                        }

                        char** luiArgs = initStringsArray(4);
                        char** oriArgs = initStringsArray(4);

                        pushArgsArray(luiArgs, "lui\0");
                        pushArgsArray(luiArgs, "$1\0");
                        pushArgsArray(luiArgs, laArgs[1]); // @todo: Fix label retrieval 

                        pushArgsArray(oriArgs, "ori\0");
                        pushArgsArray(oriArgs, laArgs[0]);
                        pushArgsArray(oriArgs, "$1\0");
                        pushArgsArray(oriArgs, laArgs[1]); // @todo: Fix ^

                        pushCmdList(commandList, CommandConstructor(luiArgs,getNextCmdAddress(commandList)));
                        pushCmdList(commandList, CommandConstructor(oriArgs,getNextCmdAddress(commandList)));
                        destroyStringArray(laArgs);
                    }
                    else
                    {
                        char argsBuffer[5];
                        char** args = initStringsArray(4);
                        pushArgsArray(args, cmd);

                        int i;
                        int prevI = 0;
                        for(i = 0; i < strlen(params)+1 ; i++)
                        {
                            if(params[i]==',' || params[i]=='(' || params[i]==')' || params[i]=='\0' || params[i]=='\n')
                            {
                                int j = 0;
                                while(prevI != i)
                                {
                                    argsBuffer[j] = params[prevI];
                                    prevI++;
                                    j++;
                                }
                                argsBuffer[j] = '\0';
                                prevI++;
                                if((strlen(argsBuffer)>0))
                                {
                                    pushArgsArray(args, argsBuffer);
                                }
                            }
                        }
                        pushCmdList(commandList, CommandConstructor(args,getNextCmdAddress(commandList)));
                    }
                }
                else
                {
                    printf("[ERROR]: Invalid command `%s` encountered.", cmd);
                }
            }
            else if(directive == 'd')
            {
                // Data Directive Parsing

                if(stringContains(lineBuffer,':'))
                {
                    // Prepare for Memory Allocation
                    unsigned bytesDeclared = INST_SIZE;

                    // Save Label & Remove From Command
                    int colonIndex = stringContains(lineBuffer,':');
                    strncpy(temp, lineBuffer, colonIndex);
                    temp[colonIndex] = '\0';
                    removeRangeFromString(lineBuffer,0,colonIndex);

                    // Evaluate Data Sub-Directive
                    if(sscanf(lineBuffer, "\t.%s %s", tempDataDirective, tempSize))
                    {
                        if(strcmp(tempDataDirective,"space")==0)
                        {
                            bytesDeclared = atoi(tempSize) * INST_SIZE;
                        }
                        else if(strcmp(tempDataDirective,"word")==0)
                        {
                            bytesDeclared = atoi(tempSize);
                        }
                        else
                        {
                            printf("[ERROR]: Invalid Sub-Directive `%s` in `.data` Directive.\n", tempDataDirective);
                        }
                    }

                    if(dataAllocations == 0)
                    {
                        pushLabelList(labelList, LabelConstructor(temp, getNextCmdAddress(commandList), bytesDeclared));
                        dataAllocations++;
                    }
                    else
                    {
                        pushLabelList(labelList, LabelConstructor(temp, getNextLabAddress(labelList), bytesDeclared)); //todo: add address
                    }
                }
            }
        }
    }

    return ParseTableConstructor(commandList, labelList);
}

// ---------- Translation Utilities ---------- //
int registerToDecimal(char* regString)
{
    // $t0-$t7 -> 8-15
    // $s0-$s7 -> 16-23
    // $0 -> 0
    
    // Temp. Registers ($t)
    if(strcmp(regString, "$t0\0") == 0)
    {
        return 8;
    }
    else if(strcmp(regString, "$t1\0")==0)
    {
        return 9;
    }
    else if(strcmp(regString, "$t2\0")==0)
    {
        return 10;
    }
    else if(strcmp(regString, "$t3\0")==0)
    {
        return 11;
    }
    else if(strcmp(regString, "$t4\0")==0)
    {
        return 12;
    }
    else if(strcmp(regString, "$t5\0")==0)
    {
        return 13;
    }
    else if(strcmp(regString, "$t6\0")==0)
    {
        return 14;
    }
    else if(strcmp(regString, "$t7\0")==0)
    {
        return 15;
    }
    // Save Registers ($s)
    else if(strcmp(regString, "$s0\0")==0)
    {
        return 16;
    }
    else if(strcmp(regString, "$s1\0")==0)
    {
        return 17;
    }
    else if(strcmp(regString, "$s2\0")==0)
    {
        return 18;
    }
    else if(strcmp(regString, "$s3\0")==0)
    {
        return 19;
    }
    else if(strcmp(regString, "$s4\0")==0)
    {
        return 20;
    }
    else if(strcmp(regString, "$s5\0")==0)
    {
        return 21;
    }
    else if(strcmp(regString, "$s6\0")==0)
    {
        return 22;
    }
    else if(strcmp(regString, "$s7\0")==0)
    {
        return 23;
    }
    // Null Register ($0)
    else if(strcmp(regString, "$0\0")==0)
    {
        return 0;
    }
    // Pseudo-Instruction Register
    else if(strcmp(regString, "$1\0")==0) // @todo May need to resolve all numeric registers
    {
        return 1;
    }
    // Others (Immediate)
    else
    {
        return atoi(regString);
    }
}

char getType(char* cmd)
{
    if( (strcmp(cmd, "add\0")==0) || (strcmp(cmd, "nor\0")==0) || (strcmp(cmd, "sll\0")==0) )
        return 'r';
    else if( (strcmp(cmd, "addi\0")==0) || (strcmp(cmd, "ori\0")==0) || (strcmp(cmd, "lui\0")==0) || (strcmp(cmd, "sw\0")==0) || (strcmp(cmd, "lw\0")==0) || (strcmp(cmd, "bnw\0")==0) || (strcmp(cmd, "beq\0")==0) )
        return 'i';
    else if( (strcmp(cmd, "j\0")==0) )
        return 'j';
    else
    {
        printf("[ERROR]: `%s` is not a valid command.\n", cmd);
        return '\0';
    }
}

int getOpcode(char* cmd)
{
    if(strcmp(cmd, "add\0")==0)
        return 32;
    else if(strcmp(cmd, "addi\0")==0)
        return 8;
    else if(strcmp(cmd, "nor\0")==0)
        return 39;
    else if(strcmp(cmd, "ori\0")==0)
        return 13;
    else if(strcmp(cmd, "sll\0")==0)
        return 0;
    else if(strcmp(cmd, "lui\0")==0)
        return 15;
    else if(strcmp(cmd, "sw\0")==0)
        return 43;
    else if(strcmp(cmd, "lw\0")==0)
        return 35;
    else if(strcmp(cmd, "beq\0")==0)
        return 4;
    else if(strcmp(cmd, "bne\0")==0)
        return 5;
    else if(strcmp(cmd, "j\0")==0)
        return 2;
    else
    {
        printf("[ERROR] Unable to get opcode for `%s`. Invalid command.\n", cmd);
        return 0;
    }
    
}

//////// DEBUG CODE PROVIDED BY https://www.geeksforgeeks.org/binary-representation-of-a-given-number/
void bin(unsigned n) 
{ 
    unsigned i; 
    for (i = 1 << 31; i > 0; i = i / 2) 
        (n & i)? printf("1"): printf("0"); 
} 

// ---------- Machine Code ---------- //
void printMachineCode(ParseTable* pt)
{
    int i = 0;
    while(pt->commandList[i]!=NULL && i < MAX_COMMANDS)
    {
        if(DEBUG_MODE)
        {
            // @NOTICE Convert to binary not built in to it. May need to do some stuff to see it.
            //@todo Next: Get the opcode and shift it to the left-most bits
            printf("0x%08X: 0x%08X ", pt->commandList[i]->address, pt->commandList[i]->instruction);
            bin(pt->commandList[i]->instruction);
            printf("\n");
        }
        else
        {
            printf("0x%08X: 0x%08X\n", pt->commandList[i]->address, pt->commandList[i]->instruction);
        }
        i++;
    }
}

/**
 * @brief The main function for the assembler. 
 */
int main()
{
    // Pass 1 (Parse Table)
    ParseTable* pt = parse();

    // Debugging
    if(DEBUG_MODE)
    {
        printLabelList(pt->labelList); //debug
        printCommandsArray(pt->commandList); // debug
    }

    evaluate(pt);

    printMachineCode(pt);

    // Destructor
    DestroyParseTable(pt);
    return(0);
} 

// ---------- Evaluate and Resolve Instructions ---------- //
void evaluate(ParseTable* pt)
{
    int i = 0;
    while(pt->commandList[i]!=NULL)
    {
        // Assign Registers from Parsed Data
        unsigned rs = 0;
        unsigned rt = 0;
        unsigned rd = 0;
        unsigned shamt = 0;
        unsigned func = 0;
        unsigned imm = 0;
        unsigned address = 0;
        unsigned opCode = 0;

        opCode = getOpcode(pt->commandList[i]->args[0]);

        switch(getType(pt->commandList[i]->args[0]))
        {
            case 'r':
            {
                func = opCode;
                if(opCode == 32 || opCode == 39) // ADD/NOR
                {
                    rd = registerToDecimal(pt->commandList[i]->args[1]);
                    rs = registerToDecimal(pt->commandList[i]->args[2]);
                    rt = registerToDecimal(pt->commandList[i]->args[3]);
                }
                else if(opCode == 0) // SLL
                {
                    rd = registerToDecimal(pt->commandList[i]->args[1]);
                    rt = registerToDecimal(pt->commandList[i]->args[2]);
                    shamt = registerToDecimal(pt->commandList[i]->args[3]);
                }
            }
            case 'i':
            {
                if(opCode == 8 || opCode == 13) // ADDI/ORI
                {
                    rt = registerToDecimal(pt->commandList[i]->args[1]);
                    rs = registerToDecimal(pt->commandList[i]->args[2]);
                    imm = registerToDecimal(pt->commandList[i]->args[3]);
                }
                else if(opCode == 15) // LUI
                {
                    rt = registerToDecimal(pt->commandList[i]->args[1]);
                    imm = registerToDecimal(pt->commandList[i]->args[2]);
                }
                else if(opCode == 43 || opCode == 35) // SW/LW
                {
                    rt = registerToDecimal(pt->commandList[i]->args[1]);
                    imm = registerToDecimal(pt->commandList[i]->args[2]);
                    rs = registerToDecimal(pt->commandList[i]->args[3]);
                }
                else if(opCode == 5) // BNE
                {
                    rs = registerToDecimal(pt->commandList[i]->args[1]);
                    rt = registerToDecimal(pt->commandList[i]->args[2]);
                    // = registerToDecimal(pt->commandList[i]->args[3]); //@todo Label Handeling
                }
            }
            case 'j':
            {
                
            }
        }


        // Further Resolution
        switch(getType(pt->commandList[i]->args[0]))
        {
            case 'r':
            {
                // R-Type Instructions
                // Begin Instruction With 0
                pt->commandList[i]->instruction = 0;

                // RS
                pt->commandList[i]->instruction = pt->commandList[i]->instruction << 5;
                pt->commandList[i]->instruction = pt->commandList[i]->instruction | rs;

                // RT
                pt->commandList[i]->instruction = pt->commandList[i]->instruction << 5;
                pt->commandList[i]->instruction = pt->commandList[i]->instruction | rt;

                // RD
                pt->commandList[i]->instruction = pt->commandList[i]->instruction << 5;
                pt->commandList[i]->instruction = pt->commandList[i]->instruction | rd;

                // Shamt
                pt->commandList[i]->instruction = pt->commandList[i]->instruction << 5;
                pt->commandList[i]->instruction = pt->commandList[i]->instruction | shamt;

                // Function
                pt->commandList[i]->instruction = pt->commandList[i]->instruction << 6;
                pt->commandList[i]->instruction = pt->commandList[i]->instruction | func;
                break;
            }
            case 'i':
            {
                // I-Type Instructions
                // Begin Instruction With Opcode
                pt->commandList[i]->instruction = opCode;

                // RS
                pt->commandList[i]->instruction = pt->commandList[i]->instruction << 5;
                pt->commandList[i]->instruction = pt->commandList[i]->instruction | rs;

                // RT
                pt->commandList[i]->instruction = pt->commandList[i]->instruction << 5;
                pt->commandList[i]->instruction = pt->commandList[i]->instruction | rt;

                // Immediate
                pt->commandList[i]->instruction = pt->commandList[i]->instruction << 16;
                pt->commandList[i]->instruction = pt->commandList[i]->instruction | imm;
                break;
            }
            case 'j':
            {
                // J-Type Instructions
                // Begin Instruction With Opcode
                pt->commandList[i]->instruction = opCode;

                pt->commandList[i]->instruction = pt->commandList[i]->instruction << 26;
                pt->commandList[i]->instruction = pt->commandList[i]->instruction | address;
                break;
            }
            default:
            {
                // Unknown Types
            }
        }

        i++;
    }
}