@echo off
tcc -run 8086.c %1 > tests\test.asm && nasm tests\test.asm && fc %1 tests\test && rm tests\test.asm && rm tests\test