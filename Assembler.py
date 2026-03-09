import sys
import re
REGISTERS={
    'zero':0,'x0':0,
    'ra':1,'x1':1,
    'sp':2,'x2':2,
    'gp':3,'x3':3,
    'tp':4,'x4':4,
    't0':5,'x5':5,
    't1':6,'x6':6,
    't2':7,'x7':7,
    's0':8,'fp':8,'x8':8,
    's1':9,'x9':9,
    'a0':10,'x10':10,
    'a1':11,'x11':11,
    'a2':12,'x12':12,
    'a3':13,'x13':13,
    'a4':14,'x14':14,
    'a5':15,'x15':15,
    'a6':16,'x16':16,
    'a7':17,'x17':17,
    's2':18,'x18':18,
    's3':19,'x19':19,
    's4':20,'x20':20,
    's5':21,'x21':21,
    's6':22,'x22':22,
    's7':23,'x23':23,
    's8':24,'x24':24,
    's9':25,'x25':25,
    's10':26,'x26':26,
    's11':27,'x27':27,
    't3':28,'x28':28,
    't4':29,'x29':29,
    't5':30,'x30':30,
    't6':31,'x31':31,
}
R_TYPE={
    'add':('0110011','000','0000000'),
    'sub':('0110011','000','0100000'),
    'sll':('0110011','001','0000000'),
    'slt':('0110011','010','0000000'),
    'sltu':('0110011','011','0000000'),
    'xor':('0110011','100','0000000'),
    'srl':('0110011','101','0000000'),
    'or':('0110011','110','0000000'),
    'and':('0110011','111','0000000'),
}
I_TYPE_ALU={
    'addi':('0010011','000'),
    'sltiu':('0010011','011'),
}
I_TYPE_LOAD={
    'lw':('0000011','010'),
}
S_TYPE={
    'sw':('0100011','010'),
}
B_TYPE={
    'beq':('1100011','000'),
    'bne':('1100011','001'),
    'blt':('1100011','100'),
    'bge':('1100011','101'),
    'bltu':('1100011','110'),
    'bgeu':('1100011','111'),
}
U_TYPE={
    'lui':'0110111',
    'auipc':'0010111',
}

#helping functions
def to_binary(value:int,bits:int) -> str:
    mask=(1<<bits)-1
    return format(value & mask,f'0{bits}b')

def check_range(val:int,bits:int,signed:bool,line_num:int,field:str="Immediate"):
    lo=-(1<<(bits-1)) if signed else 0
    hi=(1<<(bits-1))-1 if signed else (1<<bits)-1
    if not (lo <= val <=hi):
        raise ValueError(
            f"Line {line_num}:{field} value {val} out of range "
            f"[{lo},{hi}] for {bits}-bit {'signed' if signed else 'unsigned'} field"
        )
#parsing helping functions
def get_register(name:str,line_num:int) -> int:
    n=name.strip().lower()
    if n in REGISTERS:
        return REGISTERS[n]
    raise ValueError(f"Line {line_num}:Unknown register '{name.strip()}'")

def parse_imm(token:str,line_num:int) -> int:
    t=token.strip()
    try:
        if t.startswith('0x') or t.startswith('0X'):
            return int(t,16)
        if t.startswith('0b') or t.startswith('0B'):
            return int(t,2)
        return int(t)
    except ValueError:
        raise ValueError(f"Line {line_num}:Cannot parse immediate '{t}'")


def resolve_labelORimm(token:str,labels:dict,pc:int,line_num:int) -> int:
    t=token.strip()
    if t in labels:
        return labels[t] *4-pc   
    if t and t[0].isalpha():
        raise ValueError(f"Line {line_num}:Undefined label '{t}'")
    return parse_imm(t,line_num)

#encodeer
def enc_r(rd,rs1,rs2,opcode,f3,f7):
    return f7+to_binary(rs2,5)+to_binary(rs1,5)+f3+to_binary(rd,5)+opcode

def enc_i(rd,rs1,imm,opcode,f3):
    return to_binary(imm,12)+to_binary(rs1,5)+f3+to_binary(rd,5)+opcode


def enc_s(rs1,rs2,imm,opcode,f3):
    ib=to_binary(imm,12)
    return ib[0:7]+to_binary(rs2,5)+to_binary(rs1,5)+f3+ib[7:12]+opcode


def enc_b(rs1,rs2,imm,opcode,f3):
    ib=to_binary(imm,13)
    return ib[0]+ib[2:8]+to_binary(rs2,5)+to_binary(rs1,5)+f3+ib[8:12]+ib[1]+opcode


def enc_u(rd,imm,opcode):
    return to_binary(imm,20)+to_binary(rd,5)+opcode


def enc_j(rd,imm,opcode):
    ib=to_binary(imm,21)
    return ib[0]+ib[10:20]+ib[9]+ib[1:9]+to_binary(rd,5)+opcode

#first line parsing
def parse_line(raw:str):
    line=raw.strip()
    if not line:
        return None,None,None

    label=None
    colon=line.find(':')
    if colon>0:
        candidate=line[:colon]
        if candidate and candidate[0].isalpha() and ' ' not in candidate and '\t' not in candidate:
            label=candidate
            line=line[colon+1:].strip()

    if not line:
        return label,None,None
    return label,line,None
