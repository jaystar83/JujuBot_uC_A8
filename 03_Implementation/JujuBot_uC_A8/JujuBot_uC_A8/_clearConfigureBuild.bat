@echo off

rmdir /s /q "c/"
rmdir /s /q "_TestAndSim/c"

rmdir /s /q "build"

mkdir "build"
cd "build"
CALL "cmake" ..
CALL "make" 

