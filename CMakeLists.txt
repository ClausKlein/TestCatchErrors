cmake_minimum_required(VERSION 3.13...3.19)

project(CatchErrors LANGUAGES CXX)

# Determine if this project is built as a subproject (using
# add_subdirectory) or if it is the master project.
set(MASTER_PROJECT OFF)
if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    set(MASTER_PROJECT ON)
    message(STATUS "CMake version: ${CMAKE_VERSION}")
    set(CMAKE_CXX_STANDARD 17)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
    set(CMAKE_CXX_EXTENSIONS NO)
endif()

list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR})
find_package(Filesystem REQUIRED)
find_package(doctest REQUIRED)
set(Boost_VERBOSE ON)
find_package(Boost COMPONENTS filesystem)

add_executable(filesystem_test filesystem_test.cpp)

add_executable(main src/main.cpp
    src/ErrorHandler.cpp
    src/ErrorHandler.hpp
)

if("${CMAKE_HOST_SYSTEM_NAME}" STREQUAL "Linux")
    find_package(Threads REQUIRED)
    target_compile_definitions(filesystem_test PRIVATE CXX_FILESYSTEM_HAVE_FS)
    target_link_libraries(filesystem_test PRIVATE doctest::doctest std::filesystem Threads::Threads)
    target_link_libraries(main PRIVATE dl std::filesystem Threads::Threads)
else()
    target_compile_definitions(filesystem_test PRIVATE BOOST_FILESYSTEM_NO_DEPRECATED)
    target_link_libraries(filesystem_test PRIVATE doctest::doctest Boost::filesystem)
    target_link_libraries(main PRIVATE Boost::headers)
endif()

option(BUILD_TESTS "Build test programs" ${MASTER_PROJECT})
if(BUILD_TESTS)
    enable_testing()

    find_package(GTest)
    if(GTEST_FOUND)
        add_executable(ScopeGuardTest ScopeGuardTest.cpp)
        target_link_libraries(ScopeGuardTest PRIVATE GTest::GTest GTest::Main)
        add_test(NAME ScopeGuardTest COMMAND ScopeGuardTest)
    endif()

    add_test(NAME filesystem_test COMMAND filesystem_test)

    add_test(NAME usage COMMAND main)
    add_test(NAME error COMMAND main illegal parameter)
    foreach(I RANGE -3 11)
        add_test(NAME main_${I} COMMAND main ${I})
    endforeach()
endif()
