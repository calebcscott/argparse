

#ifndef ARGPARSE_H
#define ARGPARSE_H



typedef enum {
    Arg_None = 0,
    
    // ARG OPT FLAGS
    Arg_Flag = 1,
    Arg_Value = 2,
    
    // Arg Opt Flag Actions
    Arg_Action_Store_True = 4,
    Arg_Action_Store_False = 8,

    // Arg Value FLAGS
    Arg_Value_Int = 16,
    Arg_Value_Double = 32,
    Arg_Value_String = 64,
} ARG_FLAGS;

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
    ARG_FLAGS flags;
} OptArg;

// Positional Arg that must be provided
typedef struct Arg {
    char *  name;
    char *  description;
    int     poisition;
    void *  data;
    ARG_FLAGS flags;
} Arg;


// Main Struct that holds info. May abstract these into separate class so they're not known
//  by consumers/modified
typedef struct ArgParser {
    OptArg *    options;
    int         numOptions;
    Arg *       args;
    int         numArgs;
    int         positionCounter;    // Counter for when parsing positional args
    char *      progName;           // Generated on parseArgs call
} ArgParser;


// Init/Shutdown helpers to setup parser correctly
void argparser_init(ArgParser *argparser);
void argparser_shutdown(ArgParser *argparser);

// Adding of args done with these functions, must pass reference to parser object
void argparser_add_optional_arg(ArgParser *argparser, const char *name, 
        const char **shortHand, int numShort, const char **longHand, 
        int numLong, const char * description, ARG_FLAGS flags);

// pass in environment to parse
void argparser_parse(ArgParser *argparser, int argc, char *argv[]);


#endif
