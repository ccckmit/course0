"""
貝氏網路經典範例：草地灑水器問題 (Sprinkler Problem)
由 Judea Pearl 提出

網路結構：
    下雨 (Rain)
    ├──→ 灑水器 (Sprinkler)
    └──→ 草地濕潤 (WetGrass)
灑水器 ──→ 草地濕潤

使用函式庫：pgmpy
"""

from pgmpy.models import DiscreteBayesianNetwork
from pgmpy.factors.discrete import TabularCPD
from pgmpy.inference import VariableElimination

# ─────────────────────────────────────────
# 1. 定義網路結構（有向邊）
# ─────────────────────────────────────────
model = DiscreteBayesianNetwork([
    ("Rain",      "Sprinkler"),
    ("Rain",      "WetGrass"),
    ("Sprinkler", "WetGrass"),
])

# ─────────────────────────────────────────
# 2. 定義條件機率表（CPT）
#    變數值：0 = False（否），1 = True（是）
# ─────────────────────────────────────────

# P(Rain)  —— 先驗機率，無父節點
cpd_rain = TabularCPD(
    variable="Rain",
    variable_card=2,
    values=[[0.8],   # P(Rain=F)
            [0.2]],  # P(Rain=T)
    state_names={"Rain": ["F", "T"]},
)

# P(Sprinkler | Rain)
cpd_sprinkler = TabularCPD(
    variable="Sprinkler",
    variable_card=2,
    values=[
        #  Rain=F  Rain=T
        [   0.60,   0.99],  # P(Sprinkler=F | Rain)
        [   0.40,   0.01],  # P(Sprinkler=T | Rain)
    ],
    evidence=["Rain"],
    evidence_card=[2],
    state_names={"Sprinkler": ["F", "T"], "Rain": ["F", "T"]},
)

# P(WetGrass | Sprinkler, Rain)
cpd_wetgrass = TabularCPD(
    variable="WetGrass",
    variable_card=2,
    values=[
        # Sprinkler=F   Sprinkler=T
        # Rain=F Rain=T Rain=F Rain=T
        [  1.00,  0.20,  0.10,  0.01],  # P(WetGrass=F | ...)
        [  0.00,  0.80,  0.90,  0.99],  # P(WetGrass=T | ...)
    ],
    evidence=["Sprinkler", "Rain"],
    evidence_card=[2, 2],
    state_names={
        "WetGrass":  ["F", "T"],
        "Sprinkler": ["F", "T"],
        "Rain":      ["F", "T"],
    },
)

# 將 CPT 加入模型
model.add_cpds(cpd_rain, cpd_sprinkler, cpd_wetgrass)

# 驗證模型是否合法
assert model.check_model(), "模型驗證失敗！"
print("✅ 模型建立成功，結構驗證通過\n")

# ─────────────────────────────────────────
# 3. 建立推論引擎（Variable Elimination）
# ─────────────────────────────────────────
infer = VariableElimination(model)

# ─────────────────────────────────────────
# 4. 推論查詢
# ─────────────────────────────────────────

def print_section(title):
    print("=" * 50)
    print(f"  {title}")
    print("=" * 50)

# ── 查詢 1：先驗機率（無觀測）──
print_section("先驗機率（無任何觀測）")
result = infer.query(["Rain"], show_progress=False)
print(result)
print()

# ── 查詢 2：正向推論 ── 已知晴天，草地濕潤的機率
print_section("正向推論：P(WetGrass | Rain=F)")
result = infer.query(["WetGrass"], evidence={"Rain": "F"}, show_progress=False)
print(result)
print()

# ── 查詢 3：反向推論 ── 已知草地濕潤，下雨的機率
print_section("反向推論：P(Rain | WetGrass=T)")
result = infer.query(["Rain"], evidence={"WetGrass": "T"}, show_progress=False)
print(result)
print()

# ── 查詢 4：反向推論 ── 已知草濕，灑水器啟動的機率
print_section("反向推論：P(Sprinkler | WetGrass=T)")
result = infer.query(["Sprinkler"], evidence={"WetGrass": "T"}, show_progress=False)
print(result)
print()

# ── 查詢 5：解釋消除（Explaining Away）──
# 已知草濕且確認有下雨 → 灑水器的機率下降
print_section("解釋消除（Explaining Away）")
print("情境：草地濕潤，且確認有下雨")
print("問：灑水器啟動的機率？\n")
result = infer.query(
    ["Sprinkler"],
    evidence={"WetGrass": "T", "Rain": "T"},
    show_progress=False,
)
print(result)
print("\n📌 注意：相比只知道草濕（≈35%），")
print("   確認下雨後灑水器機率降至更低 ——")
print("   因為下雨已足以「解釋」草濕，不再需要灑水器。\n")

# ── 查詢 6：MAP 推論 ── 最可能的狀態組合
print_section("MAP 推論：最可能的世界狀態（無觀測）")
result = infer.map_query(
    variables=["Rain", "Sprinkler", "WetGrass"],
    show_progress=False,
)
print(f"  Rain      = {result['Rain']}")
print(f"  Sprinkler = {result['Sprinkler']}")
print(f"  WetGrass  = {result['WetGrass']}")
print()

# ── 查詢 7：已知草濕，最可能的原因
print_section("MAP 推論：草地濕潤時，最可能的原因")
result = infer.map_query(
    variables=["Rain", "Sprinkler"],
    evidence={"WetGrass": "T"},
    show_progress=False,
)
print(f"  Rain      = {result['Rain']}")
print(f"  Sprinkler = {result['Sprinkler']}\n")

# ─────────────────────────────────────────
# 5. 列印完整 CPT（供參考）
# ─────────────────────────────────────────
print_section("完整條件機率表（CPT）")
for cpd in model.cpds:
    print(cpd)
    print()

# ─────────────────────────────────────────
# 6. 網路結構資訊
# ─────────────────────────────────────────
print_section("網路結構資訊")
print(f"  節點：{model.nodes()}")
print(f"  有向邊：{model.edges()}")
print()
print("  Markov 毯（Markov Blanket）：")
for node in model.nodes():
    print(f"    {node} 的 Markov 毯：{model.get_markov_blanket(node)}")