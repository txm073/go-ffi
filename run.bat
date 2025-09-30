@echo off
cd lib
call go build -buildmode=c-shared -o goapi.dll
call ..\windows_loader.exe goapi.h
cd ..
call cmake -S . -B build
call cmake --build build
@REM call build\go-ffi.exe 