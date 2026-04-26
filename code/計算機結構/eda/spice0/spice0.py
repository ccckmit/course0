#!/usr/bin/env python3
"""
spice0.py — 簡易 SPICE 電路模擬器 (Python 版)

支援元件：
  R  — 電阻 (Resistor)
  V  — 獨立電壓源 (Voltage source)
  I  — 獨立電流源 (Current source)
  C  — 電容 (Capacitor，暫態)
  L  — 電感 (Inductor，暫態)

分析類型：
  .op          — DC 工作點
  .tran        — 暫態分析
  .dc          — DC 掃描

輸出：
  .print       — 指定輸出變數
  結果自動繪圖並存 PNG

解法：改良節點分析法 (MNA) + numpy 線性求解
"""

import sys
import re
import math
import numpy as np
import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt
import matplotlib.gridspec as gridspec
from matplotlib.ticker import AutoMinorLocator
from pathlib import Path

# ─────────────────────── 工程記號 ────────────────────────

SUFFIXES = {
    'f': 1e-15, 'p': 1e-12, 'n': 1e-9,
    'u': 1e-6,  'm': 1e-3,  'k': 1e3,
    'g': 1e9,   't': 1e12,
}

def parse_value(s: str) -> float:
    s = s.strip()
    # meg
    if s.lower().endswith('meg'):
        return float(s[:-3]) * 1e6
    m = re.match(r'^([+-]?[\d.eE+-]+)([a-zA-Z]?)$', s)
    if not m:
        raise ValueError(f"Cannot parse value: {s!r}")
    num, suf = m.groups()
    return float(num) * SUFFIXES.get(suf.lower(), 1.0)

# ─────────────────────── 資料結構 ────────────────────────

class Elem:
    def __init__(self, name, n1, n2, value):
        self.name  = name
        self.etype = name[0].upper()   # R V I C L
        self.n1    = n1    # 正節點編號 (0=GND)
        self.n2    = n2    # 負節點編號
        self.value = value
        self.state = 0.0   # C: 電壓; L: 電流
        self.vsrc_idx = -1 # 電壓源在 x 的額外索引

class Netlist:
    def __init__(self):
        self.title    = '(untitled)'
        self.nodes    = {}   # name → idx (1-based; 0=GND)
        self.elems    : list[Elem] = []
        self.analysis = None   # dict
        self.prints   : list[str] = []

    # ── 節點 ──────────────────────────────────────────
    def node_idx(self, name: str) -> int:
        lo = name.lower()
        if lo in ('0', 'gnd'): return 0
        if name not in self.nodes:
            self.nodes[name] = len(self.nodes) + 1
        return self.nodes[name]

    def node_name(self, idx: int) -> str:
        if idx == 0: return '0'
        for k, v in self.nodes.items():
            if v == idx: return k
        return '?'

    # ── Netlist 解析 ───────────────────────────────────
    @classmethod
    def from_file(cls, path: str) -> 'Netlist':
        nl = cls()
        lines = Path(path).read_text(encoding='utf-8', errors='replace').splitlines()
        first = True
        for raw in lines:
            line = raw.strip()
            if not line or line.startswith('*'):
                continue
            if first:
                nl.title = line; first = False; continue

            lo = line.lower()
            if lo.startswith('.end'):
                break

            # .tran tstep tstop
            if lo.startswith('.tran'):
                parts = line.split()
                nl.analysis = {'type': 'tran',
                               'tstep': parse_value(parts[1]),
                               'tstop': parse_value(parts[2])}
                continue

            # .op
            if lo.startswith('.op'):
                nl.analysis = {'type': 'op'}
                continue

            # .dc src start stop step
            if lo.startswith('.dc'):
                parts = line.split()
                nl.analysis = {'type': 'dc',
                               'src':   parts[1],
                               'start': parse_value(parts[2]),
                               'stop':  parse_value(parts[3]),
                               'step':  parse_value(parts[4])}
                continue

            # .print V(x) I(y) ...
            if lo.startswith('.print'):
                nl.prints += line.split()[1:]
                continue

            # 元件行
            parts = line.split()
            if len(parts) < 4:
                continue
            name, sn1, sn2, sval = parts[0], parts[1], parts[2], parts[3]
            if name[0].upper() not in 'RVICL':
                print(f"[WARN] Unknown element ignored: {name}")
                continue
            el = Elem(name, nl.node_idx(sn1), nl.node_idx(sn2),
                      parse_value(sval))
            nl.elems.append(el)

        if nl.analysis is None:
            nl.analysis = {'type': 'op'}
        return nl

