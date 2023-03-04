//================================================
//
// Shawn Fahimi
//
// Project 1 - Hatchling Simulator
// Computer System Fundamentals
//
//================================================

//================================================
// includes and defines
//================================================
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

// ACC-MEM Arithmetic Instructions
#define ADD     0x10    // Add a word from a specific location in memory to the word in the accumulator (leave the result in the accumulator)
#define SUB     0x11    // Subtract a word from a specific location in memory to the word in the accumulator (leave the result in the accumulator)
#define MUL     0x12    // Multiply a word from a specific location in memory to the word in the accumulator (leave the result in the accumulator)
#define DIV     0x13    // Divide a word from a specific location in memory to the word in the accumulator (leave the result in the accumulator)
#define MOD     0x14    // Compute the integer remainder when dividing a word from a specific location in memory to the word in the accumulator (leave the result in the accumulator)

// ACC-MEM Logical Instructions
#define AND     0x20    // Compute the binary AND of a specific memory location and the accumulator (leave the result in the accumulator)
#define ORR     0x21    // Compute the binary OR of a specific memory location and the accumulator (leave the result in the accumulator)
#define NOT     0x22    // Compute the logical NOT of the accumulator (leave the result in the accumulator)
#define XOR     0x23    // Compute the binary XOR of a specific memory location and the accumulator (leave the result in the accumulator)
#define LSR     0x24    // Compute the binary logical shift right (1-bit) of the accumulator (leave the result in the accumulator)
#define ASR     0x25    // Compute the binary arithmetic shift right (1-bit) of the accumulator (leave the result in the accumulator)
#define LSL     0x26    // Compute the binary logical shift left of the accumulator (leave the result in the accumulator)

// Branch Instructions
#define B       0X30    // Branch to a specific location in memory
#define BNEG    0X31    // Branch to a specific location in memory if the accumulator is negative
#define BPOS    0X32    // Branch to a specific location in memory if the accumulator is positive
#define BZRO    0X33    // Branch to a specific location in memory if the accumulator is zero

// ACC-MEM Load/Store Instructions
#define LOAD    0X40    // Load a word from a specific location in memory into the accumulator
#define STOR    0X41    // Store a word from the accumulator into a specific memory location

// I/O (Standard) Instructions
#define READ    0X50    // Read a word from the terminal into a specific location in memory
#define WRTE    0X51    // Write a word from a specific location in memory to the terminal

// HALT Instruction
#define HALT    0XFF   // Halt, i.e. the program has completed its task


//================================================
// struct that holds all relevant information 
// and memory for a Hatchling program
//================================================
struct hatchling{
    signed short int accumulator;   //used to represent the accumulator register
    unsigned short int instructReg; //used to store the instruction being performed
    unsigned char instructCntr;     //keeps track of the location in memory that contains the instruction being performed
    unsigned char opCode;           //upper byte of the instruction register
    unsigned char operand;          //lower byte of the instruction register
    unsigned short int mem[256];    //an array of 256 short int
    bool fatalError;                //flags whether we've encoured a fatal error in the program
};
typedef struct hatchling Hatchling;

//================================================
// function prototypes
//================================================
Hatchling readFile(FILE *f);
Hatchling readProgram();
void execute(Hatchling * hatchling);
void executeInstruction(Hatchling * hatchling);


//================================================
// reads and loads Hatchling program using
// standard input supplied by the user into the 
// terminal (using -99999 as a sentinel), and 
// returns the loaded Hatchling program
//================================================
Hatchling readProgram(){
    Hatchling prog = (Hatchling){0, 0, 0, 0, 0, {}, false};
    int i = 0;
    char line[80];
    printf("%02X    ", i); //print memory location/line number

    //read first line
    scanf("%s", line);
    long ins = strtol(line,NULL,16);
    
    //halts execution if bad instruction is entered
    if((ins < 0x0000 || ins > 0xFFFF) && ins != -0x99999){
        printf("BAD INSTRUCTION ON LINE %02x ", i);
        exit(0);
    }

    /* reads and loads loading instruction/data words into
        the Hatchling program's memory until the
        sentinel value (-99999) is entered */ 
    while(strcmp(line,"-99999")){

        /* assigns input word (casted as an unsigned short) into respective place in 
            program memory */
        prog.mem[i] = (unsigned short)ins;
        int c = i + 1;
        i = c;
        printf("%02X    ", i);
        scanf("%s",line);

        //converts input C-string into a base-16 number
        ins = strtol(line,NULL,16);
        if((ins < 0x0000 || ins > 0xFFFF) && ins != -0x99999){
                int d = i;
                printf("BAD INSTRUCTION ON LINE %02X\n", d);
                printf("PLEASE RE-RUN THE SIMULATOR WITH VALID INSTRUCTION WORDS IN RANGE [0000-FFFF]\n");
                exit(0);
            
        }

    }
    return prog;
}

