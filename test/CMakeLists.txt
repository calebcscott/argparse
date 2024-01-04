


add_executable(TestArgParser_ValidateFlags Test_ArgParse_ValidateFlags.c)
target_link_libraries(TestArgParser_ValidateFlags PUBLIC argparse)
target_include_directories( TestArgParser_ValidateFlags PUBLIC
    "${PROJECT_SOURCE_DIR}/argparse"
	)


add_test(NAME TestArgParser_ValidateFlags 
	COMMAND TestArgParser_ValidateFlags )
