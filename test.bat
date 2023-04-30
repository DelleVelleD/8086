@echo off
REM usage: test.bat tests\listing_0040_challenge_movs

echo ==== C99  ====
tcc -run 8086.c %1 > tests\test.asm && nasm tests\test.asm && fc %1 tests\test && rm tests\test.asm && rm tests\test

echo ==== Ruby ====
ruby 8086.rb %1 > tests\test.asm && nasm tests\test.asm && fc %1 tests\test && rm tests\test.asm && rm tests\test

echo ==== Rust ====
rustc 8086.rs && 8086.exe %1 > tests\test.asm && nasm tests\test.asm && fc %1 tests\test && rm 8086.exe && rm 8086.pdb && rm tests\test.asm && rm tests\test