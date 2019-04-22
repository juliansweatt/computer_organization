/* @file proj3_sweatt.c
 * @brief Main program for the cache simulator.
 * @author Julian L. Sweatt
 */

/*----------------------------------*
 *             IMPORTS              *
 *----------------------------------*/
#include <stdio.h>
#include <stdlib.h>

/*----------------------------------*
 *             CONFIG               *
 *----------------------------------*/

/*----------------------------------*
 *              HEADER              *
 *----------------------------------*/
// ---------- Structures ---------- //
/**
 * @struct Line
 * @brief Structure intended to hold a line of input containing an unsigned integer and a single character R or W.
 */
typedef struct
{
    char operation;              // Indicates (R)ead or (W)rite Operation
    unsigned int address;        // Address to Cache
} Line;

/**
 * @struct LineList
 * @brief List of lines.
 */
typedef struct
{
    Line * lines;
    unsigned int size;
} LineList;

// ---- Dynamic Input Functions --- //
/**
 * @brief Initialize a dynamic list of lines.
 * @return void
 */
void initLines(void);

/**
 * @brief Deinitialize a dynamic list of lines.
 * @return void
 */
void deinitLines(void);

/**
 * @brief Append a line to a list of lines dynamically.
 * @param char operation Indicates (R)ead or (W)rite Operation.
 * @param int address Address portion of line.
 * @return void
 */
void addLine(char operation, int address);

/**
 * @brief Print the list of lines to stdout.
 * @return void
 * @private This is a debug function.
 */
void printLines(void);

/*----------------------------------*
 *             Globals               *
 *----------------------------------*/
LineList * LINE_LIST;

/*----------------------------------*
 *          IMPLEMENTATIONS         *
 *----------------------------------*/
void initLines(void)
{
    LINE_LIST = (LineList*) malloc(sizeof(LineList));
    LINE_LIST->lines = (Line*) malloc(sizeof(Line));
    LINE_LIST->size = 0;
}

void deinitLines(void)
{
    free(LINE_LIST->lines);
    free(LINE_LIST);
}

void addLine(char operation, int address)
{
    LINE_LIST->size += 1;
    LINE_LIST->lines = (Line*)realloc(LINE_LIST->lines, sizeof(Line)*LINE_LIST->size);
    LINE_LIST->lines[LINE_LIST->size-1].address = address;
    LINE_LIST->lines[LINE_LIST->size-1].operation = operation;
}

void printLines()
{
    int i;
    for(i = 0; i < LINE_LIST->size; i++)
    {
        printf("%c %d\n", LINE_LIST->lines[i].operation, LINE_LIST->lines[i].address);
    }
}

/*----------------------------------*
 *                MAIN              *
 *----------------------------------*/
int main()
{
    initLines();
    addLine('A',1);
    addLine('B',2);
    addLine('C',3);
    printLines();
    deinitLines();
}
