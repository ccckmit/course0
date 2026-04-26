#!/usr/bin/env python3
"""
qd0vm.py — QD0 IR Virtual Machine
用法: python qd0vm.py <file.qd>
"""

import sys
import re

# ─────────────────────────────────────────────
# 詞法分析：將一行解析成 (op, arg1, arg2, result)
# ─────────────────────────────────────────────

def parse_value(s: str):
    """將字串 token 轉換成 Python 值。"""
    if s == '_': return None
    if s in ('True', 'true'): return True
    if s in ('False', 'false'): return False
    if s in ('None', 'null'): return None
    
    # 處理字串字面值 (支援 \n 等轉義字元，並完美保護中文字)
    if (s.startswith("'") and s.endswith("'")) or \
       (s.startswith('"') and s.endswith('"')):
        val = s[1:-1]
        try:
            # 完美的 UTF-8 轉義還原法：
            # 1. 取得原始 byte 序列 (包含 \n 和中文字的 UTF-8 bytes)
            # 2. unicode_escape: 解析 \n，但此時中文 bytes 會被當作 Latin-1
            # 3. 重新 encode 為 latin-1: 還原出乾淨的 UTF-8 bytes
            # 4. 重新 decode 為 utf-8: 還原成正確的中文
            return val.encode('utf-8').decode('unicode_escape').encode('latin-1').decode('utf-8')
        except Exception:
            # 萬一發生異常，手動替換常見跳脫字元作為最終保護
            return val.replace('\\n', '\n').replace('\\t', '\t').replace('\\r', '\r')
            
    try: return int(s)
    except ValueError: pass
    try: return float(s)
    except ValueError: pass
    
    return s  # 識別符（變數名 / 標籤名）

def tokenize_line(line: str):
    """去掉注釋，切出 token list。支援帶引號的字串中有空白。"""
    in_q = False
    q_char = ''
    clean = []
    i = 0
    while i < len(line):
        c = line[i]
        if in_q:
            if c == q_char:
                in_q = False
            clean.append(c)
        else:
            if c in ('"', "'"):
                in_q = True
                q_char = c
                clean.append(c)
            elif c == ';':
                break  # 注釋開始
            else:
                clean.append(c)
        i += 1
    line = ''.join(clean).strip()
    if not line:
        return []

    tokens = []
    pat = r"""'[^']*'|"[^"]*"|\S+"""
    for m in re.finditer(pat, line):
        tokens.append(m.group())
    return tokens

def parse_line(line: str):
    """回傳 (op, arg1_raw, arg2_raw, result_raw) 或 None。"""
    line = line.strip()
    if not line or line.startswith(';'):
        return None

    # 標籤定義： L_foo::
    if re.match(r'^[A-Za-z_]\w*::$', line):
        label = line[:-2]
        return ('LABEL', label, None, None)

    tokens = tokenize_line(line)
    if not tokens:
        return None

    while len(tokens) < 4:
        tokens.append('_')

    op = tokens[0].upper()
    arg1 = tokens[1]
    arg2 = tokens[2]
    result = tokens[3]
    return (op, arg1, arg2, result)

# ─────────────────────────────────────────────
# 一次性預處理：解析整個檔案
# ─────────────────────────────────────────────

class Instruction:
    __slots__ = ('op', 'arg1', 'arg2', 'result', 'lineno')
    def __init__(self, op, arg1, arg2, result, lineno):
        self.op, self.arg1, self.arg2, self.result, self.lineno = op, arg1, arg2, result, lineno
    def __repr__(self):
        return f"<{self.op} {self.arg1} {self.arg2} {self.result}>"

def load_program(path: str):
    instructions = []
    with open(path, encoding='utf-8') as f:
        for lineno, raw in enumerate(f, 1):
            parsed = parse_line(raw)
            if parsed is None: continue
            instructions.append(Instruction(*parsed, lineno))

    label_map = {inst.arg1: idx for idx, inst in enumerate(instructions) if inst.op == 'LABEL'}

    function_map = {}
    func_stack = []
    for idx, inst in enumerate(instructions):
        if inst.op == 'FUNCTION': func_stack.append((inst.result, idx))
        elif inst.op == 'FUNCTION_END':
            if func_stack:
                fname, start = func_stack.pop()
                function_map[fname] = (start, idx)

    return instructions, label_map, function_map

# ─────────────────────────────────────────────
# VM 執行環境
# ─────────────────────────────────────────────