# ─────────────────────── MNA 求解器 ──────────────────────

GMIN = 1e-12   # 防奇異最小電導

class MNASolver:
    def __init__(self, nl: Netlist):
        self.nl = nl

    def _count_vsrc(self, mode: str) -> int:
        """計算 MNA 額外電壓源變數數量"""
        n = 0
        for el in self.nl.elems:
            if el.etype == 'V': n += 1
            if el.etype == 'L' and mode == 'op': n += 1
        return n

    def _build(self, mode: str, dt: float = 0.0):
        """組建 MNA 矩陣 A, b；回傳 (A, b, N, vsrc_map)"""
        nn = len(self.nl.nodes)        # 節點數（不含 GND）
        nv = self._count_vsrc(mode)
        N  = nn + nv
        A  = np.zeros((N, N))
        b  = np.zeros(N)

        # GMIN 注入
        for i in range(nn):
            A[i, i] += GMIN

        vsrc_row = nn   # 額外電壓源從這行開始
        for el in self.nl.elems:
            el.vsrc_idx = -1
            a = el.n1 - 1   # matrix index (-1 → GND, 不填)
            c = el.n2 - 1

            if el.etype == 'R':
                g = 1.0 / el.value
                if a >= 0: A[a, a] += g
                if c >= 0: A[c, c] += g
                if a >= 0 and c >= 0:
                    A[a, c] -= g; A[c, a] -= g

            elif el.etype == 'V':
                vr = vsrc_row; vsrc_row += 1
                el.vsrc_idx = vr
                if a >= 0: A[vr, a] += 1; A[a, vr] += 1
                if c >= 0: A[vr, c] -= 1; A[c, vr] -= 1
                b[vr] += el.value

            elif el.etype == 'I':
                # I(n1,n2,val)：電流從外部 n1→n2，KCL: n1流出(-)，n2流入(+)
                if a >= 0: b[a] -= el.value
                if c >= 0: b[c] += el.value

            elif el.etype == 'C':
                if mode == 'op':
                    pass   # 電容開路
                else:
                    # 向後歐拉伴隨：Geq=C/dt，電流源 Ieq=Geq*Vc_prev (流入n1)
                    geq = el.value / dt
                    ieq = geq * el.state
                    if a >= 0: A[a, a] += geq; b[a] += ieq
                    if c >= 0: A[c, c] += geq; b[c] -= ieq
                    if a >= 0 and c >= 0:
                        A[a, c] -= geq; A[c, a] -= geq

            elif el.etype == 'L':
                if mode == 'op':
                    # 電感短路
                    vr = vsrc_row; vsrc_row += 1
                    el.vsrc_idx = vr
                    if a >= 0: A[vr, a] += 1; A[a, vr] += 1
                    if c >= 0: A[vr, c] -= 1; A[c, vr] -= 1
                    # b[vr]=0 → 短路
                else:
                    # 向後歐拉伴隨：Geq=dt/L，Ieq=IL_prev (流出n1)
                    geq = dt / el.value
                    ieq = el.state
                    if a >= 0: A[a, a] += geq; b[a] -= ieq
                    if c >= 0: A[c, c] += geq; b[c] += ieq
                    if a >= 0 and c >= 0:
                        A[a, c] -= geq; A[c, a] -= geq

        return A, b, N

    def solve(self, mode: str, dt: float = 0.0) -> np.ndarray:
        A, b, N = self._build(mode, dt)
        try:
            return np.linalg.solve(A, b)
        except np.linalg.LinAlgError:
            print("[WARN] Singular matrix — returning zeros")
            return np.zeros(N)

    def update_state(self, x: np.ndarray, dt: float):
        for el in self.nl.elems:
            a = el.n1 - 1; c = el.n2 - 1
            va = x[a] if a >= 0 else 0.0
            vc = x[c] if c >= 0 else 0.0
            if el.etype == 'C':
                el.state = va - vc
            elif el.etype == 'L':
                el.state = el.state + (va - vc) * (dt / el.value)

    def _node_v(self, x, name):
        lo = name.lower()
        if lo in ('0', 'gnd'): return 0.0
        idx = self.nl.nodes.get(name)
        if idx is None: return float('nan')
        return x[idx - 1]

    def _elem_i(self, x, ename):
        for el in self.nl.elems:
            if el.name.lower() == ename.lower():
                if el.vsrc_idx >= 0:
                    return x[el.vsrc_idx]
                if el.etype == 'R':
                    a = el.n1 - 1; c = el.n2 - 1
                    va = x[a] if a >= 0 else 0.0
                    vc = x[c] if c >= 0 else 0.0
                    return (va - vc) / el.value
                return float('nan')
        return float('nan')

    def eval_var(self, x, var: str) -> float:
        """解析 V(node) 或 I(elem)"""
        m = re.match(r'^[Vv]\((.+)\)$', var)
        if m: return self._node_v(x, m.group(1))
        m = re.match(r'^[Ii]\((.+)\)$', var)
        if m: return self._elem_i(x, m.group(1))
        return float('nan')

