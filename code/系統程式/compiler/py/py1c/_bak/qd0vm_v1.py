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
    """將字串 token 轉換成 Python 值（整數、浮點數、字串、None/布林、或原樣字串）。"""
    if s == '_':
        return None
    if s in ('True', 'true'):
        return True
    if s in ('False', 'false'):
        return False
    if s in ('None', 'null'):
        return None
    # 字串字面值  'hello' 或 "hello"
    if (s.startswith("'") and s.endswith("'")) or \
       (s.startswith('"') and s.endswith('"')):
        return s[1:-1]
    try:
        return int(s)
    except ValueError:
        pass
    try:
        return float(s)
    except ValueError:
        pass
    return s  # 識別符（變數名 / 標籤名）


def tokenize_line(line: str):
    """去掉注釋，切出 token list。支援帶引號的字串中有空白。"""
    # 移除行內注釋（; ...），但要避免誤刪字串內的分號
    # 簡單策略：找到第一個不在引號內的 ;
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

    # 切 token，保留帶引號的字串為單一 token
    tokens = []
    pat = r"""'[^']*'|"[^"]*"|\S+"""
    for m in re.finditer(pat, line):
        tokens.append(m.group())
    return tokens


def parse_line(line: str):
    """
    回傳 (op, arg1_raw, arg2_raw, result_raw) 或 None（空行/注釋）。
    支援標籤定義語法：  L_name::
    """
    line = line.strip()
    if not line or line.startswith(';'):
        return None

    # 標籤定義：  L_foo::
    if re.match(r'^[A-Za-z_]\w*::$', line):
        label = line[:-2]
        return ('LABEL', label, None, None)

    tokens = tokenize_line(line)
    if not tokens:
        return None

    # 補齊到 4 個欄位
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
        self.op = op
        self.arg1 = arg1      # 原始字串
        self.arg2 = arg2
        self.result = result
        self.lineno = lineno

    def __repr__(self):
        return f"<{self.op} {self.arg1} {self.arg2} {self.result}>"


def load_program(path: str):
    """讀取 .qd 檔，回傳 (instructions, label_map, function_map)。"""
    instructions = []
    with open(path, encoding='utf-8') as f:
        for lineno, raw in enumerate(f, 1):
            parsed = parse_line(raw)
            if parsed is None:
                continue
            op, a1, a2, r = parsed
            instructions.append(Instruction(op, a1, a2, r, lineno))

    # 建立標籤位置表  label -> instruction index
    label_map = {}
    for idx, inst in enumerate(instructions):
        if inst.op == 'LABEL':
            label_map[inst.arg1] = idx

    # 建立函式表  name -> (start_idx, end_idx)
    # start_idx = FUNCTION 指令那行；end_idx = FUNCTION_END 指令那行
    function_map = {}
    func_stack = []
    for idx, inst in enumerate(instructions):
        if inst.op == 'FUNCTION':
            fname = inst.result  # FUNCTION _ _ factorial
            func_stack.append((fname, idx))
        elif inst.op == 'FUNCTION_END':
            if func_stack:
                fname, start = func_stack.pop()
                function_map[fname] = (start, idx)

    return instructions, label_map, function_map


# ─────────────────────────────────────────────
# VM 執行環境
# ─────────────────────────────────────────────

class Frame:
    """單一呼叫框架（含區域暫存器與區域變數）。"""

    def __init__(self, name, instructions, label_map, globals_env):
        self.name = name
        self.instructions = instructions
        self.label_map = label_map       # 全域標籤表（含本函式內）
        self.globals_env = globals_env   # 全域環境（可見內建函式 & 全域變數）
        self.locals: dict = {}           # 區域變數 + 暫存器
        self.pc: int = 0                 # program counter（指向 instructions 的 index）
        self.arg_buffer: list = []       # ARG_PUSH 暫存區
        self.return_value = None

    def resolve(self, token: str):
        """取得一個 token 的執行期值。"""
        if token is None or token == '_':
            return None
        # 先從區域查
        if token in self.locals:
            return self.locals[token]
        # 再從全域查
        if token in self.globals_env:
            return self.globals_env[token]
        # 嘗試解析字面值
        val = parse_value(token)
        if isinstance(val, str) and val == token:
            raise NameError(f"未定義的名稱: {token!r}")
        return val

    def set_local(self, name: str, value):
        if name and name != '_':
            self.locals[name] = value

    def set_global(self, name: str, value):
        if name and name != '_':
            self.globals_env[name] = value


# ─────────────────────────────────────────────
# VM 主體
# ─────────────────────────────────────────────

