cmake_minimum_required(VERSION 3.1...3.19)

# Fallback for using newer policies on CMake <3.12.
if(${CMAKE_VERSION} VERSION_LESS 3.12)
  cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
endif()

project(CatchErrors LANGUAGES CXX)

# Determine if fmt is built as a subproject (using add_subdirectory)
# or if it is the master project.
set(MASTER_PROJECT OFF)
if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    set(MASTER_PROJECT ON)
    message(STATUS "CMake version: ${CMAKE_VERSION}")
endif()

add_executable(main src/main.cpp
    src/ErrorHandler.cpp
    src/ErrorHandler.h
)
target_compile_features(main PRIVATE cxx_std_17)

option(BUILD_TESTS "Build test programs" ${MASTER_PROJECT})
if(BUILD_TESTS)
    enable_testing()

    add_test(NAME usage COMMAND main)
    foreach(I RANGE 11)
        add_test(NAME main_${I} COMMAND main ${I})
    endforeach()
endif()
