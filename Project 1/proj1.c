/* @file main.c
 * @brief Main program for the MIPS assembler.
 * @author Julian Sweatt
 */

/*----------------------------------*
 *             CONFIG               *
 *----------------------------------*/
#define MAX_LABELS 200      // Maximum amount of labels
#define MAX_COMMANDS 200    // Maximum commands per file/input
#define DEBUG_MODE 0        // Enable debug mode with (1)
#define INST_SIZE 4         // Bytes Per Instruction (Default 4)

/*----------------------------------*
 *             IMPORTS              *
 *----------------------------------*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h> 

/*----------------------------------*
 *              HEADER              *
 *----------------------------------*/
// ---------- Structures ---------- //
/**
 * @struct Command
 * @brief Structure used to indicate a command. Note, command used interchangably
 *        with the term 'instruction'.
 */
typedef struct
{
    char** args;        // Tokenized command arguments where args[0] is the command name
    int address;        // Address of the instruction
    int instruction;    // 32 bit representation of the instruction
} Command;

/**
 * @struct Label
 * @brief Structure used to indicate a label.
 */
typedef struct 
{
    char* name;         // Name of the label
    int address;        // Address of the label
    int size;           // Size of the instruction or data marked by the label
} Label;

/**
 * @struct ParseTable
 * @brief Pair of Command List and Label List.
 */
typedef struct
{
    Command** commandList;  // List of commands
    Label** labelList;      // List of labels
} ParseTable;

// ---------- Command Functions ---------- //
/**
 * @brief Builds a command structure.
 * @param char** args Tokenized instruction
 * @param int address Address of the instruction
 * @return Command* Newly created command
 */
Command* CommandConstructor(char** args, int address);

/**
 * @brief Initializes an array of commands.
 * @return Command** Newly created array of commands.
 */
Command** InitCommandList();

/**
 * @brief Free all memory associated with a command list.
 * @param Command** cmdList Command list to free.
 * @return void
 */
void DestroyCommandList(Command** cmdList);

/**
 * @brief Push a new command on to a command-list at the end
 * @param Command** cmdList Command list to append to
 * @param Command* newCmd New command to append to the command list
 * @return void
 */
void pushCmdList(Command** cmdList, Command* newCmd);

/**
 * @brief Get the appropriate address of the next instruction
 * @param Command** The existing command list
 * @return int Address of next instruction
 */
int getNextCmdAddress(Command** cmdList);

/**
 * @private This is a debug function. 
 * @brief Print a list of commands.
 * @param Command** cmdList List of commands to print to stdout
 * @return void
 */
void printCommandsArray(Command** cmdList);

/**
 * @brief Get the size of a list of commands, excluding the NULL terminator
 * @param @Command** cmdList List of commands to get the size of
 * @return int The size of the list of commands (excluding terminator)
 */
int getSizeOfCommandsArray(Command** cmdList);

// ------------ Label Functions ------------ //
/**
 * @brief Builds a label structure
 * @param char* name Name of the label
 * @param int address Address of the label
 * @param size Size (in bytes) of the label's target
 * return Label* Newly created label
 */
Label* LabelConstructor(char* name, int address, int size);

/**
 * @brief Initializes a list of labels in memory
 * @return Label** Newly created list of labels
 */
Label** InitLabelList();

/**
 * @brief Free all memory associated with a list of labels
 * @param Label** labelList List of labels to free
 * @return void
 */
void DestroyLabelList(Label** labelList);

/**
 * @brief Push a label onto the end of a list of labels
 * @param Label** labelList List of labels to append to
 * @param Label* newLabel New label to append to the list
 * @return void
 */
void pushLabelList(Label** labelList, Label* newLabel);

/**
 * @private This is a debug function.
 * @brief Prints all labels in a label list to stdout
 * @param Label** labelList List of labels to print
 * @return void
 */
void printLabelList(Label** labelList);

/**
 * @brief Get the size of a labels array (excluding NULL terminator)
 * @param Label** labelList List of labels to get the size of
 * @return int Size of the label's list (excluding NULL terminator)
 */
int getSizeOfLabelsArray(Label** labelList);

/**
 * @brief Get the next label address, taking memory allocation sizes into consideration
 * @param Label** labList List of labels to get the next appropriate address for
 * @return int Next appropriate label address
 */
int getNextLabAddress(Label** labList);

