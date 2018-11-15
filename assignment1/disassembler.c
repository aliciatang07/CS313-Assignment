#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
#include <inttypes.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include "printRoutines.h"

#define ERROR_RETURN -1
#define SUCCESS 0


char* getRegister(char r)
{
    char ret[5];
    switch(r) {
        case 0x0:
            strcpy(ret,"%rax");
            break;
        case 0x1:
            strcpy(ret,"%rcx");
            break;
        case 0x2:
            strcpy(ret,"%rdx");
            break;
        case 0x3:
            strcpy(ret,"%rbx");
            break;
        case 0x4:
            strcpy(ret,"%rsp");
            break;
        case 0x5:
            strcpy(ret,"%rbp");
            break;
        case 0x6:
            strcpy(ret,"%rsi");
            break;
        case 0x7:
            strcpy(ret,"%rdi");
            break;
        case 0x8:
            strcpy(ret,"%r8");
            break;
        case 0x9:
            strcpy(ret,"%r9");
            break;
        case 0xa:
            strcpy(ret,"%r10");
            break;
        case 0xb:
            strcpy(ret,"%r11");
            break;
        case 0xc:
            strcpy(ret,"%r12");
            break;
        case 0xd:
            strcpy(ret,"%r13");
            break;
        case 0xe:
            strcpy(ret,"%r14");
            break;
        default:
            strcpy(ret, "error");
            break;
    }
    return ret;
}
//print destination
void printDestination(unsigned int* machineCharacter,long pos,FILE* outputFile){
    unsigned int dest[8];
    fprintf(outputFile,"0x");
    for( int i = 7 ; i>=0; i--){
        dest[7-i] = machineCharacter[pos+i+1] ;
        if(dest[7-i]!=0)
        {
            fprintf(outputFile,"%02x",dest[7-i]);
        }
    }
}
//print quad
void quadDestination(unsigned int* machineCharacter,long pos, FILE* outputFile){
    unsigned int dest[8];
    fprintf(outputFile,"0x");
    for( int i = 7 ; i>=0; i--){
        // get the distination;
        dest[8-i-1] = machineCharacter[pos+i] ;
        if(dest[8-i-1]!=0)
        {
            fprintf(outputFile,"%02x",dest[8-i-1]);
        }
    }
}
//print instruction binary code
void printInstruction(unsigned int* machineCharacter, long start_pos,int length, FILE* outputFile){
    unsigned int dest[11];
    for (long i = 0; i < 11; i++)
    {
        if(i<length)
        {
            dest[i]=machineCharacter[start_pos+i];
            fprintf(outputFile,"%02x", dest[i]);
        }
        else
            fprintf(outputFile,"  ");
        
    }
}

//helper method for print invalid instruction as quad or byte
void printAddr(unsigned int* machineCharacter,long currAddr, long pos, int length, FILE* outputFile,int fsize){
    if(length ==8 && fsize-pos>=8){
        fprintf(outputFile,"%016lx: ",currAddr);
        printInstruction(machineCharacter,pos,8,outputFile);
        fprintf(outputFile,".quad    ");
        quadDestination(machineCharacter,pos,outputFile);
    }else {
        printf("jump to byte case");
        printByte(machineCharacter,currAddr,pos,length,outputFile);
    }
}
//print byte
void printByte(unsigned int* machineCharacter,long currAddr, long start_pos,int length, FILE* outputFile){
    unsigned int byte[7];
    //char empty[]="\0";
    for (long i = 0; i < 7; i++)
    {  if(i<length)
    {
        currAddr = currAddr +1;
        fprintf(outputFile,"%016lx: ",currAddr);
        byte[i]=machineCharacter[start_pos+i];
        fprintf(outputFile,"%02x", byte[i]);
        fprintf(outputFile,"                    ");
        fprintf(outputFile,".byte    ");
        fprintf(outputFile,"0x%02x", byte[i]);
        fprintf(outputFile, "\n");
    }
        
    }
}


