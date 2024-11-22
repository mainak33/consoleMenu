# Cmake version
cmake_minimum_required(VERSION 3.16)

# Project name and a few useful settings.
project(
  consoleMenu
  VERSION 0.1
  LANGUAGES CXX)

# GoogleTest requires at least C++14
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Globs
file(GLOB_RECURSE SOURCES src/*.cpp include/*.h)
file(GLOB_RECURSE SOURCES_TEST test/*.cpp)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_library(consoleMenu ${SOURCES})

# Just for example add some compiler flags.
target_compile_options(consoleMenu PUBLIC /std=c++20 /Wall)

# This allows to include files relative to the root of the src directory with a <> pair
target_include_directories(consoleMenu PUBLIC includes)

# This copies all resource files in the build directory.
# We need this, because we want to work with paths relative to the executable.
#file(COPY ${data} DESTINATION resources)




enable_testing()

add_executable(
  TestconsoleMenu
  ${SOURCES}
  ${SOURCES_TEST}
)

target_link_libraries(
    TestconsoleMenu
    consoleMenu
    GTest::gtest_main
)

target_include_directories(
    TestconsoleMenu 
    PUBLIC 
    includes
)

include(GoogleTest)
gtest_discover_tests(TestconsoleMenu)

install(TARGETS consoleMenu DESTINATION "install")

# This is basically a repeat of the file copy instruction that copies the
# resources in the build directory, but here we tell cmake that we want it
# in the package.
#install(DIRECTORY resources DESTINATION example_destination)

# Now comes everything we need, to create a package
# there are a lot more variables you can set, and some
# you need to set for some package types, but we want to
# be minimal here.
set(CPACK_PACKAGE_NAME "consoleMenu")
set(CPACK_PACKAGE_VERSION "0.1.0")

# We don't want to split our program up into several incomplete pieces.
set(CPACK_MONOLITHIC_INSTALL 1)

# This must be last
include(CPack)
