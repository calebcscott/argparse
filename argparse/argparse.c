#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <stdbool.h>

#include <stdarg.h>


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

void printOptionsHelp(ArgParser *argparser)
{
    if (argparser->numOptions == 0)
        return;
    

    printf("OPTIONS:\n");

    char fmtString[4096] = "   %-20s %20s %s\n";
    for (int i = 0; i < argparser->numOptions; i++)
    {
        // do these even need 4K buffer sizes??
        char optionFlagString[4096] = "";
        char descriptionString[4096] = "";
        OptArg *arg = &argparser->options[i];
        strcat(optionFlagString, arg->shortHand[0]);

        for(int j = 1; j < arg->numShort + arg->numLong; j++)
        {
            strcat(optionFlagString, "|");
            if ( j < arg->numShort )
            {
                strcat(optionFlagString, arg->shortHand[j]);
            }
            else {
                strcat(optionFlagString, arg->longHand[j - arg->numShort]);
            }
        }

        if ( arg->flags & Arg_Flag == 0)
        {
            strcat(optionFlagString, "<");
            strcat(optionFlagString, arg->name);
            strcat(optionFlagString, ">");
        }

        if (arg->description != NULL && strlen(arg->description) > 0)
        {
            strcat(descriptionString, arg->description);
        }

        printf(fmtString, optionFlagString, " ", descriptionString);
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

void printError(ArgParser *argparser, int error, char *fmt, ...)
{
    va_list valist;
    va_start(valist, fmt);
    vprintf(fmt, valist);
    va_end(valist);

    printUsageAndExit(argparser, error);
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
    opt->data = NULL;
   
}

void setOptArgData(ArgParser *argparser, OptArg *arg, char *data)
{
    if ( arg->flags & Arg_Flag )
    {
        arg->data = malloc(sizeof(int));
        if (arg->flags & Arg_Action_Store_True)
            *(int*)arg->data = 1;
        else
            *(int*)arg->data = 0;

    }
    else 
    {
        if ( arg->flags & Arg_Value_Int )
        {
            arg->data = malloc(sizeof(int));
            char *end = NULL;
            char *orig = data;
            *(int*)arg->data = (int)strtol(data, &end, 10);

            if (*end != '\0' && errno != 0)
            {
                printError(argparser, 2, "Invalid value provided for %s argument: %s\n", arg->name, orig);
            }

        }

    }
}

int checkForFlag(OptArg *arg, char *str)
{
    for(int i = 0; i < arg->numShort + arg->numLong; i++)
    {
        if ( i < arg->numShort )
        {
            if (strcmp(str, arg->shortHand[0]) == 0)
                return 1;
        }
        else
        {
            if (strcmp(str, arg->longHand[arg->numShort - i]) == 0)
                return 1;
        }

    }

    return 0;
}

void parseOptionalArgs(ArgParser *argparser, int *index, int argc, char **retArgv[])
{
    printf("Entered parse optional args\n");
    if (*index >= argc)
        return;

    
    printf("Attempting to parse (%d) possible flag: %s\n", *index+1, (**retArgv));
    
    OptArg *arg;
    for(int i = 0; i < argparser->numOptions; i++ )
    {
        arg = &argparser->options[i];

        if ( checkForFlag(arg, **retArgv) )
        {
            printf("Found opt arg with matching flag\n");
            break;
        }

        arg = NULL;
    }

    if (arg == NULL)
        return;

    // Get data at index from retArgv
    *index += 1;
    // Change given pointer to look at next element in list
    *retArgv = (*retArgv) + 1 ;


    return parseOptionalArgs(argparser, index, argc, retArgv);

}


void parseArgs(ArgParser *argparser, int *index, int argc, char ***retArgv)
{
    if (*index >= argc)
        return;
}


void argparser_parse(ArgParser *argparser, int argc, char *argv[]) {
    if (argparser == NULL)
        return;

    argparser->progName = copyString(argv[0]);
    

    int index = 0;
    char ** tmpArgv = &argv[1];


    // pass triple pointer to allow parse optional arg to change which
    // value we are on
    parseOptionalArgs(argparser, &index, argc-1, &tmpArgv);


    parseArgs(argparser, &index, argc-1, &tmpArgv);

    if (index != argc-1)
    {
        printError(argparser, 1, "Invalid argument found: %s\n", *(tmpArgv));
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
    argparser_add_optional_arg(argparser, "version", 
                               shortHand, 1, NULL, 0, "print this usage text",
                               Arg_Flag | Arg_Action_Store_True);
}

void optargCleanup(OptArg *opt)
{
    free(opt->name);
    free(opt->description);

    freeAllStrings(opt->shortHand, opt->numShort);
    freeAllStrings(opt->longHand, opt->numLong);

    free(opt->data);
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
