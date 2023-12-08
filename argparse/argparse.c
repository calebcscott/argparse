#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "argparse.h"


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

void argparser_shutdown(ArgParser *);

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

    argparser_shutdown(argparser);
    exit(error);
}

// TODO @ccs may return optional arg pointer, might be useful for things like mutual exclusion and such
void arparser_add_optional_arg(ArgParser *argparser, const char *name, const char **shortHand, int numShort, const char **longHand, int numLong, const char * description)
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


void parseOptionalArgs(int *index, int argc, char ***retArgc)
{
}


void parseArgs(int *index, int argc, char ***retArgc)
{
}


void argparser_parse(ArgParser *argparser, int argc, char *argv[]) {
    argparser->progName = copyString(argv[0]);
    

    int index = 0;
    char ** tmpArgv = &argv[1];


    // pass triple pointer to allow parse optional arg to change which
    // value we are on
    parseOptionalArgs(&index, argc, &tmpArgv);


    parseArgs(&index, argc, &tmpArgv);

    if (index != argc)
    {
        printUsageAndExit(argparser, 1);
    }

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
    arparser_add_optional_arg(argparser, "version", shortHand, 1, NULL, 0, NULL);
}

void optargCleanup(OptArg *opt)
{
    free(opt->name);
    free(opt->description);

    freeAllStrings(opt->shortHand, opt->numShort);
    freeAllStrings(opt->longHand, opt->numLong);
}

void argCleanup(Arg *arg)
{
   free(arg->name);
   free(arg->description);
}

void argparser_shutdown(ArgParser *argparser)
{
    // Cleanup of all objects created
    for(int i = 0; i < argparser->numOptions; i++)
    {
        optargCleanup(&argparser->options[i]);
    }
    free(argparser->options);

    for(int i = 0; i < argparser->numArgs; i++)
    {
        argCleanup(&argparser->args[i]);
    }
    free(argparser->args);

    freeString(argparser->progName);
}
