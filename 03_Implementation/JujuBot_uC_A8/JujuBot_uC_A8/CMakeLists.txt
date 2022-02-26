###########################################################################
# To setup build evirement
# 1. install python3
# 2. install msys2
# 3. install additional packes
#   - $ pacman -Suyy
#   - $ pacman -S base-devel gcc cmake doxygen mingw-w64-x86_64-graphviz mingw-w64-x86_64-python mingw-w64-x86_64-python-pip
###########################################################################


project(JujuBot_uC_A8)
cmake_minimum_required(VERSION 3.00)
set(CMAKE_CXX_STANDARD 11)
include(CTest)

message("")
message("                           OPTIONS")
message("---------------------------------------------------------------")
message("  name               settings (default ^^^^)")
message("")
message("> RunCPPCheck        ON OFF")
message("                        ^^^")
message("")
message("> BuildUnitTests     ON OFF")
message("                     ^^")
message("")
message("> GenerateDocu       ON OFF")
message("                        ^^^")
message("")
message("> GeneratePDFDoc     ON OFF")
message("                        ^^^")
message("")
message("> LogLevel           NONE INFO ERROR DEBUG")
message("                     ^^^^")
message("")
message("")
message(" select option with 'cmake -Dname=setting'")
message("---------------------------------------------------------------")
message("")

option(RunCPPCheck "RunCPPCheck" OFF)
option(BuildUnitTests "BuildUnitTests" ON)
option(GenerateDocu "GenerateDocu" ON)
option(GeneratePDFDoc "GeneratePDFDoc" OFF)

# call "cmake -DLOG_LEVEL=DEBUG .." with the desired log level to enable debugging
message("### USER COMMENT: configuring LogLevel ###")
set(LogLevel "NONE" CACHE STRING "Option for setting the log level (NONE, INFO, ERROR, DEBUG) for the build.")
set_property(CACHE LogLevel PROPERTY STRINGS NONE INFO ERROR DEBUG)

if(LogLevel)
    if(${LogLevel} STREQUAL "DEBUG")
        add_definitions(-DLOG_LEVEL=3)
        message(STATUS "Compiling with DEBUG log level")
    elseif(${LogLevel} STREQUAL "ERROR")
        add_definitions(-DLOG_LEVEL=2)
        message(STATUS "Compiling with ERROR log level")
    elseif(${LogLevel} STREQUAL "INFO")
        add_definitions(-DLOG_LEVEL=1)
        message(STATUS "Ccompiling with INFO log level")
    else()
        add_definitions(-DLOG_LEVEL=0)
        message(STATUS "Compiling without logging")
    endif()
endif()


###########################################################
message("### USER COMMENT: configuring main.exe ###")

add_executable(main
    main.c
    _SUFU/_task1ms.c
    _SUFU/_task10ms.c
    _SUFU/_task100ms.c
    _SUFU/_taskIdle.c
    _SUFU/auxiliaryFunctions.c
    _TestAndSim/src/_test_timerConfig.c
)

target_include_directories(main PRIVATE
    _APLL/
    _FCAL/
    _FCAL/Com
    _MCAL/
    _MCAL/Com
    _MCAL/Timer
    _SUFU/
    _TestAndSim/include
#    C:/msys64/usr/lib/gcc/x86_64-pc-msys/10.2.0/include/c++/
)

set_target_properties(main PROPERTIES CXX_STANDARD 11)
target_compile_options(main PRIVATE -Wall)

#---    Testing ---
# build unittests if requested
if(BuildUnitTests)
    message("### USER COMMENT: Configuring UnitTest.exe ###")
    add_subdirectory(_TestAndSim)
else()
    message("### USER COMMENT: NOT configuring UnitTest.exe ###")
endif()


# run cppcheck if requested
if(RunCPPCheck)
    find_program(CMAKE_CXX_CPPCHECK NAMES cppcheck)
    if (CMAKE_CXX_CPPCHECK)
        list(
            APPEND CMAKE_CXX_CPPCHECK
                "--enable=warning"
                "--force"
                "--inline-suppr"
                "--quiet"
        )
    endif()
endif()


# build documentation if possible
#if(GenerateDocu EQUAL ON)
if(GenerateDocu)
    # pacman -S doxygen mingw-w64-x86_64-graphviz
    find_package(Doxygen)
    if(DOXYGEN_FOUND)
        message(STATUS "Doxygen found - building documentation")
        set(DOXYGEN_HTML_IN ${CMAKE_CURRENT_SOURCE_DIR}/doc/Doxyfile_html.in)
        set(DOXYGEN_HTML_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile_html)

        configure_file(${DOXYGEN_HTML_IN} ${DOXYGEN_HTML_OUT} @ONLY)

        add_custom_target( doc_doxygen_html ALL
            COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_HTML_OUT}
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/doc
            VERBATIM )

        # pdf generation may require the following packages installed:
        #  sudo apt-get install texlive-latex-base
        #  sudo apt-get install texlive-latex-recommended
        #  sudo apt-get install texlive-latex-extra
        #  pacman -S mingw-w64-x86_64-texlive-latex-base mingw-w64-x86_64-texlive-latex-recommended mingw-w64-x86_64-texlive-latex-extra
        if(GeneratePDFDoc EQUAL ON)
            set(DOXYGEN_PDF_IN ${CMAKE_CURRENT_SOURCE_DIR}/doc/Doxyfile_pdf.in)
            set(DOXYGEN_PDF_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile_pdf)

            configure_file(${DOXYGEN_PDF_IN} ${DOXYGEN_PDF_OUT} @ONLY)

            add_custom_target( doc_doxygen_tex ALL
                COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_PDF_OUT}
                WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/doc
                VERBATIM )

            string(TIMESTAMP TS "%Y%m%d")

            add_custom_target(doc_doxygen_pdf ALL
                COMMAND make
                COMMAND find ${CMAKE_CURRENT_SOURCE_DIR}/doc/latex ! -name 'refman.pdf' -type f -exec rm  {} +
                COMMAND mv refman.pdf "JujuBot_uC_A8_${TS}.pdf"
                DEPENDS doc_doxygen_tex
                WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/doc/latex
            )
        endif()
    else()
        message(SEND_ERROR "Doxygen not found. No documentation will be built")
    endif()
else()
    message("### USER COMMENT: Docu not generated per option ###")
endif()


# The custom target and custom command below allow the unit tests
# to be run.
# If you want them to run automatically by CMake, uncomment #ALL
#add_custom_target(
#        run_unit_tests #ALL
#        DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/RunUnitTests.touch hello_test)

#add_custom_command(
#        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/RunUnitTests.touch
#        COMMAND ${CMAKE_CURRENT_BINARY_DIR}/hello_test)
