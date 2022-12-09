cmake_minimum_required(VERSION 3.21...3.25)

project(CatchErrors LANGUAGES CXX)

if(PROJECT_IS_TOP_LEVEL)
  message(STATUS "CMake version: ${CMAKE_VERSION}")
  set(CMAKE_CXX_STANDARD 17)
  set(CMAKE_CXX_STANDARD_REQUIRED ON)
  set(CMAKE_CXX_EXTENSIONS NO)
endif()

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
# XXX find_package(Filesystem REQUIRED)
find_package(doctest REQUIRED)

# set(Boost_VERBOSE ON) find_package(Boost 1.71 CONFIG COMPONENTS filesystem)
if(NOT TARGET Boost::filesystem)
  find_package(Boost CONFIG NAMES Boost boost REQUIRED)
endif()

add_executable(OnLeavingScope OnLeavingScope.cpp)
add_executable(UncaughtExceptionCounter UncaughtExceptionCounter.cpp)
add_executable(FilesystemTest FilesystemTest.cpp)

if(TARGET Boost::filesystem)
  add_executable(ScopeGuardOnExit ScopeGuardOnExit.cpp)
endif()

add_executable(ErrorHandlerTest src/main.cpp src/ErrorHandler.cpp
                                src/ErrorHandler.hpp)

if("${CMAKE_HOST_SYSTEM_NAME}" STREQUAL "Linux")
  find_package(Threads REQUIRED)

  target_compile_definitions(FilesystemTest PRIVATE CXX_FILESYSTEM_HAVE_FS)
  target_link_libraries(FilesystemTest PRIVATE doctest::doctest
                                               Threads::Threads)

  target_link_libraries(ErrorHandlerTest PRIVATE dl Threads::Threads)
elseif(TARGET Boost::filesystem)
  target_compile_definitions(FilesystemTest
                             PRIVATE BOOST_FILESYSTEM_NO_DEPRECATED)
  target_link_libraries(FilesystemTest PRIVATE doctest::doctest
                                               Boost::filesystem)
  target_link_libraries(ScopeGuardOnExit PRIVATE Boost::filesystem)
endif()

target_link_libraries(ErrorHandlerTest PRIVATE Boost::boost)

option(BUILD_TESTS "Build test programs" ${PROJECT_IS_TOP_LEVEL})
if(BUILD_TESTS)
  enable_testing()

  find_package(GTest)
  if(GTEST_FOUND)
    add_executable(ScopeGuardTest ScopeGuardTest.cpp)
    target_link_libraries(ScopeGuardTest PRIVATE GTest::GTest GTest::Main)
    add_test(NAME ScopeGuardTest COMMAND ScopeGuardTest)
  endif()

  if(TARGET Boost::filesystem)
    add_test(NAME FilesystemTest COMMAND FilesystemTest)
  endif()

  add_test(NAME usage COMMAND ErrorHandlerTest)
  add_test(NAME error COMMAND ErrorHandlerTest illegal parameter)
  foreach(I RANGE -3 11)
    add_test(NAME main_${I} COMMAND ErrorHandlerTest ${I})
  endforeach()
endif()
