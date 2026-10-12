#!/usr/bin/env python3
"""Include-граф с группировкой по слоям."""
import re
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
SRC  = ROOT / "src"
INC_RE = re.compile(r'^\s*#include\s+"([^"]+)"')

LAYERS = ["Core", "Data", "OpenGL", "UI", "Panels", "App", "Themes"]

LAYER_STYLE = {
    "Core":     ("#dbeafe", "#1e40af"),
    "Data":     ("#dcfce7", "#166534"),
    "OpenGL":   ("#fee2e2", "#991b1b"),
    "UI":       ("#ffedd5", "#9a3412"),
    "Panels":   ("#ede9fe", "#5b21b6"),
    "App":      ("#fef9c3", "#854d0e"),
    "Themes":   ("#f3f4f6", "#374151"),
}

def layer_of(path):
    parts = path.parts
    for L in LAYERS:
        if L in parts:
            return L
    return "Other"

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
                edges.append((f, t))

    with open(ROOT / "include_graph_clustered.dot", "w") as out:
        out.write("digraph includes {\n")
        out.write("  rankdir=TB;\n")
        out.write("  compound=true;\n")
        out.write("  node [shape=box, style=filled, fontsize=9];\n")

        # Кластеры по слоям
        for layer in LAYERS:
            layer_files = [f for f in files if layer_of(f) == layer]
            if not layer_files:
                continue
            bg, border = LAYER_STYLE[layer]
            out.write(f'  subgraph cluster_{layer} {{\n')
            out.write(f'    label="{layer}";\n')
            out.write(f'    style="filled,rounded";\n')
            out.write(f'    fillcolor="{bg}";\n')
            out.write(f'    color="{border}";\n')
            for f in layer_files:
                rel = f.relative_to(ROOT)
                out.write(f'    "{rel}";\n')
            out.write("  }\n")

        # Рёбра
        for a, b in edges:
            rel_a = a.relative_to(ROOT)
            rel_b = b.relative_to(ROOT)
            la, lb = layer_of(a), layer_of(b)
            if la == lb:
                out.write(f'  "{rel_a}" -> "{rel_b}" [color="#888888"];\n')
            elif la in ("Core", "Data", "OpenGL") and lb in ("UI", "Panels", "App"):
                # Нарушение
                out.write(f'  "{rel_a}" -> "{rel_b}" [color="red", penwidth=2.5];\n')
            else:
                out.write(f'  "{rel_a}" -> "{rel_b}" [color="#1f77b4"];\n')

        out.write("}\n")

    print(f"Файлов: {len(files)}, рёбер: {len(edges)}")

if __name__ == "__main__":
    main()
