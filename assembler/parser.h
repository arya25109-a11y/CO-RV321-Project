#ifndef PARSER_H
#define PARSER_H
#define MAX_LINE 256
#define MAX_TOKENS 5
typedef struct{
    char opcode[10];
    char rd[10];
    char rs1[10];
    char rs2[10];
    int imm;
    int type;
    int funct3;
    int funct7;
    int opcodeVal;
    int lineNumber;
    int errorFlag;
    char errorMsg[128];
}ParsedInstr;
int tokenizeLine(const char *line, char tokens[MAX_TOKENS][MAX_LINE]);
#endif
