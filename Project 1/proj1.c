/* @file main.c
 * @brief Main program for the MIPS assembler.
 * @author Julian Sweatt
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h> 

#define MAX_LABELS 25
#define MAX_COMMANDS 100

// Declarations 
typedef struct
{
    char** args;
    int address;
} Command;

typedef struct 
{
    char* name;
    int address;
} Label;

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
char** initStringsArray(int size);
void destroyStringArray(char** arr);
void pushArgsArray(char** args, char* newArg);
void printCommandsArray(Command** cmdList);
int getSizeOfCommandsArray(Command** cmdList);
int stringContains(char* str, char target);
char* removeRangeFromString(char* str, int indexLow, int indexHigh);
void parse();

// ---------- Command Functions ---------- //
Command* CommandConstructor(char** args, int address)
{
    Command* newCommand = malloc(sizeof(Command));
    newCommand->args = args;
    newCommand->address = address;
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
        printf("[ERROR]: Expected a maximum of %d commands. Unable to add another command.\n", MAX_COMMANDS);
    }
}

int getNextAddress(Command** cmdList)
{
    int address = 0;
    if(getSizeOfCommandsArray(cmdList) > 0)
    {
        address = cmdList[getSizeOfCommandsArray(cmdList)-1]->address + 1;
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
        //printf("PUSHING:%s\n", newArg); //debug
        args[i] = calloc(strlen(newArg)+1, sizeof(char));
        strcpy(args[i], newArg);
        args[i+1] = NULL;
    }
    else
    {
        printf("[ERROR]: Expected a maximum of %d args. Unable to add command.\n", 4);
    }
}

void printCommandsArray(Command** cmdList)
{
    int i = 0;
    while(cmdList[i]!=NULL && i < MAX_COMMANDS)
    {
        int j = 0;
        while(cmdList[i]->args[j] != NULL)
        {
            printf("%s ", cmdList[i]->args[j]);
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

void parse()
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
            printf("DIR:%s\n",temp); // debug
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
                    int i;
                    char argsBuffer[5];
                    char** args = initStringsArray(4);
                    pushArgsArray(args, cmd);
                    for(i = 0; i < strlen(params); i ++)
                    {
                        if(params[i]=='$')
                        {
                            argsBuffer[0] = params[i];
                            argsBuffer[1] = params[i+1];
                            argsBuffer[2] = params[i+2];
                            argsBuffer[3] = '\0';

                            pushArgsArray(args,argsBuffer);
                            i = i+2;
                        }
                        else if(params[i]==',')
                        {
                            argsBuffer[0] = params[i+1];
                            argsBuffer[1] = params[i+2];
                            argsBuffer[2] = params[i+3];
                            argsBuffer[3] = '\0';

                            pushArgsArray(args,argsBuffer);
                            i = i+2;
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

    printLabelList(labelList); //debug
    printCommandsArray(commandList); // debug

    DestroyLabelList(labelList);
    DestroyCommandList(commandList);
}

/**
 * @brief The main function for the assembler. 
 */
int main()
{
    parse();
    return(0);
} 