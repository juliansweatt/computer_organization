/* @file proj3_sweatt.c
 * @brief Main program for the cache simulator.
 * @author Julian L. Sweatt
 */

/*----------------------------------*
 *             IMPORTS              *
 *----------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

/*----------------------------------*
 *             CONFIG               *
 *----------------------------------*/
#define DEBUG_MODE 1
#define MAX_LINES 100

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

/**
 * @struct Set
 * @brief Set within a cache;
 */
typedef struct
{
    int * data;
} Set;

/**
 * @struct Cache
 * @brief General purpose cache.
 */
typedef struct
{
    Set* sets;
    unsigned int hits;
    unsigned int misses;
    unsigned int memrefs;
} Cache;

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

/**
 * @brief Print block size, number of sets, associativity, and list of lines from input.
 * @return void
 * @private This is a debug function.
 */
void printInput(void);

// -------- Cache Functions ------- //
/**
 * @brief Initialize a set associative cache.
 * @return void
 */
void initCache(void);

/**
 * @brief Deinitialize the set associative cache.
 * @return void
 */
void deinitCache(void);

/**
 * @brief Reset the set associative cache's contents.
 * @return void
 */
void resetCache(void);

/**
 * @brief Calculate bitwise breakdown of the address.
 * @return void
 */
void calculateAddressBits(void);

/**
 * @brief Print the shared and basic information of the set associative cache.
 * @return void
 */
void printHeader(void);

/**
 * @brief Print the cache report after processing.
 * @param char Method of caching to report Write (B)ack or Write (T)hrough.
 * @return void
 */
void printCacheReport(char cachingMethod);

/**
 * @brief Print the set associative cache's contents.
 * @return void
 * @private This is a debug function.
 */
void printCache(void);

// ----------- Utilities ---------- //
/**
 * @brief Parse input from stdin. Expects 3 integers, each on seperate lines, followed by
 *        an indefinite list of W/R address lines (char and int).
 * @return void
 * @private This is a debug function.
 */
void parseInput(void);

/*----------------------------------*
 *             Globals               *
 *----------------------------------*/
LineList * LINE_LIST;
Cache* CACHE;
unsigned int BLOCK_SIZE;
unsigned int NUM_SETS;
unsigned int SET_ASSOCIATIVITY;
unsigned int OFFSET_BITS;
unsigned int INDEX_BITS;
unsigned int TAG_BITS;

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

void printLines(void)
{
    int i;
    for(i = 0; i < LINE_LIST->size; i++)
    {
        printf("%c %d\n", LINE_LIST->lines[i].operation, LINE_LIST->lines[i].address);
    }
}

void parseInput(void)
{
    // Get Base Variables
    scanf("%d", &BLOCK_SIZE);
    scanf("%d", &NUM_SETS);
    scanf("%d\n", &SET_ASSOCIATIVITY);

    // Indefinite Line Input
    char lineBuffer[256];
    char op;
    int add;
    while(fgets(lineBuffer, 256, stdin))
    {
        sscanf(lineBuffer,"%c %d", &op, &add);
        addLine(op,add);
    }
}

void initCache(void)
{
    CACHE = (Cache*)malloc(sizeof(Cache));
    CACHE->sets = (Set*)calloc(NUM_SETS,sizeof(Set));
    int i;
    for(i = 0; i < NUM_SETS; i++)
    {
        CACHE->sets[i].data = (int*)calloc(SET_ASSOCIATIVITY,sizeof(int));
    }

    CACHE->hits = 0;
    CACHE->misses = 0;
    CACHE->memrefs = 0;
}

void deinitCache(void)
{
    int i;
    for(i = 0; i < NUM_SETS; i++)
    {
        free(CACHE->sets[i].data);
    }
    free(CACHE->sets);
    free(CACHE);
}

void resetCache(void)
{
    int i;
    for(i = 0; i < NUM_SETS; i++)
    {
        int j;
        for(j = 0; j < SET_ASSOCIATIVITY; j++)
        {
            CACHE->sets[i].data[j] = 0;
        }
    }

    CACHE->hits = 0;
    CACHE->misses = 0;
    CACHE->memrefs = 0;
}

void calculateAddressBits(void)
{
    OFFSET_BITS = log2(BLOCK_SIZE);
    INDEX_BITS = log2(NUM_SETS);
    TAG_BITS = 32 - OFFSET_BITS - INDEX_BITS;
}

void printHeader(void)
{
    printf("Block Size: %d\nNumber of Sets: %d\nSet Associativity: %d\n", BLOCK_SIZE, NUM_SETS, SET_ASSOCIATIVITY);
    printf("Number of offset bits: %d\nNumber of index bits: %d\nNumber of tag bits: %d\n", OFFSET_BITS, INDEX_BITS, TAG_BITS);
}

void printCacheReport(char cachingMethod)
{
    char* div = "****************************************\n";
    printf("%s",div);
    if(cachingMethod == 'T')
        printf("Write-through with No Write Allocate\n");
    else if(cachingMethod == 'B')
        printf("Write-back with Write Allocate\n");
    printf("%s",div);
}

void printCache(void)
{
    int i;
    for(i = 0; i < NUM_SETS; i++)
    {
        int j;
        for(j = 0; j < SET_ASSOCIATIVITY; j++)
        {
            printf("%d", CACHE->sets[i].data[j]);
        }
    }
}

void printInput(void)
{
    printf("Block Size: %d\nNumber of Sets: %d\nSet Associativity: %d\nLines: %d\n", BLOCK_SIZE, NUM_SETS, SET_ASSOCIATIVITY, LINE_LIST->size);
    printLines();
}

/*----------------------------------*
 *                MAIN              *
 *----------------------------------*/
int main()
{
    // Initialize Dynamic Lines
    initLines();

    // Parse Input
    parseInput();

    // [Debug]: Print Parsed Data
    if(DEBUG_MODE){printInput();}

    // Create Cache
    initCache();

    // Calculate & Print Common/Shared Cache Information
    calculateAddressBits();
    printHeader();

    // @todo Execute Caching Patterns

    // @todo Print Cache Reports
    printCacheReport('T');
    printCacheReport('B');

    // [Debug]: Print Cache
    if(DEBUG_MODE){printCache();}

    // Deinitialize
    deinitLines();
    deinitCache();
}
