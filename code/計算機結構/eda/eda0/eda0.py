import sys
import os
import re
import sympy
from sympy.logic.boolalg import simplify_logic
import matplotlib.pyplot as plt
import matplotlib.patches as patches
import random

# 設定 matplotlib 不使用 GUI 後端，避免在無桌面環境下報錯
plt.switch_backend('Agg')

class SimpleEDA:
    def __init__(self, verilog_file):
        self.filename = verilog_file
        self.basename = os.path.splitext(os.path.basename(verilog_file))[0]
        self.out_dir = "out"
        self.img_dir = os.path.join(self.out_dir, "images")
        
        os.makedirs(self.out_dir, exist_ok=True)
        os.makedirs(self.img_dir, exist_ok=True)
        
        self.md_content = f"# EDA Flow Report for `{self.basename}.v`\n\n"
        self.equations = {}
        self.simplified_equations = {}
        self.mapped_gates = []

    def run_flow(self):
        print(f"--- Processing {self.filename} ---")
        self.rtl_to_logic()
        self.tech_independent_synthesis()
        self.tech_dependent_synthesis()
        self.placement()
        self.global_routing()
        self.detailed_routing()
        self.save_markdown()
        print(f"Done. Output saved to {self.out_dir}/{self.basename}.md\n")

    # 1. RTL to Logic Synthesis (Parsing Verilog)
    def rtl_to_logic(self):
        self.md_content += "## 1. RTL to Logic Synthesis\n"
        self.md_content += "Extracting boolean equations from Verilog:\n```verilog\n"
        
        with open(self.filename, 'r') as f:
            content = f.read()
            
        # 簡單的正則表達式尋找 assign 語句: assign y = a & b;
        assigns = re.findall(r'assign\s+([a-zA-Z0-9_]+)\s*=\s*(.+?);', content)
        
        for out_var, expr in assigns:
            self.equations[out_var] = expr
            self.md_content += f"{out_var} = {expr}\n"
        self.md_content += "```\n\n"

    # 2. Technology Independent Synthesis (Logic Simplification)
    def tech_independent_synthesis(self):
        self.md_content += "## 2. Technology Independent Synthesis\n"
        self.md_content += "Simplifying boolean equations (using Quine-McCluskey / Sympy):\n```text\n"
        
        for out_var, expr in self.equations.items():
            # 尋找所有變數並宣告為 Sympy Symbols
            vars_in_expr = set(re.findall(r'[a-zA-Z]+', expr))
            syms = {v: sympy.Symbol(v) for v in vars_in_expr}
            
            try:
                # 解析並化簡
                parsed_expr = eval(expr, {"__builtins__": None}, syms)
                simplified = simplify_logic(parsed_expr)
                self.simplified_equations[out_var] = str(simplified)
                self.md_content += f"{out_var} = {simplified}\n"
            except Exception as e:
                self.md_content += f"Error simplifying {out_var}: {e}\n"
                self.simplified_equations[out_var] = expr
        self.md_content += "```\n\n"

    # 3. Technology Dependent Synthesis (Tech Mapping)
    def tech_dependent_synthesis(self):
        self.md_content += "## 3. Technology Dependent Synthesis\n"
        self.md_content += "Mapping to Standard Cell Library (AND, OR, NOT, XOR):\n```text\n"
        
        for out_var, expr in self.simplified_equations.items():
            # 簡單模擬：計算運算子數量來決定需要幾個 Cell
            gates = []
            if '~' in expr: gates.append("INV")
            if '&' in expr: gates.append("AND2")
            if '|' in expr: gates.append("OR2")
            if '^' in expr: gates.append("XOR2")
            if not gates: gates.append("BUF") # 直接輸出
            
            self.mapped_gates.extend(gates)
            self.md_content += f"Net '{out_var}' requires cells: {', '.join(gates)}\n"
            
        self.md_content += "```\n\n"

    # --- 共用的繪圖函數 ---
    def _draw_layout_base(self, ax, stage):
        ax.set_xlim(0, 10)
        ax.set_ylim(0, 5)
        ax.axis('off')
        
        # 畫 Row (電源線/地線軌道)
        for i in range(1, 5):
            ax.axhline(y=i, color='salmon', linewidth=4, alpha=0.5)
            ax.axhline(y=i-0.8, color='salmon', linewidth=4, alpha=0.5)

        # 隨機擺置 Cells (模擬 Placement)
        random.seed(sum([ord(c) for c in self.basename])) # 確保每次同檔案畫出來一樣
        cell_positions = []
        for i, gate in enumerate(self.mapped_gates + ["IN", "IN", "OUT"]):
            x = random.uniform(1, 8)
            y = random.choice([0.2, 1.2, 2.2, 3.2])
            width, height = 0.8, 0.8
            rect = patches.Rectangle((x, y), width, height, linewidth=1, edgecolor='steelblue', facecolor='aliceblue')
            ax.add_patch(rect)
            ax.text(x+width/2, y+height/2, gate, ha='center', va='center', fontsize=6)
            cell_positions.append((x+width/2, y+height/2))
            
        return cell_positions

    # 4. Placement
    def placement(self):
        fig, ax = plt.subplots(figsize=(6, 3))
        self._draw_layout_base(ax, "Placement")
        plt.title("Placement (Standard Cells on Rows)")
        
        img_path = os.path.join(self.img_dir, f"{self.basename}_placement.png")
        plt.savefig(img_path, bbox_inches='tight', dpi=150)
        plt.close()
        
        self.md_content += "## 4. Placement\n"
        self.md_content += "Placing standard cells onto chip rows.\n\n"
        self.md_content += f"![Placement](images/{self.basename}_placement.png)\n\n"

    # 5. Global Routing
    def global_routing(self):
        fig, ax = plt.subplots(figsize=(6, 3))
        pos = self._draw_layout_base(ax, "Global")
        
        # 模擬 Global Routing (畫出粗略的 Manhattan 連線與網格)
        for i in range(len(pos)-1):
            x1, y1 = pos[i]
            x2, y2 = pos[i+1]
            # 畫曼哈頓直線 (先X後Y)
            ax.plot([x1, x2], [y1, y1], color='green', linewidth=1, alpha=0.5)
            ax.plot([x2, x2], [y1, y2], color='green', linewidth=1, alpha=0.5)
            
        plt.title("Global Routing (Coarse Grid Allocation)")
        img_path = os.path.join(self.img_dir, f"{self.basename}_global.png")
        plt.savefig(img_path, bbox_inches='tight', dpi=150)
        plt.close()
        
        self.md_content += "## 5. Global Routing\n"
        self.md_content += "Allocating routing resources and determining coarse paths.\n\n"
        self.md_content += f"![Global Routing](images/{self.basename}_global.png)\n\n"

    # 6. Detailed Routing
    def detailed_routing(self):
        fig, ax = plt.subplots(figsize=(6, 3))
        pos = self._draw_layout_base(ax, "Detailed")
        
        # 模擬 Detailed Routing (加入 via 節點，區分金屬層顏色)
        for i in range(len(pos)-1):
            x1, y1 = pos[i]
            x2, y2 = pos[i+1]
            
            # M1 (水平 - 綠色), M2 (垂直 - 黃色), Via (黑點)
            ax.plot([x1, x2], [y1, y1], color='darkgreen', linewidth=2) # M1
            ax.plot([x2, x2], [y1, y2], color='goldenrod', linewidth=2) # M2
            ax.plot(x1, y1, 'ks', markersize=3) # Pin
            ax.plot(x2, y1, 'ko', markersize=3) # Via
            ax.plot(x2, y2, 'ks', markersize=3) # Pin

        plt.title("Detailed Routing (Exact Metal Shapes and Vias)")
        img_path = os.path.join(self.img_dir, f"{self.basename}_detailed.png")
        plt.savefig(img_path, bbox_inches='tight', dpi=150)
        plt.close()
        
        self.md_content += "## 6. Detailed Routing\n"
        self.md_content += "Drawing exact metal traces, vias, and pin connections.\n\n"
        self.md_content += f"![Detailed Routing](images/{self.basename}_detailed.png)\n\n"

    def save_markdown(self):
        with open(os.path.join(self.out_dir, f"{self.basename}.md"), "w") as f:
            f.write(self.md_content)

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage: python eda0.py <file.v>")
        sys.exit(1)
        
    eda = SimpleEDA(sys.argv[1])
    eda.run_flow()