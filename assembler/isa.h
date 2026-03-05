#ifndef ISA_H
#define ISA_H

#include <stdio.h>
#include <string.h>
#include <ctype.h>

typedef struct{
    const char *name;
    int code;
} RegisterMap;

static const RegisterMap registerTable[] = {
    {"zero",0},{"x0",0},
    {"ra",1},{"x1",1},
    {"sp",2},{"x2",2},
    {"gp",3},{"x3",3},
    {"tp",4},{"x4",4},
    {"t0",5},{"x5",5},
    {"t1",6},{"x6",6},
    {"t2",7},{"x7",7},
    {"s0",8},{"fp",8},{"x8",8},
    {"s1",9},{"x9",9},
    {"a0",10},{"x10",10},
    {"a1",11},{"x11",11},
    {"a2",12},{"x12",12},
    {"a3",13},{"x13",13},
    {"a4",14},{"x14",14},
    {"a5",15},{"x15",15},
    {"a6",16},{"x16",16},
    {"a7",17},{"x17",17},
    {"s2",18},{"x18",18},
    {"s3",19},{"x19",19},
    {"s4",20},{"x20",20},
    {"s5",21},{"x21",21},
    {"s6",22},{"x22",22},
    {"s7",23},{"x23",23},
    {"s8",24},{"x24",24},
    {"s9",25},{"x25",25},
    {"s10",26},{"x26",26},
    {"s11",27},{"x27",27},
    {"t3",28},{"x28",28},
    {"t4",29},{"x29",29},
    {"t5",30},{"x30",30},
    {"t6",31},{"x31",31}
};

int getRegisterCode(const char *reg){
    if(reg==NULL) return -1;

    char buf[16];
    int len=0;

    while(*reg && len<15){
        if(!isspace((unsigned char)*reg)){
            buf[len++]=tolower((unsigned char)*reg);
        }
        reg++;
    }

    buf[len]='\0';

    int size = sizeof(registerTable)/sizeof(registerTable[0]);

    for(int i=0;i<size;i++){
        if(strcmp(registerTable[i].name,buf)==0){
            return registerTable[i].code;
        }
    }

    return -1;
}

#endif
