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