class Frame:
    def __init__(self, name, instructions, label_map, globals_env):
        self.name = name
        self.instructions = instructions
        self.label_map = label_map
        self.globals_env = globals_env
        self.locals: dict = {}
        self.pc: int = 0
        self.return_value = None
        
        # 緩衝區 (改為 Stack 邏輯，支援無限巢狀)
        self.arg_buffer: list = []     
        self.list_buffer: list = []    
        self.dict_buffer: list = []    

    def resolve(self, token: str):
        if token is None or token == '_': return None
        if token in self.locals: return self.locals[token]
        if token in self.globals_env: return self.globals_env[token]
        val = parse_value(token)
        if isinstance(val, str) and val == token:
            raise NameError(f"未定義的名稱: {token!r}")
        return val

    def set_local(self, name: str, value):
        if name and name != '_': self.locals[name] = value

# ─────────────────────────────────────────────
# VM 主體
# ─────────────────────────────────────────────

class QD0VM:
    def __init__(self, instructions, label_map, function_map):
        self.instructions = instructions
        self.label_map = label_map
        self.function_map = function_map

        self.globals: dict = {
            'print': print,
            'len': len,
            'range': range,
            'int': int,
            'float': float,
            'str': str,
            'bool': bool,
            'list': list,
            'dict': dict,
            'sum': sum,
            'format': format, # 支援小數點格式化 (例如 :.2f)
            'True': True,
            'False': False,
            'None': None,
        }

        for fname, (start, end) in function_map.items():
            self.globals[fname] = self._make_function(fname, start, end)

    def _make_function(self, fname, start_idx, end_idx):
        vm = self
        def qd_function(*args):
            return vm._call_qd_function(fname, start_idx, end_idx, list(args))
        qd_function.__name__ = fname
        return qd_function

    def _call_qd_function(self, fname, start_idx, end_idx, args):
        frame = Frame(fname, self.instructions, self.label_map, self.globals)
        pc = start_idx + 1
        param_idx = 0

        while pc <= end_idx:
            inst = self.instructions[pc]
            if inst.op == 'ENTER_SCOPE': pc += 1; continue
            if inst.op == 'PARAM':
                if param_idx < len(args):
                    frame.locals[inst.result] = args[param_idx]
                param_idx += 1
                pc += 1
                continue
            break

        frame.pc = pc
        return self._run_frame(frame, end_idx)

    def run(self):
        frame = Frame('__main__', self.instructions, self.label_map, self.globals)
        frame.pc = 0
        self._run_frame(frame, len(self.instructions) - 1, top_level_only=True)

    def _run_frame(self, frame: Frame, end_idx: int, top_level_only=False):
        func_ranges = set(i for s, e in self.function_map.values() for i in range(s, e + 1))

        while frame.pc <= end_idx:
            idx = frame.pc
            if top_level_only and idx in func_ranges:
                frame.pc += 1; continue

            inst = self.instructions[idx]
            op = inst.op

            if op in ('FUNCTION', 'FUNCTION_END', 'ENTER_SCOPE', 'EXIT_SCOPE', 'PARAM', 'LABEL'):
                frame.pc += 1; continue

            try:
                ret = self._exec(inst, frame)
            except Exception as e:
                print(f"[執行錯誤] 第 {inst.lineno} 行 {inst}: {e}", file=sys.stderr)
                sys.exit(1)

            if ret and ret[0] == 'RETURN': return ret[1]
            if ret and ret[0] == 'JUMP':
                if ret[1] not in self.label_map: raise RuntimeError(f"未知標籤: {ret[1]!r}")
                frame.pc = self.label_map[ret[1]]
                continue

            frame.pc += 1
        return frame.return_value

    def _exec(self, inst: Instruction, frame: Frame):
        op, a1, a2, r = inst.op, inst.arg1, inst.arg2, inst.result
        V = lambda t: None if t in (None, '_') else frame.resolve(t)
        lit = lambda t: parse_value(t) if t not in (None, '_') else None

        # ── Load / Store ──
        if op == 'LOAD_CONST': frame.set_local(r, lit(a1))
        elif op == 'LOAD_NAME': frame.set_local(r, V(a1))
        elif op == 'LOAD_ATTR': frame.set_local(r, getattr(V(a1), a2))
        elif op == 'STORE':
            frame.globals_env[r] = V(a1)
            frame.set_local(r, V(a1))

        # ── 容器操作 (List/Dict/Subscript) - 支援巢狀堆疊 ──
        elif op == 'DICT_INSERT':
            frame.dict_buffer.append((V(a1), V(a2)))

        elif op == 'BUILD_DICT':
            count = int(lit(a1))
            items = frame.dict_buffer[-count:] # 取出最後 count 個
            del frame.dict_buffer[-count:]
            frame.set_local(r, {k: v for k, v in items})
            
        elif op == 'LIST_APPEND':
            frame.list_buffer.append((int(lit(a2)), V(a1)))

        elif op == 'BUILD_LIST':
            count = int(lit(a1))
            items_raw = frame.list_buffer[-count:] # 取出最後 count 個
            del frame.list_buffer[-count:]
            ordered = sorted(items_raw, key=lambda x: x[0])
            frame.set_local(r, [v for _, v in ordered])

        elif op == 'SUBSCRIPT':
            frame.set_local(r, V(a1)[V(a2)])

        # ── Arithmetic ──
        elif op == 'ADD': frame.set_local(r, V(a1) + V(a2))
        elif op == 'SUB': frame.set_local(r, V(a1) - V(a2))
        elif op == 'MUL': frame.set_local(r, V(a1) * V(a2))
        elif op == 'DIV': frame.set_local(r, V(a1) / V(a2))
        elif op == 'MOD': frame.set_local(r, V(a1) % V(a2))
        elif op == 'NEG': frame.set_local(r, -V(a1))

        # ── Comparison & Logical (補齊) ──
        elif op == 'CMP_EQ': frame.set_local(r, V(a1) == V(a2))
        elif op == 'CMP_NE': frame.set_local(r, V(a1) != V(a2))
        elif op == 'CMP_LT': frame.set_local(r, V(a1) < V(a2))
        elif op == 'CMP_LE': frame.set_local(r, V(a1) <= V(a2))
        elif op == 'CMP_GT': frame.set_local(r, V(a1) > V(a2))
        elif op == 'CMP_GE': frame.set_local(r, V(a1) >= V(a2))
        elif op == 'OR': frame.set_local(r, V(a1) or V(a2))
        elif op == 'AND': frame.set_local(r, V(a1) and V(a2))
        elif op == 'NOT': frame.set_local(r, not V(a1))

        # ── Control Flow ──
        elif op == 'JUMP': return ('JUMP', a1)
        elif op == 'BRANCH_IF_TRUE':
            if V(a1): return ('JUMP', r)
        elif op == 'BRANCH_IF_FALSE':
            if not V(a1): return ('JUMP', r)

        # ── Iteration ──
        elif op == 'GET_ITER':
            frame.set_local(r, iter(V(a1)))
        elif op == 'ITER_NEXT':
            try:
                frame.set_local(r, next(V(a1)))
                frame.locals[f'__exhaust_{a1}'] = False
            except StopIteration:
                frame.locals[f'__exhaust_{a1}'] = True
        elif op == 'BRANCH_IF_EXHAUST':
            if frame.locals.get(f'__exhaust_{a1}', False):
                return ('JUMP', r)

        # ── Function / Call ──
        elif op == 'ARG_PUSH':
            frame.arg_buffer.append((lit(a2), V(a1)))

        elif op == 'CALL':
            func = V(a1)
            argc = int(lit(a2)) if a2 and a2 != '_' else len(frame.arg_buffer)
            raw_args = frame.arg_buffer[-argc:] # 取出最後 argc 個參數
            del frame.arg_buffer[-argc:]
            ordered = sorted(raw_args, key=lambda x: (x[0] is None, x[0]))
            args = [v for _, v in ordered]
            frame.set_local(r, func(*args))

        elif op == 'RETURN':
            return ('RETURN', V(a1))

        else:
            raise RuntimeError(f"未知指令: {op!r}")
        return None

# ─────────────────────────────────────────────
# 入口點
# ─────────────────────────────────────────────

def main():
    if len(sys.argv) < 2:
        print("用法: python qd0vm.py <file.qd>", file=sys.stderr)
        sys.exit(1)

    path = sys.argv[1]
    try:
        instructions, label_map, function_map = load_program(path)
    except FileNotFoundError:
        print(f"找不到檔案: {path!r}", file=sys.stderr)
        sys.exit(1)

    vm = QD0VM(instructions, label_map, function_map)
    vm.run()

if __name__ == '__main__':
    main()