# ─────────────────────── 分析執行 ────────────────────────

def fmt(v):
    """科學記號格式化"""
    if math.isnan(v): return 'N/A'
    return f'{v:>14.6g}'

def run_op(nl: Netlist):
    print(f"\n{'═'*52}")
    print(f"  DC Operating Point")
    print(f"{'═'*52}")
    solver = MNASolver(nl)
    x = solver.solve('op')

    results = {}
    if nl.prints:
        header = f"{'':>14}" + ''.join(f'{v:>14}' for v in nl.prints)
        print(header)
        print('-' * len(header))
        row = f"{'0':>14}" + ''.join(fmt(solver.eval_var(x, v)) for v in nl.prints)
        print(row)
        for v in nl.prints:
            results[v] = [solver.eval_var(x, v)]
    else:
        for name, idx in nl.nodes.items():
            val = x[idx - 1]
            print(f"  V({name}) = {val:14.6g} V")
            results[f'V({name})'] = [val]
        for el in nl.elems:
            if el.vsrc_idx >= 0:
                val = x[el.vsrc_idx]
                print(f"  I({el.name}) = {val:14.6g} A")
                results[f'I({el.name})'] = [val]

    return {'type': 'op', 'x_data': [0.0], 'results': results,
            'xlabel': '', 'title': 'DC Operating Point'}

def run_tran(nl: Netlist):
    an = nl.analysis
    tstep, tstop = an['tstep'], an['tstop']
    print(f"\n{'═'*52}")
    print(f"  Transient Analysis   step={tstep:.3g}  stop={tstop:.3g}")
    print(f"{'═'*52}")

    solver = MNASolver(nl)
    # 零初始條件
    for el in nl.elems: el.state = 0.0

    pvars = nl.prints or []
    t_data = []
    results = {v: [] for v in pvars}

    if pvars:
        header = f"{'time':>14}" + ''.join(f'{v:>14}' for v in pvars)
        print(header)
        print('-' * len(header))

    t = 0.0
    while t <= tstop + tstep * 0.5:
        x = solver.solve('tran', tstep)
        t_data.append(t)
        if pvars:
            vals = [solver.eval_var(x, v) for v in pvars]
            for v, val in zip(pvars, vals): results[v].append(val)
            row = f"{t:>14.6g}" + ''.join(fmt(val) for val in vals)
            print(row)
        solver.update_state(x, tstep)
        t += tstep

    return {'type': 'tran', 'x_data': t_data, 'results': results,
            'xlabel': 'Time (s)', 'title': 'Transient Analysis'}

