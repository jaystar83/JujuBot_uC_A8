cmake_minimum_required(VERSION 3.00)
set(CMAKE_CXX_STANDARD 11)

message("### USER COMMENT: FetchContent - googletest ###")
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
)

message("### USER COMMENT: gtest_force_shared_crt ON CACHE BOOL ** FORCE ###")
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

message("### USER COMMENT: FetchContent_MakeAvailable(googletest) ###")
FetchContent_MakeAvailable(googletest)

#---    Testing ---
message("### USER COMMENT: configuring UnitTests.exe ###")

add_executable(UnitTests
    ../_SUFU/_task1ms.c
    ../_SUFU/_task10ms.c
    ../_SUFU/_task100ms.c
    ../_SUFU/_taskIdle.c
    ../_SUFU/auxiliaryFunctions.c
    src/functions.c
    UnitTests.cpp
)

target_include_directories(UnitTests PRIVATE
    ../_APLL/
    ../_FCAL/
    ../_FCAL/Com
    ../_MCAL/
    ../_MCAL/Com
    ../_MCAL/Timer
    ../_SUFU/
    include/
#    C:/msys64/usr/lib/gcc/x86_64-pc-msys/10.2.0/include/c++/
)

target_link_libraries(
    UnitTests
    gtest_main
    pthread
)

include(GoogleTest)
gtest_discover_tests(UnitTests)

set_target_properties(UnitTests PROPERTIES CXX_STANDARD 11)
target_compile_options(UnitTests PRIVATE -Wall)

add_test(UnitTests UnitTests)

#genrate output even for a failed test run
add_custom_target(check ${CMAKE_COMMAND} -E env CTEST_OUTPUT_ON_FAILURE=1
                  ${CMAKE_CTEST_COMMAND} -C $<CONFIG> -verbose
                  WORKING_DIRECTORY ${CMAKE_BINARY_DIR})

