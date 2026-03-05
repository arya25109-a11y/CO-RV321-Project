#include"isa.h"
#include"encode.h"
#include<stdio.h>
#include<string.h>
#include<stdint.h>
int getRegisterCode(const char *reg){
    extern const RegisterMap registerTable[];
    extern const int REGISTER_COUNT;
    for (int i =0;i<REGISTER_COUNT;i++){
        if (strcmp(registerTable[i].name,reg)==0){
            return registerTable[i].code;
        }
    }
    return -1;
   //R TYPE//
uint32_t encodeRType(const char *rd, const char *rs1, const char *rs2,
                     int funct3,int funct7,int opcode){
    int rdCode  =getRegisterCode(rd);
    int rs1Code =getRegisterCode(rs1);
    int rs2Code =getRegisterCode(rs2);
    if (rdCode<0 || rs1Code<0 || rs2Code<0)
        return 0;
    uint32_t instruction =0;
    instruction |=(funct7  &0x7F)<<25;
    instruction |=(rs2Code &0x1F)<<20;
    instruction |=(rs1Code &0x1F)<<15;
    instruction |=(funct3  &0x7)<<12;
    instruction |=(rdCode  &0x1F)<<7;
    instruction |=(opcode  &0x7F);
    return instruction;
}
//I TYPE //
uint32_t encodeIType(const char *rd, const char *rs1,int imm,
                     int funct3,int opcode){
    if (imm<-2048 || imm> 2047)
        return 0;
    int rdCode  =getRegisterCode(rd);
    int rs1Code =getRegisterCode(rs1);
    if (rdCode<0 || rs1Code<0)
        return 0;
    uint32_t instruction =0;
    instruction |=(imm &0xFFF)<<20;
    instruction |=(rs1Code &0x1F)<<15;
    instruction |=(funct3 &0x7)<<12;
    instruction |=(rdCode &0x1F)<<7;
    instruction |=(opcode &0x7F);
    return instruction;
}
// S TYPE//
uint32_t encodeSType(const char *rs2, const char *rs1,int imm,
                     int funct3,int opcode){

    if (imm<-2048 || imm>2047)
        return 0;
    int rs1Code =getRegisterCode(rs1);
    int rs2Code=getRegisterCode(rs2);
    if (rs1Code<0 ||rs2Code<0)
        return 0;
    int imm_high =(imm >>5)&0x7F;
    int imm_low  =imm &0x1F;
    uint32_t instruction =0;
    instruction |=(imm_high &0x7F)<<25;
    instruction |=(rs2Code &0x1F)<<20;
    instruction |=(rs1Code &0x1F)<<15;
    instruction|=(funct3 &0x7)<<12;
    instruction |=(imm_low &0x1F)<<7;
    instruction |=(opcode &0x7F);
    return instruction;
}
//B TYPE//
uint32_t encodeBType(const char *rs1,const char *rs2,int imm,
                     int funct3,int opcode){
    int rs1Code =getRegisterCode(rs1);
    int rs2Code =getRegisterCode(rs2);
    if (rs1Code<0 || rs2Code<0)
        return 0;
    int imm12 =(imm >>12)&1;
    int imm10_5 =(imm >>5)&0x3F;
    int imm4_1 =(imm >>1)&0xF;
    int imm11 =(imm >>11)&1;
    uint32_t instruction =0;
    instruction |=(imm12<<31);
    instruction |=(imm10_5<<25);
    instruction |=(rs2Code<<20);
    instruction |=(rs1Code<<15);
    instruction |=(funct3<<12);
    instruction |=(imm4_1<<8);
    instruction |=(imm11<<7);
    instruction|=opcode;
    return instruction;
}
//  U TYPE //
uint32_t encodeUType(const char *rd,int imm,int opcode){
    int rdCode =getRegisterCode(rd);
    if (rdCode<0)
        return 0;
    return ((imm &0xFFFFF)<<12)|
           ((rdCode &0x1F)<<7)|
           (opcode &0x7F);
}
// J TYPE //
uint32_t encodeJType(const char *rd,int imm,int opcode){
    int rdCode =getRegisterCode(rd);
    if (rdCode<0)
        return 0;
    int imm20  =(imm >>20)&1;
    int imm10_1 =(imm >>1)&0x3FF;
    int imm11 =(imm >>11)&1;
    int imm19_12 =(imm >>12)&0xFF;
    uint32_t instruction =0;
    instruction |=imm20<<31;
    instruction |=imm10_1<<21;
    instruction |=imm11<<20;
    instruction |=imm19_12<<12;
    instruction |=rdCode<<7;
    instruction |=opcode;
    return instruction;
}                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                             
~                  
