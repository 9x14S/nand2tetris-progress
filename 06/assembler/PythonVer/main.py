#! /usr/bin/python3
"""
This program is a Hack machine language assembler for the Nand2Tetris course.
Made by 9x14S. 

This file is the one to run. 
"""

from sys import argv, stderr
from os import path

from lib.insts import get_a_inst, get_c_inst
from lib.labels import parse_label, GOT

def first_pass(file):
    line_count = 0
    for lnum, line in enumerate(file):
        work_line = line.strip()
        if len(work_line) >= 2:
            match work_line[0]:
                case '(':
                    parse_label(work_line, line_count)
                case '@':
                    line_count += 1
                case '0':
                    line_count += 1
                case 'D'|'M'|'A':
                    line_count += 1
                case _:
                    if work_line[0] == '/' and work_line[1] == '/':
                        continue
                    else:
                        raise SyntaxError(f"Bad instruction {work_line} at line {lnum+1}.")
    file.seek(0)
    return


def second_pass(inf, outf):
    buffer = []
    for line in inf:
        work_line = line.strip()
        if len(work_line) >= 2:
            match work_line[0]:
                case '('|'/': 
                    continue
                case '@':
                    buffer.append(get_a_inst(work_line))
                case 'D'|'M'|'A'|'0':
                    buffer.append(get_c_inst(work_line))
                case _:
                    raise SyntaxError(f"Unknown instruction <\"{work_line}\">. Aborting.")

    outf.write(''.join(buffer))

    return


def main():
    if len(argv) < 2:
        print("Bad usage. Provide at least one file as argument.",
              file=stderr)
        exit(-1)

    file_path = argv[1]
    out_path = path.basename(file_path).split('.')[0] + ".hack"
    with open(file_path, "r") as f, open(out_path, "w") as o:
        first_pass(f)
        second_pass(f, o)
    print("Done!")

if __name__ == "__main__":
    main()
