#include"parser.h"
#include<stdlib.h>
#include<string.h>
#include<stdio.h>
int tokenizeLine(const char *line,char tokens[MAX_TOKENS][MAX_LINE]){
    int count=0;
    char temp[MAX_LINE];
    strncpy(temp,line,MAX_LINE-1);
    temp[MAX_LINE-1]='\0';
    char *tok=strtok(temp," ,\t\n");
    while(tok != NULL&&count<MAX_TOKENS){
        strncpy(tokens[count],tok,MAX_LINE-1);
        tokens[count][MAX_LINE-1]='\0';
        count++;
        tok=strtok(NULL," ,\t\n");
    }
    return count;
}
ParsedInstr parseInstruction(const char *line,int lineNumber){
    ParsedInstr instr;
    memset(&instr,0,sizeof(instr));
    instr.lineNumber=lineNumber;
    char tokens[MAX_TOKENS][MAX_LINE];
    int count=tokenizeLine(line,tokens);
    if(count==0)
        return instr;
    strncpy(instr.opcode,tokens[0],sizeof(instr.opcode)-1);
    if(strcmp(instr.opcode,"add")==0&&count==4){
        strncpy(instr.rd,tokens[1],sizeof(instr.rd)-1);
        strncpy(instr.rs1,tokens[2],sizeof(instr.rs1)-1);
        strncpy(instr.rs2,tokens[3],sizeof(instr.rs2)-1);
        instr.type=0;
        instr.funct3=0b000;
        instr.funct7=0b0000000;
        instr.opcodeVal=0b0110011;
    }
    else if(strcmp(instr.opcode,"addi")==0&&count==4){
        strncpy(instr.rd,tokens[1],sizeof(instr.rd)-1);
        strncpy(instr.rs1,tokens[2],sizeof(instr.rs1)-1);
        instr.imm=atoi(tokens[3]);
        if(instr.imm<-2048 || instr.imm >2047){
            instr.errorFlag=1;
            sprintf(instr.errorMsg,
                    "Line %d: Immediate out of range for addi",lineNumber);
        }
        instr.type=1;
        instr.funct3=0b000;
        instr.opcodeVal=0b0010011;
    }
    else if(strcmp(instr.opcode,"lw")==0&&count==3){
        strncpy(instr.rd,tokens[1],sizeof(instr.rd)-1);
        char *paren=strchr(tokens[2],'(');
        if(paren==NULL){
            instr.errorFlag=1;
            sprintf(instr.errorMsg,
                    "Line %d: Invalid lw syntax",lineNumber);
        }
        else{
            *paren='\0';
            instr.imm=atoi(tokens[2]);
            strncpy(instr.rs1,paren + 1,sizeof(instr.rs1)-1);
            int len=strlen(instr.rs1);
            if(len > 0&&instr.rs1[len-1]==')')
                instr.rs1[len-1]='\0';
            instr.type=1;
            instr.funct3=0b010;
            instr.opcodeVal=0b0000011;
        }
    }
    else if(strcmp(instr.opcode,"sw")==0&&count==3){
        strncpy(instr.rs2,tokens[1],sizeof(instr.rs2)-1);
        char *paren=strchr(tokens[2],'(');
        if(paren==NULL){
            instr.errorFlag=1;
            sprintf(instr.errorMsg,
                    "Line %d: Invalid sw syntax",lineNumber);
        }
        else{
            *paren='\0';
            instr.imm=atoi(tokens[2]);
            strncpy(instr.rs1,paren +1,sizeof(instr.rs1)-1);
            int len=strlen(instr.rs1);
            if(len >0&&instr.rs1[len-1]==')')
                instr.rs1[len-1]='\0';
            instr.type=2;
            instr.funct3=0b010;
            instr.opcodeVal=0b0100011;
        }
    }
    else if(strcmp(instr.opcode,"beq")==0&&count==4){
        strncpy(instr.rs1,tokens[1],sizeof(instr.rs1)-1);
        strncpy(instr.rs2,tokens[2],sizeof(instr.rs2)-1);
        instr.imm=atoi(tokens[3]);
        instr.type=3;
        instr.funct3=0b000;
        instr.opcodeVal=0b1100011;
    }
    else if(strcmp(instr.opcode,"lui")==0&&count==3){
        strncpy(instr.rd,tokens[1],sizeof(instr.rd)-1);
        instr.imm=atoi(tokens[2]);
        instr.type=4;
        instr.opcodeVal=0b0110111;
    }
    else if(strcmp(instr.opcode,"jal")==0&&count==3){
        strncpy(instr.rd,tokens[1],sizeof(instr.rd)-1);
        instr.imm=atoi(tokens[2]);
        instr.type=5;
        instr.opcodeVal=0b1101111;
    }

    else{
        instr.errorFlag=1;
        sprintf(instr.errorMsg,
                "Line %d: Unknown or invalid instruction '%s'",
                lineNumber,instr.opcode);
    }
    return instr;
}
