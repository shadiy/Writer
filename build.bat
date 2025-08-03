@echo off
clang src/writer.cpp  -o writer.lib -ld2d1 -ldwrite -fuse-ld=llvm-lib

clang test/main.cpp -o test.exe -lwriter -Wl,/ENTRY:mainCRTStartup

