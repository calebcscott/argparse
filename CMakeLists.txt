cmake_minimum_required(VERSION 3.27)

project(argparse VERSION 0.1.0)

set(CMAKE_INSTALL_PREFIX ${PROJECT_SOURCE_DIR})


add_subdirectory(argparse)
add_subdirectory(src)


include(CTest)
add_subdirectory(test)