class QD0VM:

    def __init__(self, instructions, label_map, function_map):
        self.instructions = instructions
        self.label_map = label_map        # str -> idx
        self.function_map = function_map  # str -> (start_idx, end_idx)

        # 全域環境：內建函式
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
            'abs': abs,
            'max': max,
            'min': min,
            'sum': sum,
            'True': True,
            'False': False,
            'None': None,
        }

        # 預先把函式物件放進全域
        for fname, (start, end) in function_map.items():
            self.globals[fname] = self._make_function(fname, start, end)

    # ── 函式物件工廠 ──────────────────────────────

    def _make_function(self, fname, start_idx, end_idx):
        """回傳一個 Python callable，執行對應的 QD0 函式體。"""
        vm = self

        def qd_function(*args):
            return vm._call_qd_function(fname, start_idx, end_idx, list(args))

        qd_function.__name__ = fname
        return qd_function

    def _call_qd_function(self, fname, start_idx, end_idx, args):
        """建立新 Frame 並執行函式體，回傳返回值。"""
        frame = Frame(fname, self.instructions, self.label_map, self.globals)

        # 找到 ENTER_SCOPE 之後開始，蒐集 PARAM 宣告並填入引數
        pc = start_idx + 1  # 跳過 FUNCTION 本身
        param_idx = 0

        # 先掃描 ENTER_SCOPE / PARAM，再決定起始 pc
        while pc <= end_idx:
            inst = self.instructions[pc]
            if inst.op == 'ENTER_SCOPE':
                pc += 1
                continue
            if inst.op == 'PARAM':
                pname = inst.result
                if param_idx < len(args):
                    frame.locals[pname] = args[param_idx]
                param_idx += 1
                pc += 1
                continue
            break  # 第一個非 PARAM/ENTER_SCOPE 指令

        frame.pc = pc
        return self._run_frame(frame, end_idx)

    # ── 主執行迴圈 ────────────────────────────────

    def run(self):
        """從第一條非函式定義指令開始執行（頂層程式碼）。"""
        # 找到第一條「不在任何函式定義內」的指令
        top_level_start = 0
        in_func = False
        for idx, inst in enumerate(self.instructions):
            if inst.op == 'FUNCTION':
                in_func = True
            elif inst.op == 'FUNCTION_END':
                in_func = False
                top_level_start = idx + 1  # 繼續掃描
            # 這個指令是頂層的起點（不在函式內，且不是 FUNCTION/FUNCTION_END）
            # 我們要找第一個這樣的 idx

        # 重新掃描，找到所有「頂層」指令段
        # 策略：把頂層指令蒐集後建立一個主 Frame
        frame = Frame('__main__', self.instructions, self.label_map, self.globals)
        frame.pc = 0
        self._run_frame(frame, len(self.instructions) - 1, top_level_only=True)

    def _run_frame(self, frame: Frame, end_idx: int, top_level_only=False):
        """執行 frame，從 frame.pc 到 end_idx（含）或遇到 RETURN 為止。"""

        instructions = self.instructions
        label_map = self.label_map

        # 追蹤哪些 index 屬於函式定義（需跳過）
        func_ranges = set()
        for fname, (s, e) in self.function_map.items():
            for i in range(s, e + 1):
                func_ranges.add(i)

        while frame.pc <= end_idx:
            idx = frame.pc
            if top_level_only and idx in func_ranges:
                frame.pc += 1
                continue

            inst = instructions[idx]
            op = inst.op

            # ── 跳過這些結構性指令 ──
            if op in ('FUNCTION', 'FUNCTION_END', 'ENTER_SCOPE',
                      'EXIT_SCOPE', 'PARAM', 'LABEL'):
                frame.pc += 1
                continue

            try:
                ret = self._exec(inst, frame)
            except Exception as e:
                print(f"[執行錯誤] 第 {inst.lineno} 行 {inst}: {e}", file=sys.stderr)
                sys.exit(1)

            if ret is not None and ret[0] == 'RETURN':
                return ret[1]

            # JUMP / BRANCH 指令會直接修改 frame.pc
            if ret is not None and ret[0] == 'JUMP':
                label = ret[1]
                if label not in label_map:
                    raise RuntimeError(f"未知標籤: {label!r}")
                frame.pc = label_map[label]
                continue

            frame.pc += 1

        return frame.return_value

    # ── 單條指令執行 ──────────────────────────────

    def _exec(self, inst: Instruction, frame: Frame):
        op = inst.op
        a1, a2, r = inst.arg1, inst.arg2, inst.result

        def V(token):
            """解析 token 為執行期值（跳過 '_'）。"""
            if token is None or token == '_':
                return None
            return frame.resolve(token)

        def lit(token):
            """直接解析字面值（不查變數）。"""
            return parse_value(token) if token and token != '_' else None

        # ── Load / Store ──────────────────────────
        if op == 'LOAD_CONST':
            val = lit(a1)  # a1 是字面值
            frame.set_local(r, val)

        elif op == 'LOAD_NAME':
            val = frame.resolve(a1)
            frame.set_local(r, val)

        elif op == 'LOAD_ATTR':
            obj = V(a1)
            attr = a2
            frame.set_local(r, getattr(obj, attr))

        elif op == 'STORE':
            frame.globals_env[r] = V(a1)  # 存到可見作用域（全域 or 區域均可）
            frame.set_local(r, V(a1))

        elif op == 'STORE_ATTR':
            obj = V(a1)
            attr = a2
            setattr(obj, attr, V(r))

        elif op == 'DELETE_NAME':
            name = a1
            frame.locals.pop(name, None)
            frame.globals_env.pop(name, None)

        # ── Arithmetic ────────────────────────────
        elif op == 'ADD':
            frame.set_local(r, V(a1) + V(a2))
        elif op == 'SUB':
            frame.set_local(r, V(a1) - V(a2))
        elif op == 'MUL':
            frame.set_local(r, V(a1) * V(a2))
        elif op == 'DIV':
            frame.set_local(r, V(a1) / V(a2))
        elif op == 'NEG':
            frame.set_local(r, -V(a1))
        elif op == 'BINOP':
            # 舊版泛型 BINOP（規格書範例中使用）
            # 無法靜態判斷運算子，嘗試 +
            frame.set_local(r, V(a1) + V(a2))

        # ── Comparison ────────────────────────────
        elif op == 'CMP_EQ':
            frame.set_local(r, V(a1) == V(a2))
        elif op == 'CMP_LT':
            frame.set_local(r, V(a1) < V(a2))
        elif op == 'CMP_GT':
            frame.set_local(r, V(a1) > V(a2))
        elif op == 'CMP_LE':
            frame.set_local(r, V(a1) <= V(a2))
        elif op == 'CMP_GE':
            frame.set_local(r, V(a1) >= V(a2))
        elif op == 'CMP_NE':
            frame.set_local(r, V(a1) != V(a2))
        elif op == 'CMP_IN':
            frame.set_local(r, V(a1) in V(a2))
        elif op == 'CMP':
            # 舊版泛型 CMP（規格書條件判斷範例中使用）
            frame.set_local(r, V(a1) > V(a2))

        # ── Boolean ───────────────────────────────
        elif op == 'AND':
            frame.set_local(r, V(a1) and V(a2))
        elif op == 'OR':
            frame.set_local(r, V(a1) or V(a2))
        elif op == 'NOT':
            frame.set_local(r, not V(a1))

        # ── Control Flow ──────────────────────────
        elif op == 'JUMP':
            return ('JUMP', a1)

        elif op == 'BRANCH_IF_TRUE':
            if V(a1):
                return ('JUMP', r)  # result 欄位是標籤

        elif op == 'BRANCH_IF_FALSE':
            if not V(a1):
                return ('JUMP', r)

        # ── Iteration ────────────────────────────
        elif op == 'GET_ITER':
            frame.set_local(r, iter(V(a1)))

        elif op == 'ITER_NEXT':
            it = V(a1)
            try:
                val = next(it)
                frame.set_local(r, val)
                frame.locals[f'__exhaust_{a1}'] = False
            except StopIteration:
                frame.locals[f'__exhaust_{a1}'] = True

        elif op == 'BRANCH_IF_EXHAUST':
            if frame.locals.get(f'__exhaust_{a1}', False):
                return ('JUMP', r)

        # ── Function / Call ───────────────────────
        elif op == 'ARG_PUSH':
            idx_arg = lit(a2)  # argument index (for ordering)
            frame.arg_buffer.append((idx_arg, V(a1)))

        elif op == 'CALL':
            func = V(a1)
            argc = int(lit(a2)) if a2 and a2 != '_' else len(frame.arg_buffer)
            # 排序引數（依 index）
            ordered = sorted(frame.arg_buffer[:argc], key=lambda x: (x[0] is None, x[0]))
            args = [v for _, v in ordered]
            frame.arg_buffer = frame.arg_buffer[argc:]  # 消耗掉已用引數
            result_val = func(*args)
            frame.set_local(r, result_val)

        elif op == 'RETURN':
            return ('RETURN', V(a1))

        elif op == 'BUILD_LIST':
            count = int(lit(a1))
            # 從 arg_buffer 取 count 個
            items = [v for _, v in frame.arg_buffer[:count]]
            frame.arg_buffer = frame.arg_buffer[count:]
            frame.set_local(r, items)

        elif op == 'MAKE_CLOSURE':
            # 簡化版：忽略 cells，直接當普通函式查找
            fname = a1
            func = frame.resolve(fname)
            frame.set_local(r, func)

        # ── Exception ────────────────────────────
        elif op == 'TRY_BEGIN':
            pass   # 簡化版 VM 不實作完整例外跳轉
        elif op == 'TRY_END':
            pass
        elif op == 'RAISE':
            raise Exception(V(a1))
        elif op == 'MATCH_EXC':
            pass  # 簡化

        # ── LLVM Bridge ──────────────────────────
        elif op in ('ASSUME_TYPE', 'BOX', 'UNBOX'):
            if op != 'ASSUME_TYPE':
                frame.set_local(r, V(a1))

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