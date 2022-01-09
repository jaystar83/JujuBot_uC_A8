project(Hello_Test)

cmake_minimum_required(VERSION 3.00)

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

message("### USER COMMENT: configuring main.exe ###")

add_executable(main
    main.c
    MCAL/Timer/timerConfig.c
    SUFU/_task1ms.c
    SUFU/_task10ms.c
    SUFU/_task100ms.c
    SUFU/_taskIdle.c
    SUFU/auxiliaryFunctions.c
)

target_include_directories(main PRIVATE
    APLL/
    FCAL/
    FCAL/Com
    MCAL/
    MCAL/Com
    MCAL/Timer
    SUFU/
    Test/
    C:/msys64/usr/lib/gcc/x86_64-pc-msys/10.2.0/include/c++/
)

set_target_properties(main PROPERTIES CXX_STANDARD 11)
target_compile_options(main PRIVATE -Wall)

#---    Testing ---
message("### USER COMMENT: configuring hello_test.exe ###")
enable_testing()

add_executable(hello_test
    MCAL/Timer/timerConfig.c
    SUFU/_task1ms.c
    SUFU/_task10ms.c
    SUFU/_task100ms.c
    SUFU/_taskIdle.c
    SUFU/auxiliaryFunctions.c
    Test/hello_test.cpp
)

target_include_directories(hello_test PRIVATE
    APLL/
    FCAL/
    FCAL/Com
    MCAL/
    MCAL/Com
    MCAL/Timer
    SUFU/
    Test/
    C:/msys64/usr/lib/gcc/x86_64-pc-msys/10.2.0/include/c++/
)

target_link_libraries(
    hello_test
    gtest_main
)

include(GoogleTest)
gtest_discover_tests(hello_test)

set_target_properties(hello_test PROPERTIES CXX_STANDARD 11)
target_compile_options(hello_test PRIVATE -Wall)
