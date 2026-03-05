#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "encode.h"
#include "isa.h"
#include "parser.h"

#define MAX_LABELS 100
#define MAX_NAME 64

typedef struct {
    char name[MAX_NAME];
    int address;
} Label;

Label labelTable[MAX_LABELS];
int labelCount = 0;

void addLabel(const char *name, int address)
{
    if (labelCount >= MAX_LABELS)
        return;

    strncpy(labelTable[labelCount].name, name, MAX_NAME - 1);
    labelTable[labelCount].name[MAX_NAME - 1] = '\0';
    labelTable[labelCount].address = address;
    labelCount++;
}

int findLabel(const char *name)
{
    for (int i = 0; i < labelCount; i++) {
        if (strcmp(labelTable[i].name, name) == 0)
            return labelTable[i].address;
    }
    return -1;
}

void writeBinary(uint32_t value, FILE *fout)
{
    for (int i = 31; i >= 0; i--) {
        int bit = (value >> i) & 1;
        fputc(bit ? '1' : '0', fout);
    }
    fputc('\n', fout);
}

int main(int argc, char *argv[])
{
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <input.asm> <output.bin>\n", argv[0]);
        return 1;
    }

    FILE *fin = fopen(argv[1], "r");
    if (fin == NULL) {
        perror("Input file");
        return 1;
    }

    FILE *fout = fopen(argv[2], "w");
    if (fout == NULL) {
        perror("Output file");
        fclose(fin);
        return 1;
    }

    char line[256];
    int lineNo = 0;
    int pc = 0;

    while (fgets(line, sizeof(line), fin)) {

        lineNo++;

        char tokens[MAX_TOKENS][MAX_LINE];
        int count = tokenizeLine(line, tokens);

        if (count == 0)
            continue;

        int len = strlen(tokens[0]);

        if (tokens[0][len - 1] == ':') {

            tokens[0][len - 1] = '\0';
            addLabel(tokens[0], pc);

            if (count == 1)
                continue;
        }

        pc += 4;
    }

    rewind(fin);

    lineNo = 0;
    pc = 0;

    int errorFlag = 0;
    int haltFound = 0;
    int haltLine = -1;

    while (fgets(line, sizeof(line), fin)) {

        lineNo++;

        if (strlen(line) <= 1)
            continue;

        ParsedInstr p = parseInstruction(line, lineNo);

        if (p.errorFlag) {
            fprintf(stderr, "%s\n", p.errorMsg);
            errorFlag = 1;
            continue;
        }

        if (p.type == 3 || p.type == 5) {

            if (isalpha(p.rs2[0])) {

                int addr = findLabel(p.rs2);

                if (addr < 0) {
                    fprintf(stderr, "Line %d: Undefined label\n", lineNo);
                    errorFlag = 1;
                    continue;
                }

                p.imm = addr - pc;
            }
        }

        uint32_t machine = 0;

        switch (p.type) {

            case 0:
                machine = encodeRType(p.rd, p.rs1, p.rs2, p.funct3, p.funct7, p.opcodeVal);
                break;

            case 1:
                machine = encodeIType(p.rd, p.rs1, p.imm, p.funct3, p.opcodeVal);
                break;

            case 2:
                machine = encodeSType(p.rs2, p.rs1, p.imm, p.funct3, p.opcodeVal);
                break;

            case 3:
                machine = encodeBType(p.rs1, p.rs2, p.imm, p.funct3, p.opcodeVal);
                break;

            case 4:
                machine = encodeUType(p.rd, p.imm, p.opcodeVal);
                break;

            case 5:
                machine = encodeJType(p.rd, p.imm, p.opcodeVal);
                break;

            default:
                fprintf(stderr, "Line %d: Unknown instruction type\n", lineNo);
                errorFlag = 1;
                continue;
        }

        writeBinary(machine, fout);

        if (strcmp(p.opcode, "beq") == 0 &&
            strcmp(p.rs1, "zero") == 0 &&
            strcmp(p.rs2, "zero") == 0 &&
            p.imm == 0) {

            haltFound = 1;
            haltLine = lineNo;
        }

        pc += 4;
    }

    fclose(fin);
    fclose(fout);

    if (!haltFound) {
        fprintf(stderr, "Error: Virtual Halt missing\n");
        errorFlag = 1;
    }

    if (haltLine != lineNo) {
        fprintf(stderr, "Error: Virtual Halt must be last instruction\n");
        errorFlag = 1;
    }

    if (errorFlag) {
        printf("Assembly failed with errors\n");
        return 1;
    }

    printf("Assembly completed successfully\n");
    return 0;
}
