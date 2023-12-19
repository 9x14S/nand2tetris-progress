// This is a test program to see how it will be parsed

// Initialize RAM[0] to zero
@0 // Line 1
D=A // Line 2, should be equal to 48
@var // Line 3
M=D // Line 4, should be equal to 12

// Sum two to RAM[0]
@2 // Line 6
D=A  // Line 7, should be equal to 48
@var  // Line 8
M=M+D // Line 9, should be equal to 66

(END)
@END // Line 10
0;JMP // Line 11