/**
 * @brief Search for the existance of a label by instruction/command name
 * @param Label** labelList List of labels to query
 * @param @char* query Name of the instruction/command to search for
 * @return Label* Returns the label if found, returns NULL otherwise
 */
Label* queryLabel(Label** labelList, char* query);

// ------- Parsing & Utility Functions ------- //
/**
 * @brief Construct a parse table (pair of command list and label list)
 * @param Command** cmdList List of commands
 * @param Label** labList List of labels
 * @return ParseTable* Newly created ParseTable
 */
ParseTable* ParseTableConstructor(Command** cmdList, Label** labList);

/**
 * @brief Free all memory associated with an entire parse table
 * @param ParseTable* pt ParseTable to free
 * @return void
 */
void DestroyParseTable(ParseTable* pt);

/**
 * @brief Initialize an array of strings
 * @param int size Size of the array of strings
 * @return char** Newly created array of strings
 */
char** initStringsArray(int size);

/**
 * @brief Free all memory associated with an array of c-strings. 
 * @param char** arr Array of c-strings to free.
 * @return void
 */
void destroyStringArray(char** arr);

/**
 * @brief Push a string onto the end of an array of strings
 * @param char** args Existing array of strings to append to
 * @param char* newArg String to append to the array of strings
 * @return void
 */
void pushArgsArray(char** args, char* newArg);

/**
 * @brief Search a string for a character
 * @param char* str String to search
 * @param char target Character to search for
 * @return Returns the index of the character if found or 0 if not found
 */
int stringContains(char* str, char target);

/**
 * @brief Remove a range of indexes from a dynamic string
 * @param char* str String to remove from
 * @param int indexLow Lower index to remove (Inclusive)
 * @param int indexHigh Higher index to remove (Inclusive)
 * @return char* New string with range removed
 */
char* removeRangeFromString(char* str, int indexLow, int indexHigh);

/**
 * @brief Resolve an argument/register in an instruction to 32 bit representation
 * @param Label** labList List of labels to use of label resolution
 * @param char* regString Register/Argument string (null terminated)
 * @return int 32 bit integer representation of the argument
 */
int resolveRegister(Label** labList,char* regString);

/**
 * @brief Resolve a known register value to the appropriate 32 bit integer representation
 * @param char* regString Register/Argument string to resolve (null terminated)
 * @return int 32 bit integer representation of the argument
 */
int registerToDecimal(char* regString);

/**
 * @brief Generate a ParseTable from standard input. Expects a MIPS program file.
 * @return ParseTable* Parsed MIPS program
 */
ParseTable* parse();

/**
 * @brief Get the instruction type of an instruction/command
 * @param char* cmd Command name to get the type of
 * @return char `i`, `r`, or `j` for the appropriate instruction type
 */
char getType(char* cmd);

/**
 * @brief Get the operation code for an instruction/command
 * @param char* cmd Command name to get the opcode of
 * @return int 32 bit integer representation of the argument
 */
int getOpcode(char* cmd);

/**
 * @brief Print the machine code representation of the assembled ParseTable
 * @param ParseTable* pt ParseTable to print
 * @return void
 * 
 * Note, this is the end-goal of the project. The parsetable should be formed and
 * evaluated before calling this command.
 */
void printMachineCode(ParseTable* pt);


/**
 * @brief Resolve memory addresses and allocations throughout the parse table.
 * @param ParseTable* pt ParseTable to evaluate
 * @return void
 * 
 * This function is responsible for evaluating memory addresses, jumps, branches, 
 * and memory allocations throughout the parse table. This is the `second pass` discussed
 * in the assignment details. 
 */
void evaluate(ParseTable* pt);

/*----------------------------------*
 *          IMPLEMENTATIONS         *
 *----------------------------------*/