int main(int argc, char **argv){
    
    FILE *machineCode, *outputFile;
    long currAddr = 0;
    
    // Verify that the command line has an appropriate number
    // of arguments
    
    if (argc < 3 || argc > 4) {
        printf("Usage: %s InputFilename OutputFilename [startingOffset]\n", argv[0]);
        return ERROR_RETURN;
    }
    
    // First argument is the file to read, attempt to open it
    // for reading and verify that the open did occur.
    machineCode = fopen(argv[1], "rb");
    
    if (machineCode == NULL) {
        printf("Failed to open %s: %s\n", argv[1], strerror(errno));
        return ERROR_RETURN;
    }
    
    // Second argument is the file to write, attempt to open it
    // for writing and verify that the open did occur.
    outputFile = fopen(argv[2], "w");
    
    if (outputFile == NULL) {
        printf("Failed to open %s: %s\n", argv[2], strerror(errno));
        fclose(machineCode);
        return ERROR_RETURN;
    }
    
    // If there is a 3rd argument present it is an offset so
    // convert it to a value.
    if (4 == argc) {
        // See man page for strtol() as to why we check for errors by examining errno
        errno = 0;
        currAddr = strtol(argv[3], NULL, 0);
        printf("past in address %d",currAddr);
        if (errno != 0) {
            perror("Invalid offset on command line");
            fclose(machineCode);
            fclose(outputFile);
            return ERROR_RETURN;
        }
    }
    
    printf("Opened %s, starting offset 0x%lX\n", argv[1], currAddr);
    printf("Saving output to %s\n", argv[2]);
    
    /* Comment or delete the following line and this comment before
     * handing in your final version.
     */
    
    // Your code starts here.
    
    
    
    //read whole binary data from mem
    fseek(machineCode, 0, SEEK_END);
    long fsize = ftell(machineCode);
    printf("size %d\n",fsize);
    rewind(machineCode);
    unsigned int *machineCharacter=calloc(fsize,sizeof(int));
    for(int i=0;i<fsize;i++){
        machineCharacter[i]=fgetc(machineCode);
    }
    //declare the start position
    long pos=currAddr;
    while(pos<fsize)
    {
        
        //determine the length of instruction according to icode
        printf("icd!!!! %x\n",machineCharacter[pos]>>4);
        switch(machineCharacter[pos]>>4){
                // 00  halt
            case 0x0:
            {
                printf("%x",machineCharacter[pos]&0x0F);
                
                if(machineCharacter[pos]!=0){
                    //invalid instruction
                    printf("halt instrcution is not correct: 0%x\n",machineCharacter[pos]&0x0F);
                    if(pos+7 < fsize){
                        printAddr(machineCharacter,currAddr,pos,8,outputFile,fsize);
                        pos = pos +8;
                        currAddr += 8;
                        fprintf(outputFile, "\n");
                    }else{
                        printAddr(machineCharacter,currAddr,pos, fsize-pos , outputFile,fsize);
                        pos=fsize;
                    }
                    break;
                    
                }
                else{
                    fprintf(outputFile, "%016lx: %-21s %s\n",
                            currAddr, "00", "halt");
                    printf( "%016lx: %-21s %s\n",
                           currAddr, "00", "halt");
                    currAddr+=1;
                    pos+=1;
                    while(machineCharacter[pos]==0 && pos<fsize){
                        pos+=1;
                        currAddr+=1;
                    }
                    break;
                }
                
                break;
            }
            case 0x1:
            {
                if(machineCharacter[pos]&0x0f != 0){
                    //invalid instruction
                    printf("nop instrcution is not correct: 1%x\n ",machineCharacter[pos]&0x0F);
                    if(pos+7 < fsize)
                    {
                        printAddr(machineCharacter,currAddr,pos, 8, outputFile,fsize);
                        pos = pos +8;
                        currAddr += 8;
                        fprintf(outputFile, "\n");
                    }else{
                        printAddr(machineCharacter,currAddr,pos, fsize-pos , outputFile,fsize);
                        pos=fsize;
                    }
                    break;
                }
                else{
                    fprintf(outputFile, "%016lx: %-21s %s\n",currAddr, "10", "nop");
                    currAddr+=1;
                    pos+=1;
                }
                break;
            }
            case 0x3:
            {
                if(pos+7<fsize)
                {
                    if(machineCharacter[pos]&0x0F!=0x0||machineCharacter[pos]!=0x30){
                        //invalid instruction
                        printAddr(machineCharacter,currAddr,pos, 8, outputFile,fsize);
                        pos = pos + 8;
                        currAddr += 8;
                        fprintf(outputFile, "\n");
                        break;
                    }
                    else
                    {
                        pos+=1;
                        if(machineCharacter[pos]>>4!=0xf)
                        {
                            printAddr(machineCharacter,currAddr,pos-1, 8, outputFile,fsize);
                            pos = pos + 7;
                            currAddr += 8;
                            fprintf(outputFile, "\n");
                            break;
                        }
                        else
                        {
                            if(pos + 8 < fsize)
                            {
                                char* error ="error";
                                char* registerBB = getRegister(machineCharacter[pos]&0x0F);
                                char registerB[5];
                                for(int i=0;i<5;i++){
                                    registerB[i]=registerBB[i];
                                }
                                if (strcmp (registerB , error)==0){
                                    // check if register have error
                                    printAddr(machineCharacter,currAddr,pos-1, 8, outputFile,fsize);
                                    pos = pos + 7;
                                    currAddr += 8;
                                    fprintf(outputFile, "\n");
                                    break;
                                }
                                else
                                {
                                    fprintf(outputFile, "%016lx: ", currAddr);
                                    printInstruction(machineCharacter,pos-1,10, outputFile);
                                    fprintf(outputFile, "%-8s$", "irmovq");
                                    printDestination(machineCharacter,pos,outputFile);
                                    fprintf(outputFile,", %s\n", registerB);
                                    pos+=9;
                                    currAddr+=10;
                                    break;
                                }
                            }
                            else{
                                printAddr(machineCharacter,currAddr,pos-1, 8, outputFile,fsize);
                                pos = pos +7;
                                currAddr += 8;
                                fprintf(outputFile, "\n");
                                break;
                            }
                            
                        }
                    }
                }
                else
                {
                    printAddr(machineCharacter,currAddr,pos, fsize-pos , outputFile,fsize);
                    pos=fsize;
                    break;
                }
                break;
            }
                
            case 0x4:
            {
                if(pos+7 < fsize)
                {
                    if(machineCharacter[pos]&0x0F!=0x0||machineCharacter[pos]!=0x40){
                        printAddr(machineCharacter,currAddr,pos, 8, outputFile,fsize);
                        pos = pos + 8;
                        currAddr += 8;
                        fprintf(outputFile, "\n");
                        break;
                    }
                    else
                    {
                        pos+=1;
                        char* registerAA = getRegister(machineCharacter[pos]>>4);
                        char registerA4[5];
                        for(int i=0;i<5;i++){
                            registerA4[i]=registerAA[i];
                        }
                        char* registerBB = getRegister(machineCharacter[pos]&0x0F);
                        char registerB4[5];
                        for(int i=0;i<5;i++){
                            registerB4[i]=registerBB[i];
                        }
                        char* error="error";
                        printf("registerA %s",registerA4);
                        printf("registerB %s",registerB4);
                        
                        if(pos + 8 < fsize)
                        {
                            if (strcmp (registerB4 , error)==0||strcmp(registerA4,error)==0){
                                // if register have error
                                printAddr(machineCharacter,currAddr,pos-1, 8, outputFile,fsize);
                                pos = pos + 7;
                                currAddr += 8;
                                fprintf(outputFile, "\n");
                                break;
                            }
                            else
                            {
                                fprintf(outputFile, "%016lx: ",currAddr);
                                printInstruction(machineCharacter,pos-1,10,outputFile);
                                fprintf(outputFile,"%-8s", "rmmovq");
                                fprintf(outputFile,"%s, ",registerA4);
                                printDestination(machineCharacter,pos,outputFile);
                                fprintf(outputFile,"(%s)\n",registerB4);
                                pos+=9;
                                currAddr+=10;
                                break;
                            }
                        }
                        else{
                            printAddr(machineCharacter,currAddr,pos-1, 8, outputFile,fsize);
                            pos = pos +7;
                            currAddr += 8;
                            fprintf(outputFile, "\n");
                            break;
                        }
                    }
                }
                
                
                else
                {
                    printAddr(machineCharacter,currAddr,pos, fsize-pos , outputFile,fsize);
                    pos=fsize;
                    break;
                }
                break;
            }
            case 0x5:
            {
                if(pos+7 < fsize)
                {
                    if(machineCharacter[pos]&0x0F!=0x0||machineCharacter[pos]!=0x50){
                        //check ifn
                        printAddr(machineCharacter,currAddr,pos, 8, outputFile,fsize);
                        pos = pos + 8;
                        currAddr += 8;
                        fprintf(outputFile, "\n");
                        break;
                    }
                    else
                    {
                        pos+=1;
                        char* registerAA = getRegister(machineCharacter[pos]>>4);
                        char registerA5[5];
                        for(int i=0;i<5;i++){
                            registerA5[i]=registerAA[i];
                        }
                        
                        char* registerBB = getRegister(machineCharacter[pos]&0x0F);
                        char registerB5[5];
                        for(int i=0;i<5;i++){
                            registerB5[i]=registerBB[i];
                        }
                        char* error="error";
                        if(pos + 8 < fsize)
                        {
                            if (strcmp (registerB5 , error)==0||strcmp(registerA5,error)==0){
                                // check register
                                printAddr(machineCharacter,currAddr,pos-1, 8, outputFile,fsize);
                                pos = pos + 7;
                                currAddr += 8;
                                fprintf(outputFile, "\n");
                                break;
                            }
                            else
                            {
                                fprintf(outputFile, "%016lx: ",
                                        currAddr);
                                printInstruction(machineCharacter,pos-1,10,outputFile);
                                fprintf(outputFile, "%-8s", "mrmovq");
                                printDestination(machineCharacter,pos,outputFile);
                                fprintf(outputFile,"(%s), %s\n",registerB5, registerA5);
                                printf("(%s), %s\n",registerB5, registerA5);
                                pos+=9;
                                currAddr+=10;
                                break;
                            }
                        }
                        else{
                            printAddr(machineCharacter,currAddr,pos-1, 8, outputFile,fsize);
                            pos = pos +7;
                            currAddr += 8;
                            fprintf(outputFile, "\n");
                            break;
                        }
                    }
                }
                
                
                else
                {
                    printAddr(machineCharacter,currAddr,pos, fsize-pos , outputFile,fsize);
                    pos=fsize;
                    break;
                }
                break;
            }
                
            case 0x6:
            {
                if(pos+1<fsize)
                {
                    char instr[6];
                    switch(machineCharacter[pos]&0x0F){
                        case 0x0:
                            strcpy(instr,"addq");
                            break;
                        case 0x1:
                            strcpy(instr,"subq");
                            break;
                        case 0x2:
                            strcpy(instr,"andq");
                            break;
                        case 0x3:
                            strcpy(instr,"xorq");
                            break;
                        case 0x4:
                            strcpy(instr,"mulq");
                            break;
                        case 0x5:
                            strcpy(instr,"divq");
                            break;
                        case 0x6:
                            strcpy(instr,"modq");
                            break;
                        default:
                            strcpy(instr,"error");
                            break;
                    }
                    pos+=1;
                    char* registerAA = getRegister(machineCharacter[pos]>>4);
                    char registerA6[5];
                    for(int i=0;i<5;i++){
                        registerA6[i]=registerAA[i];
                    }
                    char* registerBB = getRegister(machineCharacter[pos]&0x0F);
                    char registerB6[5];
                    for(int i=0;i<5;i++){
                        registerB6[i]=registerBB[i];
                    }
                    char* error="error";
                    
                    if (strcmp (registerB6,error)==0||strcmp(registerA6,error)==0||strcmp(instr,error)==0){
                        //check register error
                        printAddr(machineCharacter,currAddr,pos-1, 8, outputFile,fsize);
                        pos = pos + 7;
                        currAddr += 8;
                        fprintf(outputFile, "\n");
                        break;
                    }
                    fprintf(outputFile,"%016lx: ", currAddr);
                    printf("%016lx: ", currAddr);
                    printInstruction(machineCharacter,pos-1,2,outputFile);
                    fprintf(outputFile,"%-8s", instr);
                    printf("%-8s", instr);
                    fprintf(outputFile,"%s, %s\n",registerA6, registerB6);
                    printf("%s, %s\n", registerA6, registerB6);
                    pos+=1;
                    currAddr+=2;
                    break;
                }
                else{
                    printAddr(machineCharacter,currAddr,pos,fsize-pos,outputFile,fsize);
                    pos=fsize;
                    break;
                }
                break;
                
            }
                
            case 7:
                if(pos + 8 < fsize){
                    char name7[4];
                    switch(machineCharacter[pos] & 0x0F){
                        case 0x0:
                            strcpy(name7,"jmp");
                            break;
                        case 0x1:
                            strcpy(name7,"jle");
                            break;
                        case 0x2:
                            strcpy(name7,"jl");
                            break;
                        case 0x3:
                            strcpy(name7,"je");
                            break;
                        case 0x4:
                            strcpy(name7,"jne");
                            break;
                        case 0x5:
                            strcpy(name7,"jge");
                            break;
                        case 0x6:
                            strcpy(name7,"jg");
                            break;
                        default:
                            strcpy(name7,"error");
                            break;
                    }
                    char* error="error";
                    if(strcmp(name7,error)==0){
                        printAddr(machineCharacter,currAddr,pos-1, 8, outputFile,fsize);
                        pos = pos + 7;
                        currAddr += 8;
                        fprintf(outputFile, "\n");
                        break;
                    }
                    if(pos + 7 == fsize){
                        printAddr(machineCharacter,currAddr,pos-1, 8, outputFile,fsize);
                    }
                    fprintf(outputFile, "%016lx: ",
                            currAddr);
                    printf( "%016lx: ",
                           currAddr);
                    printInstruction(machineCharacter,pos,9,outputFile);
                    fprintf(outputFile, "%-8s", name7);
                    printf( "%-8s",  name7);
                    printDestination(machineCharacter, pos,outputFile);
                    fprintf(outputFile,"\n");
                    pos+=9;
                    currAddr+=9;
                    break;
                }else{
                    printAddr(machineCharacter,currAddr,pos,fsize-pos,outputFile,fsize);
                    pos=fsize;
                }
                break;
                
            case 0x2:
            {
                if(pos+1<fsize)
                {
                    char name8[6];
                    switch(machineCharacter[pos] & 0x0F){
                        case 0x0:
                            strcpy(name8, "rrmovq");
                            break;
                        case 0x1:
                            strcpy(name8,"cmovle");
                            break;
                        case 0x2:
                            strcpy(name8,"cmovl");
                            break;
                        case 0x3:
                            strcpy(name8,"cmove");
                            break;
                        case 0x4:
                            strcpy(name8,"cmovge");
                            break;
                        case 0x5:
                            strcpy(name8,"cmovge");
                            break;
                        case 0x6:
                            strcpy(name8,"cmovg");
                            break;
                        default:
                            strcpy(name8,"error");
                            break;
                    }
                    pos+=1;
                    char* registerAA = getRegister(machineCharacter[pos]>>4);
                    char registerA8[5];
                    for(int i=0;i<5;i++){
                        registerA8[i]=registerAA[i];
                    }
                    char* registerBB = getRegister(machineCharacter[pos]&0x0F);
                    char registerB8[5];
                    for(int i=0;i<5;i++){
                        registerB8[i]=registerBB[i];
                    }
                    char* error="error";
                    if (strcmp (registerB8,error)==0||strcmp(registerA8,error)==0||strcmp(name8,error)==0){
                        printAddr(machineCharacter,currAddr,pos-1, 8, outputFile,fsize);
                        pos = pos + 7;
                        currAddr += 8;
                        fprintf(outputFile, "\n");
                        break;
                    }
                    fprintf(outputFile, "%016lx: ",currAddr);
                    printInstruction(machineCharacter,pos-1,2,outputFile);
                    fprintf(outputFile, "%-8s%s, %s\n", name8, registerA8, registerB8);
                    pos+=1;
                    currAddr+=2;
                    break;
                }
                else{
                    printAddr(machineCharacter,currAddr,pos,fsize-pos,outputFile,fsize);
                    pos=fsize;
                    break;
                }
                break;
                
            }
                
            case 0x8:
                
                if(pos + 8 < fsize){
                    if((machineCharacter[pos]&0x0F!=0x0)||(pos + 7 == fsize)||machineCharacter[pos]!=0x80){
                        printAddr(machineCharacter,currAddr,pos, 8, outputFile,fsize);
                        pos = pos + 8;
                        currAddr += 8;
                        fprintf(outputFile, "\n");
                        break;
                    }
                    else {
                        fprintf(outputFile, "%016lx: ",currAddr);
                        printInstruction(machineCharacter,pos,9,outputFile);
                        fprintf(outputFile, "%-8s", "call ");
                        printDestination(machineCharacter,pos,outputFile);
                        fprintf(outputFile, "\n");
                        pos=pos+9;
                        currAddr=currAddr+9;
                    }
                    break;
                }
                else{
                    printAddr(machineCharacter,currAddr,pos,fsize-pos,outputFile,fsize);
                    pos=fsize;
                    break;
                    
                }
                
            case 0x9:
            {
                if(machineCharacter[pos] & 0x0F!=0x0||machineCharacter[pos]!=0x90){
                    if(pos+7 < fsize){
                        printAddr(machineCharacter,currAddr,pos, 8, outputFile,fsize);
                        pos = pos +8;
                        currAddr += 8;
                        fprintf(outputFile, "\n");
                    }else{
                        printAddr(machineCharacter,currAddr,pos, fsize-pos , outputFile,fsize);
                        pos=fsize;
                    }
                    break;
                }
                else {
                    fprintf(outputFile, "%016lx: %-21s %s\n",
                            currAddr, "90", "ret");
                    pos +=1;
                    currAddr +=1;
                }
                pos +=1;
                break;
            }
                
            case 0xa:
            {
                if(machineCharacter[pos] & 0x0F!=0x0||machineCharacter[pos]!=0xa0){
                    if(pos+7 < fsize){
                        printAddr(machineCharacter,currAddr,pos,8,outputFile,fsize);
                        pos+=8;
                        currAddr += 8;
                        fprintf(outputFile, "\n");
                        break;
                    }else{
                        printAddr(machineCharacter,currAddr,pos,fsize-pos , outputFile,fsize);
                        pos=fsize;
                        break;
                    }
                }
                else {
                    if(pos+1<fsize)
                    {
                        pos = pos +1;
                        char* registerAA = getRegister(machineCharacter[pos]>>4);
                        char registerA[5];
                        for(int i=0;i<5;i++){
                            registerA[i]=registerAA[i];
                        }
                        char* error ="error";
                        if (strcmp (registerA , error)==0||(machineCharacter[pos]&0x0F !=0x0f)){
                            // check register
                            printAddr(machineCharacter,currAddr,pos-1, 8, outputFile,fsize);
                            pos = pos + 7;
                            currAddr += 8;
                            fprintf(outputFile, "\n");
                            break;
                        }
                        else
                        {
                            fprintf(outputFile, "%016lx: ",currAddr);
                            printInstruction(machineCharacter,pos-1,2,outputFile);
                            fprintf(outputFile, "%-8s%s\n", "pushq",registerA );
                            pos+=1;
                            currAddr+=2;
                            break;
                        }
                    }
                    else{
                        printAddr(machineCharacter,currAddr,pos,fsize-pos , outputFile,fsize);
                        pos=fsize;
                    }
                }
                break;
            }
            case 0xb:
            {
                if(machineCharacter[pos] & 0x0F!=0x0||machineCharacter[pos]!=0xb0){
                    if(pos+7 < fsize){
                        printAddr(machineCharacter,currAddr,pos,8,outputFile,fsize);
                        pos+=8;
                        currAddr += 8;
                        fprintf(outputFile, "\n");
                    }else{
                        printAddr(machineCharacter,currAddr,pos,fsize-pos , outputFile,fsize);
                        pos=fsize;
                    }
                    break;
                }
                else {
                    if(pos+1<fsize)
                    {
                        pos = pos +1;
                        char * registerAA= getRegister(machineCharacter[pos]>>4);
                        char registerA[5];
                        for(int i=0;i<5;i++){
                            registerA[i]=registerAA[i];
                        }
                        char* error ="error";
                        if (strcmp (registerA ,error)==0||machineCharacter[pos]&0x0F!=0x0f){
                            // if register have error
                            printAddr(machineCharacter,currAddr,pos-1, 8, outputFile,fsize);
                            pos = pos + 7;
                            currAddr += 8;
                            fprintf(outputFile, "\n");
                            break;
                        }
                        else
                        {
                            fprintf(outputFile, "%016lx: ",
                                    currAddr);
                            printInstruction(machineCharacter,pos-1,2,outputFile);
                            fprintf(outputFile, "%-8s%s\n",  "popq",registerA );
                            pos+=1;
                            currAddr+=2;
                            break;
                            
                        }
                    }
                    else{
                        printAddr(machineCharacter,currAddr,pos,fsize-pos , outputFile,fsize);
                        pos=fsize;
                        break;
                    }
                }
            }
            default:
                //invalid instruction
                printAddr(machineCharacter,currAddr,pos, 8, outputFile,fsize);
                pos = pos + 8;
                currAddr += 8;
                fprintf(outputFile, "\n");
                break;
        }
    }
    fclose(machineCode);
    fclose(outputFile);
    free(machineCharacter);
    return SUCCESS;
}


