#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include "argparse.h"


int main(int argc, char *argv[]) {
    ArgParser argparser;
    argparser_init(&argparser);

    const char *shortH[] = {"-l"};
    const char *longH[] = {"--long"};
    argparser_add_optional_arg(&argparser, "long", shortH, 1, longH, 1, "simple description", Arg_Flag );

    const char *ishortH[] = {"-i"};
    const char *ilongH[] = {"--int"};
    argparser_add_optional_arg(&argparser, "int", ishortH, 1, ilongH, 1, "simple description", Arg_Value | Arg_Value_Int );

    argparser_parse(&argparser, argc, argv);
    printf("Hello, world!\n");

    int long_flag = 0;
    int int_value = 0;

    argparser_get_data(&argparser, "long", (void*)&long_flag);
    argparser_get_data(&argparser, "int", (void*)&int_value);


    printf("Got the values: %d, %d\n", long_flag, int_value);


    argparser_shutdown(&argparser);
}
