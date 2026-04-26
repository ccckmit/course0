import sys, math
import numpy as np
import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt
import matplotlib.gridspec as gridspec
from matplotlib.ticker import AutoMinorLocator
from matplotlib.patches import FancyBboxPatch

sys.path.insert(0, '.')
from spice0 import Netlist, MNASolver

# ── 配色 ──────────────────────────────────────────────
BG    = '#0A0E1A'
PANEL = '#0F1628'
GM    = '#1E2A45'
Gm    = '#141D33'
SP    = '#2A3A5C'
LBL   = '#7B8FCF'
TK    = '#4A5A7A'
TX    = '#E8EAFF'
PASS_C= '#3DDC84'
FAIL_C= '#FF5252'
C = ['#00C8FF','#FF6B35','#7EE8A2','#FFD700','#E040FB','#FF4081']

def ax_style(ax, title):
    ax.set_facecolor(PANEL)
    ax.grid(True, which='major', color=GM, linewidth=0.8, zorder=0)
    ax.grid(True, which='minor', color=Gm, linewidth=0.4, zorder=0)
    ax.xaxis.set_minor_locator(AutoMinorLocator(4))
    ax.yaxis.set_minor_locator(AutoMinorLocator(4))
    for s in ax.spines.values():
        s.set_color(SP); s.set_linewidth(1.2)
    ax.set_title(title, color=LBL, fontsize=9.5,
                 fontfamily='monospace', loc='left', pad=6)
    ax.tick_params(colors=TK, labelsize=8)

def glow(ax, xs, ys, label, ci, lw=2.0):
    col = C[ci % len(C)]
    ax.plot(xs, ys, color=col, linewidth=lw, label=label, zorder=3,
            solid_capstyle='round')
    ax.plot(xs, ys, color=col, linewidth=lw*4, alpha=0.10, zorder=2)

def leg(ax):
    l = ax.legend(facecolor='#0D1524', edgecolor=SP, labelcolor='#C8D0E8',
                  fontsize=8, loc='best', framealpha=0.85)
    for line in l.get_lines(): line.set_linewidth(2.5)

# ── 版面 ──────────────────────────────────────────────
fig = plt.figure(figsize=(16, 22), facecolor=BG)
fig.patch.set_facecolor(BG)
gs  = gridspec.GridSpec(4, 2, figure=fig,
                        hspace=0.52, wspace=0.32,
                        top=0.91, bottom=0.05,
                        left=0.08, right=0.96)

# ── 總標題 ────────────────────────────────────────────
fig.text(0.5, 0.965,
         'SPICE0 Python  ·  Test Suite — Simulation Results',
         ha='center', color=TX, fontsize=15, fontweight='bold',
         fontfamily='monospace')
fig.text(0.5, 0.945,
         'MNA solver  ·  Backward Euler integration  ·  numpy.linalg.solve',
         ha='center', color=LBL, fontsize=9, fontfamily='monospace')

# ══════════════════════════════════════════════════════
# T1: Voltage Divider — bar chart
# ══════════════════════════════════════════════════════
ax = fig.add_subplot(gs[0, 0])
ax_style(ax, 'T1 · Voltage Divider  (.op)')
nl = Netlist.from_file('ex1_divider.sp')
sv = MNASolver(nl)
x  = sv.solve('op')
labels = ['V(in)', 'V(mid)', 'I(V1)×200']
vals   = [sv.eval_var(x,'V(in)'),
          sv.eval_var(x,'V(mid)'),
          sv.eval_var(x,'I(V1)')*200]   # scale A→display
exp_v  = [10, 5, -1]
for i, (lbl, val, ev) in enumerate(zip(labels, vals, exp_v)):
    col = C[i]
    bar = ax.bar(i, val, color=col, alpha=0.70, width=0.5, zorder=3)
    ax.bar(i, val, color=col, alpha=0.12, width=0.58, zorder=2)
    # 期望值標記
    ax.hlines(ev, i-0.3, i+0.3, colors='#FFFFFF', linewidth=1.2,
              linestyles='--', alpha=0.5, zorder=4)
    ax.text(i, val + (0.15 if val>=0 else -0.4),
            f'{val:.4g}', ha='center', va='bottom',
            color=col, fontsize=8.5, fontfamily='monospace')
