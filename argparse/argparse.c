#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "argparse.h"
#include <stdbool.h>


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

void printOptionsHelp(ArgParser *argparser)
{
    printf("OPTIONS:\n");

    for (int i = 0; i < argparser->numOptions; i++)
    {
        char fmtString[4096] = "\t";
        OptArg *arg = &argparser->options[i];
        strcat(fmtString, arg->shortHand[0]);

        
        for(int j = 1; j < arg->numShort + arg->numLong; j++)
        {
            strcat(fmtString, "|");
            if ( j < arg->numShort )
            {
                strcat(fmtString, arg->shortHand[j]);
            }
            else {
                strcat(fmtString, arg->longHand[j - arg->numShort]);
            }
        }

        if ( arg->flags & Arg_Flag == 0)
        {
            strcat(fmtString, "<");
            strcat(fmtString, arg->name);
            strcat(fmtString, ">");

        }

        if (arg->description != NULL && strlen(arg->description) > 0)
        {
            strcat(fmtString, "\t\t");
            strcat(fmtString, arg->description);
        }

        printf("%s\n", fmtString);

    }
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

    printOptionsHelp(argparser);

    argparser_shutdown(argparser);
    exit(error);
}


void validateFlags( ARG_FLAGS *flags )
{

    bool isFlag = ( *flags & Arg_Flag ) != 0; 
    bool isValue = ( *flags & Arg_Value ) != 0;


    if ( isFlag && isValue )
    {
        return;
    }

    if ( isFlag )
    {
        bool validAction = ( (Arg_Action_Store_True | Arg_Action_Store_False) & *flags ) == 0
            || (Arg_Action_Store_True & *flags) | (Arg_Action_Store_False & *flags) == 0;

        if (!validAction)
        {
            *flags = (Arg_Flag | Arg_Action_Store_True );    
        }

        bool validValue = ( (Arg_Value_Int & *flags) | 
                (Arg_Value_Double & *flags) | 
                (Arg_Value_String  & *flags ) ) == 0;
        if ( !validValue )
        {
            // Possible we didn't reset flags in previous step, need to make sure we clear out Value args
            // but keep Any action flag set by user
            *flags &= (Arg_Flag | Arg_Action_Store_True | Arg_Action_Store_False);    
        }
        return;
    }

    if ( isValue )
    {
        bool validAction = (Arg_Action_Store_True & *flags) | (Arg_Action_Store_False  & *flags ) == 0;

        if (!validAction)
        {
            *flags &= ~(Arg_Action_Store_True | Arg_Action_Store_False );
        }

        // Bitwise and to pull out possible Arg_Value_* set in flags
        int oneValueForArg = (Arg_Value_Int | Arg_Value_Double | Arg_Value_String ) & *flags;

        // Test is Value is == 0 which means none or not a power of two which means more than one
        // in either case we need to use default string
        if ( oneValueForArg == 0 || (oneValueForArg & (oneValueForArg - 1)) != 0)
        {
            // rest value flags in return value
            *flags &= ~(Arg_Value_Int | Arg_Value_Double | Arg_Value_String);
            // Set default string flag
            *flags |= Arg_Value_String;
        }

        return;

    }

    *flags = Arg_Value | Arg_Value_String;

}

// TODO @ccs may return optional arg pointer, might be useful for things like mutual exclusion and such
void argparser_add_optional_arg(ArgParser *argparser, const char *name, 
        const char **shortHand, int numShort, const char **longHand, 
        int numLong, const char * description, ARG_FLAGS flags)
{
    if (argparser == NULL)
        return;

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

    validateFlags(&flags);
    opt->flags = flags;
   
}


void parseOptionalArgs(ArgParser *argparser, int *index, int argc, char ***retArgc)
{
}


void parseArgs(ArgParser *argparser, int *index, int argc, char ***retArgc)
{
}


void argparser_parse(ArgParser *argparser, int argc, char *argv[]) {
    if (argparser == NULL)
        return;

    argparser->progName = copyString(argv[0]);
    

    int index = 0;
    char ** tmpArgv = &argv[1];


    // pass triple pointer to allow parse optional arg to change which
    // value we are on
    parseOptionalArgs(argparser, &index, argc, &tmpArgv);


    parseArgs(argparser, &index, argc, &tmpArgv);

    if (index != argc)
    {
        printUsageAndExit(argparser, 1);
    }

}

void argparser_init(ArgParser *argparser) {
    if (argparser == NULL)
        return;
    // Placeholder allocations to use realloc when adding arguments
    argparser->options = calloc(0, sizeof(OptArg));
    argparser->args = calloc(0, sizeof(Arg));
    argparser->positionCounter = -1;
    argparser->numOptions = 0;
    argparser->numArgs = 0;
    argparser->progName = NULL;

    const char* shortHand[] = {"-v"};
    argparser_add_optional_arg(argparser, "version", shortHand, 1, NULL, 0, NULL, Arg_Flag | Arg_Action_Store_True);
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
    if (argparser == NULL)
        return;

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
