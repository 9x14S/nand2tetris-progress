GOT = {}


def get_label(label_str: str) -> int:
    """Checks whether a given label exists in the GOT.
       Returns a value > -1 if it was found.
       Returns -1 if it wasn't found."""
    return GOT.get(label_str, -1)

# Both adds and gets (LABELS) from the GOT
def parse_label(label: str, line_num: int=-100) -> int:
    """Accepts an oligatory label string, and optionally the line number,
        which also works as a switch to return the value if it exists or to 
        set it.
        Returns -1 if the label was added.
        Returns int > -1 if it exists"""
    if len(label) < 3: # Checks both parenthesis and at least a character
        raise SyntaxError(f"Malformed label <{label}>")

    label = label.strip("()")
    label_val = GOT.get(label) 
    if label_val is None:
        GOT[label] = line_num
        return -1 

    return label_val