def run_dc(nl: Netlist):
    an = nl.analysis
    src_name = an['src']
    v_start, v_stop, v_step = an['start'], an['stop'], an['step']
    print(f"\n{'═'*52}")
    print(f"  DC Sweep   {src_name}: {v_start:.4g} → {v_stop:.4g}  step={v_step:.4g}")
    print(f"{'═'*52}")

    src = next((e for e in nl.elems if e.name.lower() == src_name.lower()), None)
    if src is None:
        print(f"[ERROR] Source not found: {src_name}"); return {}

    solver = MNASolver(nl)
    pvars = nl.prints or []
    v_data = []
    results = {v: [] for v in pvars}

    if pvars:
        header = f"{src_name:>14}" + ''.join(f'{v:>14}' for v in pvars)
        print(header)
        print('-' * len(header))

    v = v_start
    while v <= v_stop + abs(v_step) * 0.5:
        src.value = v
        x = solver.solve('op')
        v_data.append(v)
        if pvars:
            vals = [solver.eval_var(x, pv) for pv in pvars]
            for pv, val in zip(pvars, vals): results[pv].append(val)
            row = f"{v:>14.6g}" + ''.join(fmt(val) for val in vals)
            print(row)
        v += v_step

    return {'type': 'dc', 'x_data': v_data, 'results': results,
            'xlabel': f'{src_name} (V)', 'title': f'DC Sweep — {src_name}'}

# ─────────────────────── 繪圖 ────────────────────────────

# 電路主題調色盤
COLORS = [
    '#00C8FF',  # 電容藍
    '#FF6B35',  # 電感橘
    '#7EE8A2',  # 電阻綠
    '#FFD700',  # 電壓黃
    '#E040FB',  # 電流紫
    '#FF4081',  # 粉紅
    '#40C4FF',  # 淺藍
    '#69F0AE',  # 薄荷
]

def smart_unit(var: str):
    """根據變數名稱判斷單位與刻度"""
    if var.startswith('V(') or var.startswith('v('):
        return 'V', 1.0, 'Voltage (V)'
    if var.startswith('I(') or var.startswith('i('):
        return 'A', 1.0, 'Current (A)'
    return '', 1.0, var

def eng_fmt_x(vals):
    """自動決定 x 軸工程記號縮放"""
    mx = max(abs(v) for v in vals) if vals else 1
    if   mx < 1e-9:  return 1e12, 'ps'
    elif mx < 1e-6:  return 1e9,  'ns'
    elif mx < 1e-3:  return 1e6,  'µs'
    elif mx < 1.0:   return 1e3,  'ms'
    else:            return 1.0,  's'