//================================================
// reads and loads a Hatchling program into memory
//  using a valid user-supplied .hml file
//  Returns the loaded Hatchling program.
//================================================
Hatchling readFile(FILE *f){
    Hatchling prog = (Hatchling){0, 0, 0, 0, 0, {}, false};
    char line[80];
    long ins;
    int i = 0;
    while (fgets(line, 6, f)) {

        //converts line into a base-16 number
        ins = strtol(line,NULL,16);

        //checks for valid instruction words
        if(ins < 0x0000 || ins > 0xFFFF){
            
                printf("BAD INSTRUCTION ON LINE %02X \n", i);
                printf("PLEASE RE-RUN THE SIMULATOR WITH VALID INSTRUCTION WORDS IN RANGE [0000-FFFF]\n");
                exit(0);
            
        }

        prog.mem[i] = (unsigned short) ins;
        i++;
    }
    return prog;
}

//================================================
// executes the Hatchling program and updates
// fields aside from the accumulator and runs 
// through the program until a Halt instruction
// or fatal error is reached
//================================================
void execute(Hatchling * hatchling){

    /*while the current instruction is NOT HALT
        or does not produce a fatal error*/ 
    while(hatchling->opCode != 0xff && hatchling->fatalError == false){
       
        //load the instruction register from program memory
        hatchling->instructReg = (hatchling->mem[hatchling->instructCntr]);
        
        hatchling->opCode = hatchling->instructReg >> 8;
     
        hatchling->operand = hatchling->instructReg & 0xFF;
       
        executeInstruction(hatchling);
    }
    return;
}

