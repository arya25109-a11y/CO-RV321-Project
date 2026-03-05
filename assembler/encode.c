#include "isa.h"
#include "encode.h"
#include <stdio.h>
#include <string.h>
int getRegisterCode(const char*reg){
    extern const RegisterMap registerTable[];
    for (int i=0; i<sizeof(registerTable)/sizeof(registerTable[0]);i++){
        if (strcmp(registerTable[i].name,reg)==0) {
            return registerTable[i].code;
        }
    }
    return -1;
}

// R-type
uint32_t encodeRType(const char*rd,const char*rs1,const char*rs2,int funct3,int funct7,int opcode){
    int rdCode=getRegisterCode(rd);
    int rs1Code=getRegisterCode(rs1);
    int rs2Code=getRegisterCode(rs2);
    if (rdCode<0 || rs1Code<0 || rs2Code<0) 
    return 0;

    return ((funct7&0x7F)<<25) |
           ((rs2Code&0x1F)<<20) |
           ((rs1Code&0x1F)<<15) |
           ((funct3&0x7)<<12) |
           ((rdCode&0x1F)<<7) |
           (opcode&0x7F);
}

// I-type
uint32_t encodeIType(const char*rd,const char*rs1,int imm,int funct3,int opcode){
    if (imm<-2048 || imm>2047) 
    return 0;
    int rdCode=getRegisterCode(rd);
    int rs1Code=getRegisterCode(rs1);
    if (rdCode<0 || rs1Code<0) 
    return 0;

    return ((imm&0xFFF)<<20) |
           ((rs1Code&0x1F)<<15) |
           ((funct3&0x7)<<12) |
           ((rdCode&0x1F)<<7) |
           (opcode&0x7F);
}

// S-type
uint32_t encodeSType(const char*rs2,const char*rs1,int imm,int funct3,int opcode){
    if (imm<-2048 || imm>2047) 
    return 0;
    int rs1Code=getRegisterCode(rs1);
    int rs2Code=getRegisterCode(rs2);
    if (rs1Code<0 || rs2Code<0) 
    return 0;

    int imm11_5=(imm>>5)&0x7F;
    int imm4_0=imm&0x1F;

    return ((imm11_5&0x7F)<<25) |
           ((rs2Code&0x1F)<<20) |
           ((rs1Code&0x1F)<<15) |
           ((funct3&0x7)<<12) |
           ((imm4_0&0x1F)<<7) |
           (opcode&0x7F);
}

// B-type
uint32_t encodeBType(const char*rs1,const char*rs2,int imm,int funct3,int opcode){
    int rs1Code=getRegisterCode(rs1);
    int rs2Code=getRegisterCode(rs2);
    if (rs1Code<0 || rs2Code<0) 
    return 0;

    int imm12=(imm>>12)&1;
    int imm10_5=(imm>>5)&0x3F;
    int imm4_1=(imm>>1)&0xF;
    int imm11=(imm>>11)&1;

    return ((imm12&1)<<31) |
           ((imm10_5&0x3F)<<25) |
           ((rs2Code&0x1F)<<20) |
           ((rs1Code&0x1F)<<15) |
           ((funct3&0x7)<<12) |
           ((imm4_1&0xF)<<8) |
           ((imm11&1)<<7) |
           (opcode&0x7F);
}

// U-type
uint32_t encodeUType(const char*rd,int imm,int opcode) {
    int rdCode=getRegisterCode(rd);
    if (rdCode<0) return 0;

    return ((imm&0xFFFFF)<<12) |
           ((rdCode&0x1F)<<7) |
           (opcode&0x7F);
}

// J-type
uint32_t encodeJType(const char*rd,int imm,int opcode) {
    int rdCode=getRegisterCode(rd);
    if (rdCode<0) return 0;

    int imm20=(imm>>20)&1;
    int imm10_1=(imm>>1)&0x3FF;
    int imm11=(imm>>11)&1;
    int imm19_12=(imm>>12)&0xFF;

    return ((imm20&1)<<31) |
           ((imm19_12&0xFF)<<12) |
           ((imm11&1)<<20) |
           ((imm10_1&0x3FF)<<21) |
           ((rdCode&0x1F)<<7) |
           (opcode&0x7F);
}
