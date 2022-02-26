@echo off

cd "build"
CALL "make" 

cd _TestAndSim

CALL "UnitTests.exe"