//================================================
// executes a single instruction in the provided
// Hatchling program, which is determined by
// the Hatchling's opCode. Function
// manipulates the Hatchling program
// instance's state accordingly
//================================================
void executeInstruction(Hatchling * hatchling){
    
    switch(hatchling->opCode){
        
        case ADD:
        {
            //check for potential overflow
            int sum = hatchling->accumulator + (signed short)hatchling->mem[hatchling->operand];
            if(sum < -32768 || sum > 32767){
                printf("*** ACCUMULATOR OVERFLOW ***\n");
                printf("*** HATCHLING EXECUTION ABNORMALLY TERMINATED ***\n");

                //halts program if operation results in accumulator overflow
                hatchling->fatalError = true;
                return;
            }

            /* replace accumulator value with the sum of itself and the signed data word at the memory 
                location specified by the operand */
            hatchling->accumulator += (signed short)hatchling->mem[hatchling->operand];
            hatchling->instructCntr++;
            return;
        }
        
        case SUB:
        {
            int diff = hatchling->accumulator - (signed short)hatchling->mem[hatchling->operand];
            if(diff < -32768 || diff > 32767){
                printf("*** ACCUMULATOR OVERFLOW ***\n");
                printf("*** HATCHLING EXECUTION ABNORMALLY TERMINATED ***\n");

                //halts program if operation results in accumulator overflow
                hatchling->fatalError = true;
                return;
            }

            /* replace accumulator value with the difference of itself and the signed data word 
                at the memory location specified by the operand */
            hatchling->accumulator -= (signed short)hatchling->mem[hatchling->operand];
            hatchling->instructCntr++;
            return;
        }
        
        case MUL:
        {
            int prod = hatchling->accumulator * (signed short)hatchling->mem[hatchling->operand];
            if(prod < -32768 || prod > 32767){
                printf("*** ACCUMULATOR OVERFLOW ***\n");
                printf("*** HATCHLING EXECUTION ABNORMALLY TERMINATED ***\n");

                //halts program if operation results in accumulator overflow
                hatchling->fatalError = true;
                return;
            }

            /* replace accumulator value with the product of itself and the signed data word 
                at the memory location specified by the operand */
            hatchling->accumulator *= (signed short)hatchling->mem[hatchling->operand];
            hatchling->instructCntr++;
            return;
        }
        
        case DIV:
        {
            if(hatchling->mem[hatchling->operand] == 0){
                printf("*** ATTEMPT TO DIVIDE BY ZERO ***\n");
                printf("*** HATCHLING EXECUTION ABNORMALLY TERMINATED ***\n");

                //halts program if a divide-by-zero is attempted by halting
                hatchling->fatalError = true;
                return;
            }
            int quo = hatchling->accumulator / (signed short)hatchling->mem[hatchling->operand];
            if(quo < -32768 || quo > 32767){
                printf("*** ACCUMULATOR OVERFLOW ***\n");
                printf("*** HATCHLING EXECUTION ABNORMALLY TERMINATED ***\n");

                //halts program if operation results in accumulator overflow
                hatchling->fatalError = true;
                return;
            }

            /* replace accumulator value with the quotient of itself and the signed data word 
                at the memory location specified by the operand */
            hatchling->accumulator /= (signed short)hatchling->mem[hatchling->operand];
            hatchling->instructCntr++;
            return;
        }
        
        case MOD:
        {
            if(hatchling->mem[hatchling->operand] == 0){
                printf("*** ATTEMPT TO DIVIDE BY ZERO ***\n");
                printf("*** HATCHLING EXECUTION ABNORMALLY TERMINATED ***\n");

                //halts program if a divide-by-zero is attempted by halting
                hatchling->fatalError = true;
                return;
            }
            int modul = hatchling->accumulator % (signed short)hatchling->mem[hatchling->operand];
            if(modul < -32768 || modul > 32767){
                printf("*** ACCUMULATOR OVERFLOW ***\n");
                printf("*** HATCHLING EXECUTION ABNORMALLY TERMINATED ***\n");

                //halts program if operation results in accumulator overflow
                hatchling->fatalError = true;
                return;
            }

            /* replace accumulator value with the remainder of itself divided by the
                 signed data word at the memory location specified by the operand */
            hatchling->accumulator %= (signed short)hatchling->mem[hatchling->operand];
            hatchling->instructCntr++;
            return;
        }
       
        case AND:
            /* replace accumulator with the bitwise AND of itself and the data word
                at the memory location specified by the operand */
            hatchling->accumulator &= hatchling->mem[hatchling->operand];
            hatchling->instructCntr++;
            return;
        
        case ORR:
            /* replace accumulator with the bitwise OR of itself and the data word
                at the memory location specified by the operand */
            hatchling->accumulator |= hatchling->mem[hatchling->operand];
            hatchling->instructCntr++;
            return;
        
        case NOT:
            /* replace accumulator with the logical (not bitwise) 
                NOT of itself and the data word at the memory 
                    location specified by the operand */
            hatchling->accumulator = !hatchling->accumulator;
            hatchling->instructCntr++;
            return;
        
        case XOR:
            /* replace accumulator with the bitwise XOR of itself and the data word
                at the memory location specified by the operand */
            hatchling->accumulator ^= hatchling->mem[hatchling->operand];
            hatchling->instructCntr++;
            return;
        
        case LSR:

            // shifts the bits in the accumulator right by 1 bit (logical)
            hatchling->accumulator = hatchling->accumulator >> 1;
            hatchling->instructCntr++;
            return;
        
        case ASR:
        
            // shifts the bits in the accumulator right by 1 bit (preserving sign)
            if(hatchling->accumulator < 0){
                
                //shifts the bits in a negative number right by 1 bit while preserving sign
                hatchling->accumulator = ~(~hatchling->accumulator >> 1);
                hatchling->instructCntr++;
                return;
            }

            hatchling->accumulator = hatchling->accumulator >> 1;
            hatchling->instructCntr++;
            return;
        
        
        case LSL:

            // shifts the bits in the accumulator left by 1 bit (logical)
            hatchling->accumulator = hatchling->accumulator << 1;
            hatchling->instructCntr++;
            return;
        
        case B:
            /* sets the instruction counter to the program memory location
                specified by the operand of the B instruction */
            hatchling->instructCntr = hatchling->operand;
            return;
        
        case BNEG:

            /* sets the instruction counter to the program memory location
                specified by the operand of the BNEG instruction if the 
                accumulator has a negative value */
            if(hatchling->accumulator < 0){
                hatchling->instructCntr = hatchling->operand;
            }
            else{
                hatchling->instructCntr++;
            }
            return;
        
        case BPOS:

            /* sets the instruction counter to the program memory location
                specified by the operand of the BPOS instruction if the 
                accumulator has a positive value */
            if(hatchling->accumulator > 0){
                hatchling->instructCntr = hatchling->operand;
            }
            else{
                hatchling->instructCntr++;
            }
            return;
        
        case BZRO:

            /* sets the instruction counter to the program memory location
                specified by the operand of the BZRO instruction if the 
                accumulator has a value of 0 */
            if(hatchling->accumulator == 0){
                hatchling->instructCntr = hatchling->operand;
            }
            else{
                hatchling->instructCntr++;
            }
            return;
        
        case LOAD:

            //loading the SIGNED data value into the accumulator
            hatchling->accumulator = (signed short)hatchling->mem[hatchling->operand];
            hatchling->instructCntr++;
            return;
        
        case STOR:

            //stores the accumulator's value in the memory location specified by operand
            hatchling->mem[hatchling->operand] = hatchling->accumulator;
            hatchling->instructCntr++;
            return;
        
        case READ: 

            printf("INPUT A SIGNED SHORT INT (BASE 16, USE - FOR NEGATIVE):  ");
            char line[80];
            scanf("%s", line);

            //read in the number as a base 16 value
            long dat = strtol(line,NULL,16);
            if(dat < -32768 || dat > 32767){
                printf("Value out of range (-32768 to 32767, base 10)\n");
                return;
            }
            //otherwise, loading the SIGNED data value into program memory
            hatchling->mem[hatchling->operand] = (signed short)dat;
            hatchling->instructCntr++;
            return;
        
        case WRTE:
            
            //writes value at memory address specified by operand to standard output as a hex number in 2's complement
            printf("OUTPUT: %04hX (REPRESENTED IN BASE 16, 2'S COMPLEMENT)\n", (signed short)hatchling->mem[hatchling->operand]);
            hatchling->instructCntr++;
            return;
        
        case HALT:
            //do nothing here, execute function handles program behavior
            return;
        
        default:
            //halts program if we reach undefined opcode
            printf("*** UNDEFINED HATCHLING OPCODE ***\n");
            printf("*** HATCHLING EXECUTION ABNORMALLY TERMINATED ***\n");
            hatchling->fatalError = true;
            return;
    }
}

