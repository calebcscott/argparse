

#include <stdio.h>
#include <assert.h>
#include <stdbool.h>

#include "argparse.c"


const char *passed_str = "PASSED";
const char *failed_str = "FAILED";

// Probably better as a macro??
bool printMsgAndRetEval(const char *msg, bool eval ) {
	fprintf(stderr, "%-50s %20s\n", msg, eval ? passed_str : failed_str );

	return eval;
}

bool TestSimpleFlagCheck() {
	ARG_FLAGS f = Arg_Flag | Arg_Action_Store_True;
	ARG_FLAGS t = f;

	validateFlags(&t, 0);

	bool eval = printMsgAndRetEval("Test simple flag w/ no invalid flags", f == t);

	if (!eval)
	{
		fprintf(stderr, "\texpected %d ; got %d\n", f, t);
	}

	return eval;
}

bool TestNoFlagsProvided() {
	ARG_FLAGS f = Arg_Value | Arg_Value_String;
	ARG_FLAGS t = Arg_None;

	validateFlags(&t, 0);

	bool eval = printMsgAndRetEval("Test no flag provided", f == t);

	if (!eval)
	{
		fprintf(stderr, "\texpected %d ; got %d\n", f, t);
	}

	return eval;
}


int main(int argc, char *argv[]) {
	int ret_eval = 1;

	ret_eval = TestSimpleFlagCheck() && ret_eval;
	ret_eval = TestNoFlagsProvided() && ret_eval;


	// Flush stderr before possible abort
	fflush(stderr);
	assert( ret_eval == 1 );

	return 0;
}