def plot_results(data: dict, out_path: str, netlist_title: str):
    if not data or not data.get('results'):
        print("[INFO] No .print variables — skip plot")
        return

    results = data['results']
    x_data  = data['x_data']
    pvars   = [v for v, d in results.items() if d]
    if not pvars:
        return

    # ── 分組：電壓 vs 電流 ────────────────────────────
    v_vars = [v for v in pvars if v[0].upper() == 'V']
    i_vars = [v for v in pvars if v[0].upper() == 'I']
    groups = []
    if v_vars: groups.append(('Voltage', v_vars, 'V'))
    if i_vars: groups.append(('Current', i_vars, 'A'))
    # 其餘
    other = [v for v in pvars if v not in v_vars and v not in i_vars]
    if other: groups.append(('Other', other, ''))

    n_plots = len(groups)

    # ── 圖表設定 ──────────────────────────────────────
    fig = plt.figure(figsize=(12, 4 * n_plots + 1.5),
                     facecolor='#0A0E1A')
    fig.patch.set_facecolor('#0A0E1A')

    gs = gridspec.GridSpec(n_plots, 1, figure=fig,
                           hspace=0.45,
                           top=0.88, bottom=0.10,
                           left=0.10, right=0.95)

    # 標題
    an_type = data['type'].upper()
    fig.suptitle(
        f"SPICE0  ·  {netlist_title}\n{an_type} Analysis",
        color='#E8EAFF', fontsize=14, fontweight='bold',
        fontfamily='monospace', y=0.97
    )

    # x 軸縮放
    if data['type'] == 'tran' and x_data:
        x_scale, x_unit = eng_fmt_x(x_data)
        x_label = f"Time ({x_unit})"
    elif data['type'] == 'dc':
        x_scale, x_unit = 1.0, ''
        x_label = data['xlabel']
    else:
        x_scale, x_unit = 1.0, ''
        x_label = data['xlabel']

    xs = [v * x_scale for v in x_data]

    for gi, (gname, gvars, yunit) in enumerate(groups):
        ax = fig.add_subplot(gs[gi])
        ax.set_facecolor('#0F1628')

        # 格線
        ax.grid(True, which='major', color='#1E2A45', linewidth=0.8, zorder=0)
        ax.grid(True, which='minor', color='#141D33', linewidth=0.4, zorder=0)
        ax.xaxis.set_minor_locator(AutoMinorLocator(4))
        ax.yaxis.set_minor_locator(AutoMinorLocator(4))

        # 邊框
        for spine in ax.spines.values():
            spine.set_color('#2A3A5C')
            spine.set_linewidth(1.2)

        # 繪製各變數
        for ci, var in enumerate(gvars):
            ys = results[var]
            col = COLORS[ci % len(COLORS)]
            # 主線
            ax.plot(xs, ys, color=col, linewidth=2.0,
                    label=var, zorder=3, solid_capstyle='round')
            # 光暈
            ax.plot(xs, ys, color=col, linewidth=6.0,
                    alpha=0.12, zorder=2)
            # OP 只有一個點時畫散點
            if len(xs) == 1:
                ax.scatter(xs, ys, color=col, s=80, zorder=5)

        # 標題、標籤
        ax.set_title(gname, color='#7B8FCF', fontsize=10,
                     fontfamily='monospace', loc='left', pad=6)
        if gi == n_plots - 1:
            ax.set_xlabel(x_label, color='#5A6A9A', fontsize=9,
                          fontfamily='monospace')
        ax.set_ylabel(yunit, color='#5A6A9A', fontsize=9,
                      fontfamily='monospace')
        ax.tick_params(colors='#4A5A7A', labelsize=8)

        # 圖例
        leg = ax.legend(facecolor='#0D1524', edgecolor='#2A3A5C',
                        labelcolor='#C8D0E8', fontsize=8.5,
                        loc='upper right', framealpha=0.85)
        for line in leg.get_lines():
            line.set_linewidth(2.5)

        # y=0 基準線
        ylim = ax.get_ylim()
        if ylim[0] < 0 < ylim[1]:
            ax.axhline(0, color='#3A4A6A', linewidth=0.8,
                       linestyle='--', zorder=1)

    plt.savefig(out_path, dpi=150, bbox_inches='tight',
                facecolor='#0A0E1A')
    plt.close()
    print(f"\n[PLOT] Saved → {out_path}")

# ─────────────────────── main ────────────────────────────

def main():
    if len(sys.argv) < 2:
        print("Usage: python3 spice0.py <netlist.sp> [output.png]")
        sys.exit(1)

    netlist_path = sys.argv[1]
    out_png = sys.argv[2] if len(sys.argv) > 2 else \
              Path(netlist_path).stem + '_result.png'

    nl = Netlist.from_file(netlist_path)

    print(f"SPICE0 (Python)  —  {nl.title}")
    print(f"Nodes: {len(nl.nodes)}   Elements: {len(nl.elems)}")

    an = nl.analysis
    if an['type'] == 'op':
        data = run_op(nl)
    elif an['type'] == 'tran':
        data = run_tran(nl)
    elif an['type'] == 'dc':
        data = run_dc(nl)
    else:
        print(f"[ERROR] Unknown analysis type: {an['type']}")
        sys.exit(1)

    plot_results(data, out_png, nl.title)

if __name__ == '__main__':
    main()
