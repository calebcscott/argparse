#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include "argparse.h"


int main(int argc, char *argv[]) {
    ArgParser argparser;
    argparser_init(&argparser);
    argparser_parse(&argparser, argc, argv);


    printf("Hello, world!\n");


    argparser_shutdown(&argparser);
}
