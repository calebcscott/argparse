#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Optional Arg that may not always be provided
typedef struct OptArg{
    char *  name;
    // Short hand names like: -v, -V, etc.
    char ** shortHand;
    int     numShort;
    // long form names like: --version, etc.
    char ** longHand;
    int     numLong;
    char *  description;
    void *  data;
} OptArg;

// Positional Arg that must be provided
typedef struct Arg {
    char *  name;
    char *  description;
    int     poisition;
    void *  data;
} Arg;

typedef struct ArgParser {
    OptArg *    options;
    int         numOptions;
    Arg *       args;
    int         numArgs;
    int         positionCounter;    // Counter for when parsing positional args
    char *      progName;           // Generated on parseArgs call
} ArgParser;

char * copyString(char *str)
{
    if (str == NULL)
        return str;

    int strLen = strlen(str);
    char *pDest = malloc(sizeof(char)*strLen+1);
    strcpy(pDest, str);

    return str;
}

char ** copyAllStrings(char **str, int length)
{
    if (str == NULL)
        return str;

    char **ppDest = malloc(sizeof(char**) * length);

    if (!ppDest) {
        return NULL;
    }

    int i = 0;
    for( int i = 0; i < length; i++) {
        char *pCurr = str[i];
        size_t currLen = strlen(pCurr);
        ppDest[i] = malloc(currLen + 1);
        strcpy(ppDest[i], str[i]);
    }

    return ppDest;
}

void printUsageAndExit(ArgParser *argparser, int error) {
    char fmtString[4096] = "Usage: %s";

    if (argparser->numOptions > 0)
    {
        strcat(fmtString, " [OPTIONS]");
    }

    if (argparser->numArgs > 0)
    {
        for(int i = 0; i < argparser->numArgs; i++)
        {
            strcat(fmtString, " <");
            strcat(fmtString, argparser->args[i].name);
            strcat(fmtString, ">");
        }
    }

    strcat(fmtString, "\n");
    printf(fmtString, argparser->progName);

    exit(error);
}

void addOptionalArgument(ArgParser *argparser, char *name, char **shortHand, int numShort, char **longHand, int numLong, char * description)
{
    argparser->numOptions++;
    argparser->options = realloc(argparser->options, sizeof(OptArg)*argparser->numOptions);

    // Get pointer for newly created optional arg
    //OptArg *opt = argparser->options + (argparser->numOptions - 1);
    OptArg *opt = &argparser->options[argparser->numOptions - 1];    
    opt->name = copyString(name);
    opt->shortHand = copyAllStrings(shortHand, numShort);
    opt->numShort = numShort;
    opt->longHand = copyAllStrings(longHand, numLong);
    opt->numLong = numLong;
    opt->description = copyString(description);

}


void parseArgs(ArgParser *argparser, int argc, char *argv[]) {
    int i = 0;

    argparser->progName = copyString(argv[0]);


    printUsageAndExit(argparser, 0);
}

void initialize_argparser(ArgParser *argparser) {
    // Placeholder allocations to use realloc when adding arguments
    argparser->options = calloc(0, sizeof(OptArg));
    argparser->args = calloc(0, sizeof(Arg));
    argparser->positionCounter = -1;
    argparser->numOptions = 0;
    argparser->numArgs = 0;
    argparser->progName = NULL;

    char* shortHand[] = {"-v"};
    addOptionalArgument(argparser, "version", shortHand, 1, NULL, 0, NULL);
}




int main(int argc, char *argv[]) {
    ArgParser argparser;
    initialize_argparser(&argparser);
    parseArgs(&argparser, argc, argv);


    printf("Hello, world!\n");
}
