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

void freeString(char *str)
{
    if (str == NULL)
        return;

    free(str);
}

void freeAllStrings(char **str, int length)
{
    if (str == NULL)
        return;

    for( int i = 0; i < length; i++) {
        char *pCurr = str[i];
        free(pCurr);
    }

    free(str);
}

char * copyString(const char *str)
{
    if (str == NULL)
        return NULL;

    int strLen = strlen(str);
    char *pDest = malloc(sizeof(char)*strLen+1);
    strcpy(pDest, str);

    return pDest;
}

char ** copyAllStrings(const char **str, int length)
{
    if (str == NULL)
        return NULL;

    char **ppDest = malloc(sizeof(char**) * length);

    if (!ppDest) {
        return NULL;
    }

    for( int i = 0; i < length; i++) {
        const char *pCurr = str[i];
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

// TODO @ccs may return optional arg pointer, might be useful for things like mutual exclusion and such
void addOptionalArgument(ArgParser *argparser, const char *name, const char **shortHand, int numShort, const char **longHand, int numLong, const char * description)
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

void argparser_init(ArgParser *argparser) {
    // Placeholder allocations to use realloc when adding arguments
    argparser->options = calloc(0, sizeof(OptArg));
    argparser->args = calloc(0, sizeof(Arg));
    argparser->positionCounter = -1;
    argparser->numOptions = 0;
    argparser->numArgs = 0;
    argparser->progName = NULL;

    const char* shortHand[] = {"-v"};
    addOptionalArgument(argparser, "version", shortHand, 1, NULL, 0, NULL);
}

void optarg_cleanup(OptArg *opt)
{
    free(opt->name);
    free(opt->description);

    freeAllStrings(opt->shortHand, opt->numShort);
    freeAllStrings(opt->longHand, opt->numLong);
}

void arg_cleanup(Arg *arg)
{
   free(arg->name);
   free(arg->description);
}

void argparser_shutdown(ArgParser *argparser)
{
    // Cleanup of all objects created
    for(int i = 0; i < argparser->numOptions; i++)
    {
        optarg_cleanup(&argparser->options[i]);
    }
    free(argparser->options);

    for(int i = 0; i < argparser->numArgs; i++)
    {
        arg_cleanup(&argparser->args[i]);
    }
    free(argparser->args);

    freeString(argparser->progName);
}




int main(int argc, char *argv[]) {
    ArgParser argparser;
    argparser_init(&argparser);
    parseArgs(&argparser, argc, argv);


    printf("Hello, world!\n");


    argparser_shutdown(&argparser);
}