// ------------- MAIN ------------- //
/**
 * @brief The main function for the assembler. 
 * @return int 0 on success 1 on fail/fatal error
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

// ---------- Parse/General/Utility Functions ---------- //
ParseTable* ParseTableConstructor(Command** cmdList, Label** labList)
{
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

                        pushArgsArray(luiArgs, "_lui\0");
                        pushArgsArray(luiArgs, "$1\0");
                        pushArgsArray(luiArgs, laArgs[1]); // @todo: Fix label retrieval 

                        pushArgsArray(oriArgs, "_ori\0");
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
                            bytesDeclared = atoi(tempSize);
                        }
                        else if(strcmp(tempDataDirective,"word")==0)
                        {
                            bytesDeclared = atoi(tempSize) * INST_SIZE;
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
int resolveRegister(Label** labList,char* regString)
{
    if(queryLabel(labList, regString))
    {
        if(DEBUG_MODE)
        {
            printf("Resolving label `%s` to address `%d`.\n", regString, queryLabel(labList, regString)->address);
        }
        return queryLabel(labList, regString)->address;
    }
    return registerToDecimal(regString);
}

int registerToDecimal(char* regString)
{
    // Temp. Registers ($t)
    if(strcmp(regString, "$t0\0") == 0)
        return 8;
    else if(strcmp(regString, "$t1\0")==0)
        return 9;
    else if(strcmp(regString, "$t2\0")==0)
        return 10;
    else if(strcmp(regString, "$t3\0")==0)
        return 11;
    else if(strcmp(regString, "$t4\0")==0)
        return 12;
    else if(strcmp(regString, "$t5\0")==0)
        return 13;
    else if(strcmp(regString, "$t6\0")==0)
        return 14;
    else if(strcmp(regString, "$t7\0")==0)
        return 15;
    // Save Registers ($s)
    else if(strcmp(regString, "$s0\0")==0)
        return 16;
    else if(strcmp(regString, "$s1\0")==0)
        return 17;
    else if(strcmp(regString, "$s2\0")==0)
        return 18;
    else if(strcmp(regString, "$s3\0")==0)
        return 19;
    else if(strcmp(regString, "$s4\0")==0)
        return 20;
    else if(strcmp(regString, "$s5\0")==0)
        return 21;
    else if(strcmp(regString, "$s6\0")==0)
        return 22;
    else if(strcmp(regString, "$s7\0")==0)
        return 23;
    // Null Register ($0)
    else if(strcmp(regString, "$0\0")==0)
        return 0;
    // Pseudo-Instruction Register
    else if(strcmp(regString, "$1\0")==0) // @todo May need to resolve all numeric registers
        return 1;
    // Others (Immediate)
    else
        return atoi(regString);
}

char getType(char* cmd)
{
    if( 
        (strcmp(cmd, "add\0")==0) || 
        (strcmp(cmd, "nor\0")==0) ||
        (strcmp(cmd, "sll\0")==0) ||
        (strcmp(cmd, "sub\0")==0) 
        )
        return 'r';
    else if(
        (strcmp(cmd, "addi\0")==0) ||
        (strcmp(cmd, "ori\0")==0) ||
        (strcmp(cmd, "lui\0")==0) ||
        (strcmp(cmd, "_ori\0")==0) ||
        (strcmp(cmd, "_lui\0")==0) ||
        (strcmp(cmd, "sw\0")==0) ||
        (strcmp(cmd, "lw\0")==0) ||
        (strcmp(cmd, "bne\0")==0) ||
        (strcmp(cmd, "beq\0")==0)
        )
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
    if(strcmp(cmd, "sll\0")==0)
        return 0;
    else if(strcmp(cmd, "j\0")==0)
        return 2;
    else if(strcmp(cmd, "beq\0")==0)
        return 4;
    else if(strcmp(cmd, "bne\0")==0)
        return 5;
    else if(strcmp(cmd, "addi\0")==0)
        return 8;
    else if(strcmp(cmd, "ori\0")==0)
        return 13;
    else if(strcmp(cmd, "lui\0")==0)
        return 15;
    else if(strcmp(cmd, "add\0")==0)
        return 32;
    else if(strcmp(cmd, "sub\0")==0)
        return 34;
    else if(strcmp(cmd, "lw\0")==0)
        return 35;
    else if(strcmp(cmd, "nor\0")==0)
        return 39;
     else if(strcmp(cmd, "sw\0")==0)
        return 43;
    else if(strcmp(cmd, "_lui\0")==0)
        return -15;
    else if(strcmp(cmd, "_ori\0")==0)
        return -13;
    else
    {
        printf("[ERROR] Unable to get opcode for `%s`. Invalid command.\n", cmd);
        return 0;
    }
    
}

// ---------- Machine Code ---------- //
void printMachineCode(ParseTable* pt)
{
    int i = 0;
    while(pt->commandList[i]!=NULL && i < MAX_COMMANDS)
    {
        if(DEBUG_MODE)
        {
            printf("0x%08X: 0x%08X ", pt->commandList[i]->address, pt->commandList[i]->instruction);

            // Print Binary Representation (Debug Only)
            // Reference: https://www.geeksforgeeks.org/binary-representation-of-a-given-number/
            unsigned j; 
            for (j = 1 << 31; j > 0; j = j / 2) 
            (pt->commandList[i]->instruction & j)? printf("1"): printf("0"); 

            printf("\n");
        }
        else
        {
            printf("0x%08X: 0x%08X\n", pt->commandList[i]->address, pt->commandList[i]->instruction);
        }
        i++;
    }
}

// ---------- Evaluate and Resolve Instructions ---------- //
void evaluate(ParseTable* pt)
{
    int i = 0;
    while(pt->commandList[i]!=NULL)
    {
        // Assign Registers from Parsed Data
        int8_t rs = 0;
        int8_t rt = 0;
        int8_t rd = 0;
        int8_t shamt = 0;
        int8_t func = 0;
        int32_t imm = 0;
        int32_t address = 0;
        int8_t opCode = 0;

        opCode = getOpcode(pt->commandList[i]->args[0]);

        switch(getType(pt->commandList[i]->args[0]))
        {
            case 'r':
            {
                func = opCode;
                if(opCode == 32 || opCode == 39 || opCode == 34) // ADD/NOR/SUB
                {
                    rd = resolveRegister(pt->labelList, pt->commandList[i]->args[1]);
                    rs = resolveRegister(pt->labelList, pt->commandList[i]->args[2]);
                    rt = resolveRegister(pt->labelList, pt->commandList[i]->args[3]);
                }
                else if(opCode == 0) // SLL
                {
                    rd = resolveRegister(pt->labelList, pt->commandList[i]->args[1]);
                    rt = resolveRegister(pt->labelList, pt->commandList[i]->args[2]);
                    shamt = resolveRegister(pt->labelList, pt->commandList[i]->args[3]);
                }
            }
            case 'i':
            {
                if(opCode == 8 || opCode == 13) // ADDI/ORI
                {
                    rt = resolveRegister(pt->labelList, pt->commandList[i]->args[1]);
                    rs = resolveRegister(pt->labelList, pt->commandList[i]->args[2]);
                    imm = resolveRegister(pt->labelList, pt->commandList[i]->args[3]);
                }
                else if(opCode == 15) // LUI
                {
                    rt = resolveRegister(pt->labelList, pt->commandList[i]->args[1]);
                    imm = resolveRegister(pt->labelList, pt->commandList[i]->args[2]);
                }
                else if(opCode == 43 || opCode == 35) // SW/LW
                {
                    rt = resolveRegister(pt->labelList, pt->commandList[i]->args[1]);
                    imm = resolveRegister(pt->labelList, pt->commandList[i]->args[2]);
                    rs = resolveRegister(pt->labelList, pt->commandList[i]->args[3]);
                }
                else if(opCode == 5) // BNE
                {
                    rs = resolveRegister(pt->labelList, pt->commandList[i]->args[1]);
                    rt = resolveRegister(pt->labelList, pt->commandList[i]->args[2]);
                    imm = resolveRegister(pt->labelList, pt->commandList[i]->args[3]);
                }
                else if(opCode == 4) // BEQ
                {
                    rs = resolveRegister(pt->labelList, pt->commandList[i]->args[1]);
                    rt = resolveRegister(pt->labelList, pt->commandList[i]->args[2]);
                    imm = resolveRegister(pt->labelList, pt->commandList[i]->args[3]);
                    imm = (pt->commandList[i]->address - imm)/INST_SIZE;
                    imm = ~imm;
                }
                else if(opCode == -15) // Handling for LUI generated from LA
                {
                    opCode = 15;
                    rt = resolveRegister(pt->labelList, pt->commandList[i]->args[1]);
                    imm = resolveRegister(pt->labelList, pt->commandList[i]->args[2]);
                    // Trim Lower-Half of Bits
                    imm = imm & 0xFFFF0000;
                    imm = imm >> 16;
                }
                else if(opCode == -13) // Handling for ORI generated from LA
                {
                    opCode = 13;
                    rt = resolveRegister(pt->labelList, pt->commandList[i]->args[1]);
                    rs = resolveRegister(pt->labelList, pt->commandList[i]->args[2]);
                    imm = resolveRegister(pt->labelList, pt->commandList[i]->args[3]);
                }
                
                // Enforce Bounds
                imm = imm & 0x0000FFFF;
            }
            case 'j':
            {
                address = resolveRegister(pt->labelList, pt->commandList[i]->args[1])/4; //todo Investigate further
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