#instruction encoding aka second passing
MEM_PATTERN=re.compile(r'^([^\s,]+)\s*,\s*(-?(?:0x[\da-fA-F]+|0b[01]+|\d+))\s*\(\s*([^\s)]+)\s*\)$',re.I)

def encode_instruction(text:str,pc:int,labels:dict,line_num:int) -> str:
    parts=text.split(None,1)
    if not parts:
        raise ValueError(f"Line {line_num}:Empty instruction")
    op=parts[0].lower()
    rest=parts[1].strip() if len(parts)>1 else ''

    #R type
    if op in R_TYPE:
        ops=[x.strip() for x in rest.split(',')]
        if len(ops) != 3:
            raise ValueError(f"Line {line_num}:'{op}' expects rd,rs1,rs2")
        rd =get_register(ops[0],line_num)
        rs1=get_register(ops[1],line_num)
        rs2=get_register(ops[2],line_num)
        opcode,f3,f7=R_TYPE[op]
        return enc_r(rd,rs1,rs2,opcode,f3,f7)

    #I type ALU (addi,sltiu)
    if op in I_TYPE_ALU:
        ops=[x.strip() for x in rest.split(',')]
        if len(ops) != 3:
            raise ValueError(f"Line {line_num}:'{op}' expects rd,rs1,imm")
        rd =get_register(ops[0],line_num)
        rs1=get_register(ops[1],line_num)
        imm=parse_imm(ops[2],line_num)
        check_range(imm,12,True,line_num)
        opcode,f3=I_TYPE_ALU[op]
        return enc_i(rd,rs1,imm,opcode,f3)

    #Load lw rd,imm(rs1)
    if op in I_TYPE_LOAD:
        m=MEM_PATTERN.match(rest)
        if not m:
            raise ValueError(f"Line {line_num}:'{op}' expects 'rd,imm(rs1)'")
        rd =get_register(m.group(1),line_num)
        imm=parse_imm(m.group(2),line_num)
        rs1=get_register(m.group(3),line_num)
        check_range(imm,12,True,line_num)
        opcode,f3=I_TYPE_LOAD[op]
        return enc_i(rd,rs1,imm,opcode,f3)

    #jalr
    if op == 'jalr':
        m=MEM_PATTERN.match(rest)
        if m:
            rd =get_register(m.group(1),line_num)
            imm=parse_imm(m.group(2),line_num)
            rs1=get_register(m.group(3),line_num)
        else:
            ops=[x.strip() for x in rest.split(',')]
            if len(ops) != 3:
                raise ValueError(f"Line {line_num}:'jalr' expects 'rd,rs1,imm' or 'rd,imm(rs1)'")
            rd =get_register(ops[0],line_num)
            rs1=get_register(ops[1],line_num)
            imm=parse_imm(ops[2],line_num)
        check_range(imm,12,True,line_num)
        return enc_i(rd,rs1,imm,'1100111','000')
    #Store
    if op in S_TYPE:
        m=MEM_PATTERN.match(rest)
        if not m:
            raise ValueError(f"Line {line_num}:'{op}' expects 'rs2,imm(rs1)'")
        rs2=get_register(m.group(1),line_num)
        imm=parse_imm(m.group(2),line_num)
        rs1=get_register(m.group(3),line_num)
        check_range(imm,12,True,line_num)
        opcode,f3=S_TYPE[op]
        return enc_s(rs1,rs2,imm,opcode,f3)

    #B type
    if op in B_TYPE:
        ops=[x.strip() for x in rest.split(',')]
        if len(ops)!= 3:
            raise ValueError(f"Line {line_num}:'{op}' expects rs1,rs2,label/imm")
        rs1=get_register(ops[0],line_num)
        rs2=get_register(ops[1],line_num)
        imm=resolve_labelORimm(ops[2],labels,pc,line_num)
        check_range(imm,13,True,line_num)
        if imm % 2!= 0:
            raise ValueError(f"Line {line_num}:Branch offset must be 2-byte aligned")
        opcode,f3=B_TYPE[op]
        return enc_b(rs1,rs2,imm,opcode,f3)

    #U type
    if op in U_TYPE:
        ops=[x.strip() for x in rest.split(',')]
        if len(ops)!= 2:
            raise ValueError(f"Line {line_num}:'{op}' expects rd,imm[31:12]")
        rd =get_register(ops[0],line_num)
        imm=parse_imm(ops[1],line_num)
        check_range(imm,20,True,line_num)
        return enc_u(rd,imm,U_TYPE[op])

    #J type
    if op == 'jal':
        ops=[x.strip() for x in rest.split(',')]
        if len(ops)!= 2:
            raise ValueError(f"Line {line_num}:'jal' expects rd,label/imm")
        rd =get_register(ops[0],line_num)
        imm=resolve_labelORimm(ops[1],labels,pc,line_num)
        check_range(imm,21,True,line_num)
        if imm%2!= 0:
            raise ValueError(f"Line {line_num}:Jump offset must be 2-byte aligned")
        return enc_j(rd,imm,'1101111')
    raise ValueError(f"Line {line_num}:Unknown instruction '{op}'")
