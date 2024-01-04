


add_executable(TestArgParser Test_ArgParse.c)
target_link_libraries(TestArgParser PUBLIC argparse)
target_include_directories( TestArgParser PUBLIC
    "${PROJECT_SOURCE_DIR}/argparse"
	)


add_test(NAME TestArgParser
	COMMAND TestArgParser)
