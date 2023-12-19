from . import labels
from . import vars

COMPS = {
    '0': "101010",
    '1': "111111",
    '-1' : "111010",
    'D' : "001100",
    'A' : "110000",
    'M' : "110000",
    '!D' : "001101",
    '!A' : "110001",
    '!M' : "110001",
    '-D': "001111",
    '-A': "110011",
    '-M': "110011",
    'D+1': "011111",
    'A+1': "110111",
    'M+1': "110111",
    'D-1': "001110",
    'A-1': "110010",
    'M-1': "110010",
    'D+A': "000010",
    'D+M': "000010",
    'D-A': "010011",
    'D-M': "010011",
    'A-D': "000111",
    'M-D': "000111",
    'D&A': "000000",
    'D&M': "000000",
    'D|A': "010101",
    'D|M': "010101",
}

def get_comp(comp: str | bool) -> str:
    if type(comp) == bool:
        raise SyntaxError(f"A compare statement is required.")

    bin_comp = COMPS.get(comp)
    if bin_comp is None:
        raise SyntaxError(f"Unknown compare instruction <{comp}>.")

    a = '0'
    if 'M' in comp:
        a = '1'

    return a + bin_comp


def get_jump(jump: str | bool) -> str:
    if type(jump) == bool:
        return "000"

    cond = jump[1:]
    match cond:
        case "GT":
            return "001"
        case "GE":
            return "011"
        case "LT":
            return "100"
        case "LE":
            return "110"
        case "NE":
            return "101"
        case "EQ":
            return "010"
        case "MP":
            return "111"
        case _ as err:
            raise SyntaxError(f"Unknown jump condition <{err}>")


def get_dest(dest: str | bool) -> str:
    if type(dest) == bool:
        return "000"

    A, D, M = ('0', '0', '0')
    for letter in dest:
        match letter:
            case 'M':
                M = '1'
            case 'A':
                A = '1'
            case 'D':
                D = '1'
            case ' ':
                continue
            case _ as err:
                raise SyntaxError(f"Unknown destination <{err}>. ")

    d_code: str = A + D + M
    return d_code


def get_c_inst(inst: str) -> str:
    dest = False
    comp = False
    jump = False
    try:
        comment = inst.index('/')
    except ValueError:
        comment = len(inst)

    inst = inst[:comment].strip()

    # Divide inst in its constituent parts
    if '=' not in inst and ';' not in inst:
        comp = inst
    else:
        if '=' in inst:
            dest = inst.split('=')[0]
            comp = inst.split('=')[1]
            if ';' in inst:
                jump = inst.split(';')[1]
        else:
            if ';' in inst:
                comp, jump = inst.split(';')
            else:
                comp = inst


    bin_inst: str = '111' + get_comp(comp) + get_dest(dest) + get_jump(jump)

    return bin_inst + '\n'

# Expects a trimmed instruction
def get_a_inst(inst: str) -> str:
    """Receives an instruction string, then checks if the symbol is 
        a registered label or variable and replaces it.
        """
    if len(inst) < 2 or inst[0] != '@': # Notice here the short-circuit evaluation
        raise SyntaxError(f"Bad A-Instruction <{inst}>")

    try:
        comment = inst.index('/')
    except ValueError:
        comment = len(inst)

    inst = inst[:comment].strip()


    sym = inst[1:].strip()
    if sym.isdigit():
        var_val = int(sym)
    else:
        # Get label if it exists (priority over values)
        var_val = labels.get_label(sym)
        if var_val == -1:
            # parse_vars always returns a memory location
            var_val = vars.parse_vars(sym)

    bin_inst = bin(var_val)
    assert len(bin_inst) < 16 + 2, f"Limit of value size for <{var_val}> exceeded."

    # TODO Check if changing 15 to 16 works
    return bin_inst.split("b")[1].rjust(16, "0") + '\n'

