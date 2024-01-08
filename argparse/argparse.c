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

    const char defaultFmtString[] = "%4s%-30s %20s %s\n";
    for (int i = 0; i < argparser->numOptions; i++)
    {
        // do these even need 4K buffer sizes??
        char optionFlagString[1024] = "";
        // maybe devise way to add new lines if reach 80 char limit(?)
        // will need to add necessary spacing
        char descriptionString[4096] = "";
        char fmtString[4096];
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

        if ( (arg->flags & Arg_Flag ) == 0)
        {
            strcat(optionFlagString, "<");
            strcat(optionFlagString, arg->name);
            strcat(optionFlagString, ">");
        }

        if (arg->description != NULL && strlen(arg->description) > 0)
        {
            strcat(descriptionString, arg->description);
        }

        int flagLen = strlen(optionFlagString);

        if ( flagLen > 30 && flagLen < 50 )
        {
            int leftoverLen = 20 - ( flagLen - 30);
            snprintf(fmtString, 4095, "%%4s%%-%ds %%%ds %%s\n", flagLen, leftoverLen );
        }
        else if ( flagLen > 50 )
        {
            snprintf(fmtString, 4095, "%%4s%%-30s %%s %%s\n");
        }
        else {
            snprintf(fmtString, 4095, "%s", defaultFmtString);
        }

        printf(fmtString, " ", optionFlagString, " ", descriptionString);
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

void validateFlags( ARG_FLAGS *flags , ARG_FLAGS invalid_flags)
{

    bool isFlag = ( *flags & Arg_Flag ) != 0; 
    bool isValue = ( *flags & Arg_Value ) != 0;


    // Maybe just straight fail?? Or panic??
    if ( isFlag && isValue )
    {
        *flags = Arg_None;
    }

    if ( (*flags & invalid_flags) != 0)
    {
        *flags = Arg_None;
    }

    if ( isFlag )
    {
        bool inValidAction = ( (Arg_Action_Store_True | Arg_Action_Store_False) & *flags ) == 0
            && ( (Arg_Action_Store_True & *flags) | (Arg_Action_Store_False & *flags) ) == 0;

        if (inValidAction )
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
        bool validAction = ((Arg_Action_Store_True & *flags) | (Arg_Action_Store_False  & *flags )) == 0;

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


void argparser_add_arg(ArgParser *argparser, const char *name, const char *desc,
                       int position, ARG_FLAGS flags)
{

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

    validateFlags(&flags, Arg_None);
    opt->flags = flags;
    opt->data = NULL;
   
}

int setOptArgData(ArgParser *argparser, OptArg *arg, char *data)
{
    if ( arg->flags & Arg_Flag )
    {
        arg->data = malloc(sizeof(int));
        if (arg->flags & Arg_Action_Store_True)
            *(int*)arg->data = 1;
        else
            *(int*)arg->data = 0;

        return 0;

    }
    else 
    {
        if ( data == NULL )
        {
            // Possibly reached end of argv so break out
            printError(argparser, 2, "No value provided for %s argument\n", arg->name);
        }
        // Parse value type with corresponding function,
        // probably easier to use scanf?, but wont know if string is invalid
        // maybe if format string was life %d%s and if %s is NULL then we should be good?
        if ( arg->flags & Arg_Value_Int )
        {
            arg->data = malloc(sizeof(int));
            char *end = NULL;
            char *orig = data;
            *(int*)arg->data = (int)strtol(orig, &end, 10);

            if ( orig == end || *end != '\0' )
            {
                printError(argparser, 2, "Invalid value provided for %s argument: %s\n", arg->name, orig);
                return 0;
            }
        }

        return 1;

    }

    return 0;
}

int checkForFlag(OptArg *arg, const char *str)
{
    for(int i = 0; i < arg->numShort + arg->numLong; i++)
    {
        if ( i < arg->numShort )
        {
            if (strcmp(str, arg->shortHand[i]) == 0)
                return 1;
        }
        else
        {
            if (strcmp(str, arg->longHand[i - arg->numShort]) == 0)
                return 1;
        }
    }

    return 0;
}

void parseOptionalArgs(ArgParser *argparser, int *index, int argc, char **retArgv[])
{
    if (*index >= argc)
        return;

    OptArg *arg;
    for(int i = 0; i < argparser->numOptions; i++ )
    {
        arg = &argparser->options[i];

        if ( checkForFlag(arg, **retArgv) )
        {
            break;
        }

        arg = NULL;
    }

    if (arg == NULL)
        return;

    if (arg->data != NULL)
        printError(argparser, 1, "Provided '%s' argument more than once, see help for argument usage.\n", **retArgv);

    // Get data at index from retArgv
    *index += 1;
    // Change given pointer to look at next element in list
    *retArgv = (*retArgv) + 1;

    // If optional arg took value from list, increment to next argument string to parse
    // Otherwise argument was flag thus not value taken from list
    if ( setOptArgData(argparser, arg, **retArgv) )
    {
        *index += 1;
        *retArgv = (*retArgv) + 1;

    }

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

    // Help/Version is always first option
    if (argparser->options->data != NULL )
    {
        printUsageAndExit(argparser, 0);
    }

}


Arg* getArg(ArgParser *argparser, const char *arg_name) {
    Arg *arg = NULL;
    for(int i = 0; i < argparser->numArgs; i++ )
    {
        arg = &argparser->args[i];

        if ( strcmp(arg->name, arg_name) == 0 )
        {
            break;
        }

        arg = NULL;
    }

    return arg;
}

OptArg* getOptArg(ArgParser *argparser, const char *arg_name) {
    OptArg *arg = NULL;
    for(int i = 0; i < argparser->numOptions; i++ )
    {
        arg = &argparser->options[i];

        if ( strcmp(arg->name, arg_name) == 0 )
        {
            break;
        }

        arg = NULL;
    }


    return arg;
}

/**
*  separate procedure for checking flags to not clutter main get data func
*   use memcpy to give the caller the data rather than just a ref, this way
*   if user calls shutdown they will still have access to values.
*
*   this does incur a double allocation for the same data, but ¯\_(ツ)_/¯
*/
bool assignData(void *inData, void *outData, ARG_FLAGS flags) {
    // quick check to see if data is available
    // may change to check a found flag if implemented
    if (inData == NULL || outData == NULL)
        return false;

    bool isFlag = ( flags & Arg_Flag ) != 0; 
    bool isValue = ( flags & Arg_Value ) != 0;

    if ( isFlag )
    {
        memcpy(outData, inData, sizeof(int));
    }

    if ( isValue )
    {
        if ( flags & Arg_Value_Int )
        {
            memcpy(outData, inData, sizeof(int));
        }

        //TODO: implement rest of values

    }

    return true;
}

int argparser_get_data(ArgParser *argparser, const char *arg_name, void *data) {
    if (argparser == NULL)
        return false;

    void *arg;
    // Check if provided arg name is opt arg or positional, then copy out data
    if ( (arg = (void*)getOptArg(argparser, arg_name) ) != NULL 
        && assignData( ((OptArg*)arg)->data, data, ((OptArg*)arg)->flags) ) {;
        return true;
    }
    else if ( (arg = (void*)getArg(argparser, arg_name) ) != NULL
                && assignData( ((Arg*)arg)->data, data, ((Arg*)arg)->flags) ) {
        return true;
    }

    return false;
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

    const char* shortHand[] = {"-v", "-h"};
    const char* longHand[] = {"--version", "--help"};
    argparser_add_optional_arg(argparser, "help/version", 
                               shortHand, 2, longHand, 2, "Print this usage text and version",
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