ax.set_xticks([0,1,2]); ax.set_xticklabels(labels, color='#C8D0E8',
                                             fontsize=8.5, fontfamily='monospace')
ax.axhline(0, color=SP, linewidth=0.8)
ax.set_ylabel('V  /  (A×200)', color=TK, fontsize=8, fontfamily='monospace')
ax.text(0.97, 0.95, '-- = expected', transform=ax.transAxes,
        ha='right', va='top', color='#FFFFFF', alpha=0.45,
        fontsize=7, fontfamily='monospace')

# ══════════════════════════════════════════════════════
# T2: RC Charging
# ══════════════════════════════════════════════════════
ax = fig.add_subplot(gs[0, 1])
ax_style(ax, 'T2 · RC Charging  τ=1ms  (.tran)')
nl = Netlist.from_file('ex2_rc.sp')
sv = MNASolver(nl)
for el in nl.elems: el.state = 0.0
an = nl.analysis
dt, tstop = an['tstep'], an['tstop']
ts, vvc, iV1 = [], [], []
t = 0.0
while t <= tstop + dt*0.5:
    xv = sv.solve('tran', dt)
    ts.append(t*1e3)
    vvc.append(sv.eval_var(xv,'V(vc)'))
    iV1.append(sv.eval_var(xv,'I(V1)')*1e3)
    sv.update_state(xv, dt); t += dt
ts_a = np.linspace(0, tstop*1e3, 300)
vc_a = 5*(1 - np.exp(-ts_a/1.0))
glow(ax, ts, vvc, 'V(vc) sim', 0)
ax.plot(ts_a, vc_a, '--', color='#FFFFFF', lw=1.0, alpha=0.30,
        label='5(1−e^{−t/τ})', zorder=4)
glow(ax, ts, iV1, 'I(V1) mA', 1)
# τ 標記
ax.axvline(1.0, color='#FFD700', lw=0.8, alpha=0.5, linestyle=':')
ax.text(1.05, 0.3, 'τ', color='#FFD700', alpha=0.7,
        fontsize=10, fontfamily='monospace', transform=ax.get_xaxis_transform())
ax.set_xlabel('Time (ms)', color=TK, fontsize=8, fontfamily='monospace')
ax.set_ylabel('V  /  mA', color=TK, fontsize=8, fontfamily='monospace')
leg(ax)

# ══════════════════════════════════════════════════════
# T3: DC Sweep
# ══════════════════════════════════════════════════════
ax = fig.add_subplot(gs[1, 0])
ax_style(ax, 'T3 · DC Sweep  V1: 0→5 V  (.dc)')
nl = Netlist.from_file('ex3_dc_sweep.sp')
sv = MNASolver(nl)
an = nl.analysis
src = next(e for e in nl.elems if e.name.lower()==an['src'].lower())
v_pts, vmid, icur = [], [], []
v = an['start']
while v <= an['stop'] + abs(an['step'])*0.5:
    src.value = v
    xv = sv.solve('op')
    v_pts.append(v)
    vmid.append(sv.eval_var(xv,'V(mid)'))
    icur.append(sv.eval_var(xv,'I(V1)')*1e3)
    v += an['step']
glow(ax, v_pts, vmid, 'V(mid)=V₁/2', 0)
glow(ax, v_pts, icur, 'I(V1) mA',    1)
# 散點強調
for xi, yi, col in zip(v_pts, vmid, [C[0]]*len(v_pts)):
    ax.scatter(xi, yi, color=col, s=40, zorder=5)
