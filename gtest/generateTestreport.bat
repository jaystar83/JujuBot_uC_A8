echo Off
:: Parameter an *.bat übergeben ! %0 -> ist Script Name, beginnt mit %1
    	

echo ### Run test ###

start %1 --gtest_output=xml:%2.xml
timeout /T 2

echo ### generate test report###
call xsltproc c:\Repos\JujuBot_uC_A8\gtest\gtest2html\gtest2html.xslt "%2.xml" > "%2.html"

echo ### done ###
echo ### close window in 5sec ###
timeout /T 5

