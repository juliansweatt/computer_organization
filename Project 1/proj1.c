/* @file main.c
 * @brief Main program for the MIPS assembler.
 * @author Julian Sweatt
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h> 

#define MAX_LABELS 25
#define MAX_COMMANDS 100
#define DEBUG_MODE 1

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
int getNextAddress(Command** cmdList);
Label* LabelConstructor(char* name, int address);
Label** InitLabelList();
void DestroyLabelList(Label** labelList);
void pushLabelList(Label** labelList, Label* newLabel);
void printLabelList(Label** labelList);
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

int getNextAddress(Command** cmdList)
{
    int address = 0;
    if(getSizeOfCommandsArray(cmdList) > 0)
    {
        address = cmdList[getSizeOfCommandsArray(cmdList)-1]->address + 4;
    }
    return address;
}

// ---------- Label Functions ---------- //
Label* LabelConstructor(char* name, int address)
{
    Label* newLabel = malloc(sizeof(Label));
    newLabel->name = calloc(strlen(name)+1, sizeof(char));
    strcpy(newLabel->name, name);
    newLabel->address = address;
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

int getSizeOfCommandsArray(Command** cmdList)
{
    int i = 0;
    while(cmdList[i]!=NULL)
    {
        i++;
    }
    return i;
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

    // Iterate Standard Input
    while(fgets(lineBuffer,100, stdin))
    {
        char temp[256];
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
                    pushLabelList(labelList, LabelConstructor(temp, getNextAddress(commandList))); //todo: add address
                }

                // Command Handling
                char cmd[20];
                char params[50];
                if(sscanf(lineBuffer,"\t%s\t%s", cmd, params) == 2)
                {
                    char argsBuffer[5];
                    char** args = initStringsArray(4);
                    pushArgsArray(args, cmd);

                    int i;
                    int prevI = 0;
                    for(i = 0; i < strlen(params)+1 ; i++)
                    {
                        if(params[i]==',' || params[i]=='\0' || params[i]=='\n')
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
                            pushArgsArray(args, argsBuffer);
                        }
                    }
                    pushCmdList(commandList, CommandConstructor(args,getNextAddress(commandList)));
                }
                else
                {
                    printf("[ERROR]: Invalid command `%s` encountered.", cmd);
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
    else if(strcmp(cmd, "j\0")==0)
        return '\0';
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
    // Resolve Opcodes
    int i = 0;
    while(pt->commandList[i]!=NULL)
    {
        pt->commandList[i]->instruction = getOpcode(pt->commandList[i]->args[0]);
        pt->commandList[i]->instruction = pt->commandList[i]->instruction << 26;
        i++;
    }
}