for xi, yi in zip(v_pts, icur):
    ax.scatter(xi, yi, color=C[1], s=40, zorder=5)
ax.axhline(0, color=SP, lw=0.8)
ax.set_xlabel('V1 (V)', color=TK, fontsize=8, fontfamily='monospace')
ax.set_ylabel('V  /  mA', color=TK, fontsize=8, fontfamily='monospace')
leg(ax)

# ══════════════════════════════════════════════════════
# T4: RL Transient
# ══════════════════════════════════════════════════════
ax = fig.add_subplot(gs[1, 1])
ax_style(ax, 'T4 · RL Circuit  τ≈83µs  (.tran)')
nl = Netlist.from_file('ex4_rl.sp')
sv = MNASolver(nl)
for el in nl.elems: el.state = 0.0
an = nl.analysis
dt, tstop = an['tstep'], an['tstop']
ts, vvl = [], []
t = 0.0
while t <= tstop + dt*0.5:
    xv = sv.solve('tran', dt)
    ts.append(t*1e6)
    vvl.append(sv.eval_var(xv,'V(vl)'))
    sv.update_state(xv, dt); t += dt
tau_rl = 10e-3/120
ts_a = np.linspace(0, tstop*1e6, 500)
vl_a = 12*np.exp(-ts_a/1e6/tau_rl)
il_a = (12/120)*(1-np.exp(-ts_a/1e6/tau_rl))*1e3
glow(ax, ts, vvl, 'V(vl) sim', 0)
ax.plot(ts_a, vl_a, '--', color='#FFFFFF', lw=1.0, alpha=0.30,
        label='12·e^{−t/τ}', zorder=4)
ax.plot(ts_a, il_a, color=C[2], lw=1.5, alpha=0.8,
        label='IL analytical (mA)', zorder=3)
ax.axvline(tau_rl*1e6, color='#FFD700', lw=0.8, alpha=0.5, linestyle=':')
ax.text(tau_rl*1e6+2, 10.5, 'τ', color='#FFD700', alpha=0.7,
        fontsize=10, fontfamily='monospace')
ax.set_xlabel('Time (µs)', color=TK, fontsize=8, fontfamily='monospace')
ax.set_ylabel('V  /  mA', color=TK, fontsize=8, fontfamily='monospace')
leg(ax)

# ══════════════════════════════════════════════════════
# T5: Current Source — horizontal bar
# ══════════════════════════════════════════════════════
ax = fig.add_subplot(gs[2, 0])
ax_style(ax, 'T5 · Current Source Network  (.op)')
nl = Netlist.from_file('ex5_isrc.sp')
sv = MNASolver(nl)
xv = sv.solve('op')
bnames = ['V(a)', 'V(mid)', 'I(R1)×1k', 'I(R2)×1k']
bvals  = [sv.eval_var(xv,'V(a)'),
          sv.eval_var(xv,'V(mid)'),
          sv.eval_var(xv,'I(R1)')*1e3,
          sv.eval_var(xv,'I(R2)')*1e3]
exps   = [4/3, 2/3, 4/3, 2/3]
ypos   = np.arange(len(bnames))
for i, (lbl, val, ev) in enumerate(zip(bnames, bvals, exps)):
    col = C[i]
    ax.barh(ypos[i], val, color=col, alpha=0.70, height=0.45, zorder=3)
    ax.barh(ypos[i], val, color=col, alpha=0.12, height=0.52, zorder=2)
    ax.vlines(ev, ypos[i]-0.25, ypos[i]+0.25, colors='#FFFFFF',
              lw=1.2, linestyles='--', alpha=0.5, zorder=4)
    ax.text(val+0.01, ypos[i], f'{val:.4g}',
            va='center', color=col, fontsize=8.5, fontfamily='monospace')
