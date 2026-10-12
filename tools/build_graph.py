#!/usr/bin/env python3
"""Строит граф include-зависимостей проекта."""
import re
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
SRC  = ROOT / "src"
INC_RE = re.compile(r'^\s*#include\s+"([^"]+)"')

def collect_files():
    out = []
    for ext in ("*.h", "*.hpp", "*.cpp"):
        out.extend(SRC.rglob(ext))
    return [p for p in out if "build" not in p.parts]

def resolve(src_file, inc):
    inc_path = Path(inc)
    cand = (src_file.parent / inc_path).resolve()
    if cand.exists() and SRC in cand.parents:
        return cand
    for m in SRC.rglob(inc_path.name):
        if SRC in m.parents:
            return m.resolve()
    return None

def main():
    files = collect_files()
    edges = []
    for f in files:
        try:
            text = f.read_text(encoding="utf-8", errors="ignore")
        except OSError:
            continue
        for line in text.splitlines():
            m = INC_RE.match(line)
            if not m:
                continue
            t = resolve(f, m.group(1))
            if t and t != f:
                edges.append((f.relative_to(ROOT), t.relative_to(ROOT)))

    with open(ROOT / "include_edges.tsv", "w") as out:
        for a, b in edges:
            out.write(f"{a}\t{b}\n")

    with open(ROOT / "include_graph.dot", "w") as out:
        out.write("digraph includes {\n  rankdir=LR;\n")
        out.write("  node [shape=box, fontsize=9];\n")
        for a, b in edges:
            out.write(f'  "{a}" -> "{b}";\n')
        out.write("}\n")

    print(f"Файлов: {len(files)}, рёбер: {len(edges)}")

if __name__ == "__main__":
    main()
