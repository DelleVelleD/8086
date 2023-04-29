@echo off
REM usage: test.bat tests\listing_0040_challenge_movs
tcc -run 8086.c %1 > tests\test.asm && nasm tests\test.asm && fc %1 tests\test && rm tests\test.asm && rm tests\test
ruby 8086.rb %1 > tests\test.asm && nasm tests\test.asm && fc %1 tests\test && rm tests\test.asm && rm tests\test