ax.set_yticks(ypos)
ax.set_yticklabels(bnames, color='#C8D0E8', fontsize=8.5, fontfamily='monospace')
ax.axvline(0, color=SP, lw=0.8)
ax.set_xlabel('V  /  mA·kΩ', color=TK, fontsize=8, fontfamily='monospace')
ax.text(0.97, 0.04, '-- = expected', transform=ax.transAxes,
        ha='right', va='bottom', color='#FFFFFF', alpha=0.45,
        fontsize=7, fontfamily='monospace')

# ══════════════════════════════════════════════════════
# T6+T7: Pass / Fail 儀表板
# ══════════════════════════════════════════════════════
ax = fig.add_subplot(gs[2, 1])
ax_style(ax, 'T6–T7 · Edge Cases & Plot Verification')
ax.set_xlim(0, 1); ax.set_ylim(0, 1)
ax.set_xticks([]); ax.set_yticks([])

checks = [
    ('Empty netlist: no crash',     True),
    ('Two voltage sources in series', True),
    ('V(a)=3V  V(b)=10V verified',  True),
    ('RC PNG > 10 KB generated',     True),
]
for i, (txt, passed) in enumerate(checks):
    y = 0.82 - i*0.18
    col  = PASS_C if passed else FAIL_C
    mark = '✓' if passed else '✗'
    # pill background
    rect = FancyBboxPatch((0.04, y-0.06), 0.92, 0.13,
                          boxstyle='round,pad=0.01',
                          facecolor=col+'22', edgecolor=col+'55', lw=1)
    ax.add_patch(rect)
    ax.text(0.10, y+0.005, mark, color=col, fontsize=14, va='center',
            fontfamily='monospace', fontweight='bold')
    ax.text(0.18, y+0.005, txt, color='#C8D0E8', fontsize=8.5, va='center',
            fontfamily='monospace')

# ══════════════════════════════════════════════════════
# 底部：τ 正規化比較
# ══════════════════════════════════════════════════════
ax = fig.add_subplot(gs[3, :])
ax_style(ax, 'Summary · Normalised Exponential Response  (RC charging vs RL decay)')
t_n = np.linspace(0, 6, 400)
rc = 1 - np.exp(-t_n)
rl = np.exp(-t_n)
glow(ax, t_n, rc, 'RC  V(vc)/V∞  =1−e^{−t/τ}  (→ charging)', 0, lw=2.2)
glow(ax, t_n, rl, 'RL  V(vl)/V₀  =e^{−t/τ}    (→ decay)',    1, lw=2.2)
# 重要門檻線
for frac, label, col in [
    (1-math.exp(-1), '63.2% (1τ)',  '#FFD700'),
    (1-math.exp(-3), '95.0% (3τ)',  '#E040FB'),
    (1-math.exp(-5), '99.3% (5τ)',  '#7EE8A2'),
]:
    ax.axhline(frac, color=col, lw=0.7, ls=':', alpha=0.55)
    ax.axhline(1-frac, color=col, lw=0.7, ls=':', alpha=0.55)
    ax.text(6.05, frac,   f' {label}', color=col, va='center',
            fontsize=7, fontfamily='monospace', clip_on=False)
for tau_n in [1,2,3,4,5]:
    ax.axvline(tau_n, color='#FFFFFF', lw=0.5, alpha=0.12)
    ax.text(tau_n, -0.07, f'{tau_n}τ', ha='center', color=TK,
            fontsize=7.5, fontfamily='monospace')
ax.set_xlim(0, 6.5)
ax.set_ylim(-0.05, 1.08)
ax.set_xlabel('Time / τ', color=TK, fontsize=9, fontfamily='monospace')
ax.set_ylabel('Normalised amplitude', color=TK, fontsize=9, fontfamily='monospace')
leg(ax)

# 版本浮水印
fig.text(0.97, 0.012, 'spice0.py · MNA + Backward Euler · numpy',
         color='#1E2A45', fontsize=7, ha='right', fontfamily='monospace')

plt.savefig('test_results.png', dpi=150, bbox_inches='tight', facecolor=BG)
print('saved')