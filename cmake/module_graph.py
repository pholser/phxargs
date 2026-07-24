#!/usr/bin/env python3
"""Emit a graphviz dot graph of #include "..." dependencies in a C source tree."""

import re
import sys
from pathlib import Path

def stem(path):
    return Path(path).stem

def main():
    src_dir = Path(sys.argv[1]) if len(sys.argv) > 1 else Path("src")
    include_re = re.compile(r'#include\s+"([^"]+)"')

    edges = set()
    nodes = set()

    for src in sorted(src_dir.glob("*.c")):
        caller = stem(src)
        nodes.add(caller)
        for header in include_re.findall(src.read_text()):
            callee = stem(header)
            if callee != caller:
                edges.add((caller, callee))
                nodes.add(callee)

    print("digraph modules {")
    print("  rankdir=LR;")
    print('  node [shape=box fontname="Helvetica" fontsize=10];')
    for node in sorted(nodes):
        print(f'  "{node}";')
    for src, dst in sorted(edges):
        print(f'  "{src}" -> "{dst}";')
    print("}")

if __name__ == "__main__":
    main()
