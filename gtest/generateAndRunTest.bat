echo Off
:: Parameter an *.bat übergeben ! %0 -> ist Script Name, beginnt mit %1

:: ----------------------------------------------------------------
:: ---  Configuration   -------------------------------------------
:: ----------------------------------------------------------------
set cmakePath=C:\Repos\JujuBot_uC_A8\gtest
set buildPath=C:\Repos\JujuBot_uC_A8\gtest\build

set xmlConvertToolPath=C:\Repos\JujuBot_uC_A8\gtest\gtest2html\gtest2html.xslt

set testExePath=C:\Repos\JujuBot_uC_A8\gtest\build\hello_test.exe

::set testReportPath=C:\Repos\JujuBot_uC_A8\gtest\build\testReport
set relativeTestReportPath=build\testReport

echo ### run cmake ###
call cmake -H%cmakePath% -B%buildPath%
::call cmake -HC:\Repos\JujuBot_uC_A8\gtest -BC:\Repos\JujuBot_uC_A8\gtest\build

echo ---
echo ### build test ###
echo ---
call cmake --build %buildPath% -j 
::call cmake --build C:\Repos\JujuBot_uC_A8\gtest\build -j 

echo ---
echo ### run test ###
echo ---
call %testExePath% --gtest_output=xml:%relativeTestReportPath%.xml
::call %1 --gtest_output=xml:%2.xml
::timeout /T 3

echo ---
echo ### generate test report###
echo ---
call xsltproc %xmlConvertToolPath% %relativeTestReportPath%.xml > %relativeTestReportPath%.html
::call xsltproc c:\Repos\JujuBot_uC_A8\gtest\gtest2html\gtest2html.xslt "%2.xml" > "%2.html"

echo ---
echo ### open test report ###
echo ---
call %relativeTestReportPath%.html

echo ---
echo ### done ###
echo ### close window in 5sec ###
echo ---
timeout /T 5

