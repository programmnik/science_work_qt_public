#!/usr/bin/env python3
"""Строит include-граф с раскраской узлов по слоям."""
import re
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
SRC  = ROOT / "src"
INC_RE = re.compile(r'^\s*#include\s+"([^"]+)"')

# Цвета по слоям
LAYER_COLORS = {
    "Core":     "#b3d9ff",   # голубой
    "Data":     "#b3ffb3",   # зелёный
    "OpenGL":   "#ffb3b3",   # красный (там нарушение)
    "UI":       "#ffe6b3",   # оранжевый
    "Panels":   "#e6b3ff",   # фиолетовый
    "App":      "#ffffb3",   # жёлтый
    "Themes":   "#d9d9d9",   # серый
}

def layer_of(path):
    parts = path.parts
    if "Core" in parts:   return "Core"
    if "Data" in parts:   return "Data"
    if "OpenGL" in parts: return "OpenGL"
    if "UI" in parts:     return "UI"
    if "Panels" in parts: return "Panels"
    if "App" in parts:    return "App"
    if "Themes" in parts: return "Themes"
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

    with open(ROOT / "include_graph_colored.dot", "w") as out:
        out.write("digraph includes {\n")
        out.write("  rankdir=LR;\n")
        out.write("  splines=true;\n")
        out.write("  node [shape=box, style=filled, fontsize=10];\n")
        out.write("  edge [fontsize=8];\n")

        # Узлы с цветом по слою
        for f in files:
            rel = f.relative_to(ROOT)
            layer = layer_of(f)
            color = LAYER_COLORS.get(layer, "#ffffff")
            out.write(f'  "{rel}" [fillcolor="{color}"];\n')

        # Рёбра
        for a, b in edges:
            rel_a = a.relative_to(ROOT)
            rel_b = b.relative_to(ROOT)
            la = layer_of(a)
            lb = layer_of(b)
            # Нарушения слоёв — красные жирные стрелки
            is_violation = (
                la in ("Core", "Data", "OpenGL") and
                lb in ("UI", "Panels", "App")
            )
            style = 'color="red", penwidth=2.0' if is_violation else ''
            out.write(f'  "{rel_a}" -> "{rel_b}" [{style}];\n')

        out.write("}\n")

    print(f"Файлов: {len(files)}, рёбер: {len(edges)}")
    print(f"Нарушений слоёв: ", end="")
    violations = sum(1 for a, b in edges
                     if layer_of(a) in ("Core", "Data", "OpenGL")
                     and layer_of(b) in ("UI", "Panels", "App"))
    print(violations)

if __name__ == "__main__":
    main()
