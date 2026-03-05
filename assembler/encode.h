#ifndef ENCODE_H
#define ENCODE_H
#include <stdint.h>

uint32_t encodeRType(const char *rd,const char*rs1,const char*rs2,int funct3,int funct7,int opcode);
uint32_t encodeIType(const char*rd,const char*rs1,int imm,int funct3,int opcode);
uint32_t encodeSType(const char*rs2,const char*rs1,int imm,int funct3,int opcode);
uint32_t encodeBType(const char*rs1,const char*rs2,int imm,int funct3,int opcode);
uint32_t encodeUType(const char*rd,int imm,int opcode);
uint32_t encodeJType(const char*rd,int imm,int opcode);
#endif
