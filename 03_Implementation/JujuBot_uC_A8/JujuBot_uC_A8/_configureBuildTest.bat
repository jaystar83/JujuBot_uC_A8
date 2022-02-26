@echo off

cd "build"
CALL "cmake" ..
CALL "make" 
CALL "make" test