# The global counter
VAR_COUNT = 16

# Some predefs
VARS = {
        'R0': 0, 'R1': 1, 'R2': 2, 
        'R3': 3, 'R4': 4, 'R5': 5, 
        'R6': 6, 'R7': 7, 'R8': 8, 
        'R9': 9, 'R10': 10, 'R11': 11, 
        'R12': 12, 'R13': 13, 'R14': 14, 
        'R15': 15, "SCREEN": 16384, "KDB": 24576,
        "SP": 0, "LCL": 1, "ARG": 2,
        "THIS": 3, "THAT": 4
}

def check(f):
    assert VAR_COUNT < 32767, "Maximum amount of variables allocated."
    # TODO Add some asserts
    return f
    

@check
def parse_vars(variable: str) -> int:
    """Accepts a variable's name as input.
        Then checks if the variable name is already 
        registered, if so, returns the variable's value.
        """
    global VAR_COUNT
    global VARS
    if variable in VARS: # If variable exists, return its value
        return VARS[variable]

    VARS[variable] = VAR_COUNT
    VAR_COUNT += 1
    return VARS[variable]