//================================================
// main function
//================================================
int main(int argc, char *argv[]){

    //if we're reading from standard input
    if(argc == 1){
        
        Hatchling h = readProgram();
        printf("*** PROGRAM LOADING COMPLETED ***\n");
        printf("*** PROGRAM EXECUTION BEGINS ***\n");
        execute(&h);
        
        //Hatchling computer dump
        printf("*** PROGRAM EXECUTION TERMINATED ***\n\n");
        printf("REGISTERS\n");
        printf("ACC         %04hX\n", h.accumulator);
        printf("InstCtr       %02hX\n", h.instructCntr);
        printf("InstReg     %04hX\n", h.instructReg);
        printf("OpCode        %02hX\n", h.opCode);
        printf("Operand       %02hX\n", h.operand);
        printf("\nMemory: \n");

        //print Hatchling program memory in matrix form
        printf("    ");
            for(int i = 0; i < 16; i++){
                printf("%5X   ", i);
            }

            for(int i = 0; i < 256; i++){
                
                //new row in memory output
                if(i % 16 == 0){
                    printf("\n%2X   ", i);
                }

                printf("%04X    ", h.mem[i]);
            }
            printf("\n");
    }
    
    //if we're reading from hml file
    else if(argc == 2){
        FILE *hp;

        //proceeds only if the 2nd command line argument is a valid file 
        if((hp = (fopen(argv[1], "r")))){
            
            Hatchling hf = readFile(hp);
            fclose(hp);
            printf("*** PROGRAM LOADING COMPLETED ***\n");
            printf("*** PROGRAM EXECUTION BEGINS ***\n");
            execute(&hf);

            //Hatchling computer dump
            printf("*** PROGRAM EXECUTION TERMINATED ***\n\n");
            printf("REGISTERS\n");
            printf("ACC         %04hX\n", hf.accumulator);
            printf("InstCtr       %02hX\n", hf.instructCntr);
            printf("InstReg     %04hX\n", hf.instructReg);
            printf("OpCode        %02hX\n", hf.opCode);
            printf("Operand       %02hX\n", hf.operand);
            printf("\nMemory: \n");

            //prints Hatchling program memory in matrix form
            printf("    ");
            for(int i = 0; i < 16; i++){
                printf("%5X   ", i);
            }

            for(int i = 0; i < 256; i++){
                
                //new row in memory output
                if(i % 16 == 0){
                    printf("\n%2X   ", i);
                }
                printf("%04X    ", hf.mem[i]);
            }
            printf("\n");
        }
        else{
            printf("Please enter a valid filepath\n");
        }
    }
    
    //otherwise, invalid program execution attempt
    else{
        printf("Please enter \"./[executable]\" or \"./[executable] [filepath]\" to run the program\n");
    }
    
    // return to calling environment
    return(0);
}
