cmake_minimum_required(VERSION 3.21...3.27)

project(CatchErrors LANGUAGES CXX)

if(PROJECT_IS_TOP_LEVEL)
  message(STATUS "CMake version: ${CMAKE_VERSION}")
  set(CMAKE_CXX_STANDARD 17)
  set(CMAKE_CXX_STANDARD_REQUIRED ON)
  set(CMAKE_CXX_EXTENSIONS NO)
endif()

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
# XXX find_package(Filesystem REQUIRED)

find_package(doctest)
if(doctest_FOUND)
  set(Boost_VERBOSE ON)
  find_package(Boost 1.71 CONFIG COMPONENTS filesystem)
  if(NOT TARGET Boost::filesystem)
    find_package(Boost CONFIG NAMES Boost boost)
  endif()

  if(APPLE)
    add_executable(FilesystemTest FilesystemTest.cpp)
  endif()
endif()

add_executable(OnLeavingScope OnLeavingScope.cpp)
add_executable(UncaughtExceptionCounter UncaughtExceptionCounter.cpp)

if(APPLE AND TARGET Boost::filesystem)
  add_executable(ScopeGuardOnExit ScopeGuardOnExit.cpp)
endif()

if(TARGET Boost::boost)
  add_executable(ErrorHandlerTest src/main.cpp src/ErrorHandler.cpp
                                  src/ErrorHandler.hpp)
  target_link_libraries(ErrorHandlerTest PRIVATE Boost::boost)
endif()

if("${CMAKE_HOST_SYSTEM_NAME}" STREQUAL "Linux")
  find_package(Threads REQUIRED)

  if(TARGET FilesystemTest)
    target_compile_definitions(FilesystemTest PRIVATE CXX_FILESYSTEM_HAVE_FS)
    target_link_libraries(FilesystemTest PRIVATE doctest::doctest
                                                 Threads::Threads)
  endif()

  if(TARGET ErrorHandlerTest)
    target_link_libraries(ErrorHandlerTest PRIVATE dl Threads::Threads)
  endif()
elseif(TARGET Boost::filesystem)
  if(TARGET FilesystemTest)
    target_compile_definitions(FilesystemTest
                               PRIVATE BOOST_FILESYSTEM_NO_DEPRECATED)
    target_link_libraries(FilesystemTest PRIVATE doctest::doctest
                                                 Boost::filesystem)
  endif()

  if(TARGET ScopeGuardOnExit)
    target_link_libraries(ScopeGuardOnExit PRIVATE Boost::filesystem)
  endif()
endif()

option(BUILD_TESTS "Build test programs" ${PROJECT_IS_TOP_LEVEL})
if(BUILD_TESTS)
  enable_testing()

  add_test(NAME OnLeavingScope COMMAND OnLeavingScope)
  add_test(NAME UncaughtExceptionCounter COMMAND UncaughtExceptionCounter)

  find_package(GTest)
  if(GTEST_FOUND)
    add_executable(ScopeGuardTest ScopeGuardTest.cpp)
    target_link_libraries(ScopeGuardTest PRIVATE GTest::GTest GTest::Main)
    add_test(NAME ScopeGuardTest COMMAND ScopeGuardTest)
  endif()

  if(TARGET FilesystemTest)
    add_test(NAME FilesystemTest COMMAND FilesystemTest)
  endif()

  if(TARGET ScopeGuardOnExit)
    add_test(NAME ScopeGuardOnExit COMMAND ScopeGuardOnExit 42)
  endif()

  if(TARGET ErrorHandlerTest)
    add_test(NAME usage COMMAND ErrorHandlerTest)
    add_test(NAME error COMMAND ErrorHandlerTest illegal parameter)
    foreach(nu RANGE -3 11)
      add_test(NAME main_${nu} COMMAND ErrorHandlerTest ${nu})
    endforeach()
  endif()
endif()
