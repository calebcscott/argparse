

add_executable(main main.c)


target_link_libraries(main PUBLIC argparse)

target_include_directories( main PUBLIC
    "${PROJECT_BINARY_DIR}"
    "${PROJECT_SOURCE_DIR}/argparse"
    )

install(
    TARGETS main
    DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
    )
