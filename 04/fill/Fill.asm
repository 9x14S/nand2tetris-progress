// This file is part of www.nand2tetris.org
// and the book "The Elements of Computing Systems"
// by Nisan and Schocken, MIT Press.
// File name: projects/04/Fill.asm

// Runs an infinite loop that listens to the keyboard input.
// When a key is pressed (any key), the program blackens the screen,
// i.e. writes "black" in every pixel;
// the screen should remain fully black as long as the key is pressed. 
// When no key is pressed, the program clears the screen, i.e. writes
// "white" in every pixel;
// the screen should remain fully clear as long as no key is pressed.


(START)
//  R0 has the fill value
    @R0
    M=0
//  R1 is the compare value
    @R1
    M=0
//  R2 is the screen pointer
    @SCREEN 
    D=A
    @R2
    M=D
//  R3 is the return pointer
    @R3
    M=0 


// Check whether a key is being pressed
    @R3
    M=0
    @TRUERETURN
    D=A
    @R3 
    M=D
    @KBD
    D=M
    @TRUE
        D;JGT 
    (TRUERETURN)

    @FALSERETURN
    D=A 
    @R3 
    M=D
    @KBD
    D=M 
    @FALSE
        D;JEQ 
    (FALSERETURN)


(WHILE)
    @CMP
        0;JMP

    (CHECKED)
    @R3
    D=M 
    @START
        D;JEQ
    
    @R0
    D=M 
    @R2
    A=M 
    M=D
    @KBD 
    D=A 
    @R2
    D=D-M 
    @START
        D;JLE
    @R2
    M=M+1


    @WHILE
        0;JMP

(CMP)
    @KBD
    D=M 
    @GT
        D;JNE

    @EQ
        D;JEQ
(GT)
    @R1
    D=M 
    D=D-1
    @R3
    M=1
    @CHECKED
        D;JEQ
    @R3
    M=0
    @CHECKED
        0;JMP

(EQ)
    @R1
    D=M 
    D=D-1
    @R3
    M=1
    @CHECKED
        D;JLT
    @R3
    M=0
    @CHECKED
        0;JMP


(TRUE)
    @R0
    M=-1
    @R1
    M=1

    @R3
        A=M 
        0;JMP

(FALSE)
    @R0
    M=0
    @R1
    M=0

    @R3
        A=M 
        0;JMP


(END)
    @END
        0;JMP