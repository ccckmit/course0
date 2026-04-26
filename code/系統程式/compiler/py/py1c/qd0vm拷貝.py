#!/usr/bin/env python3
"""
qd0vm.py — QD0 IR Virtual Machine
用法: python qd0vm.py <file.qd> [args...]
"""

import sys
import re

# ─────────────────────────────────────────────
# 詞法分析：將一行解析成 (op, arg1, arg2, result)
# ─────────────────────────────────────────────

def parse_value(s):
    if s == '_': return None
    if s == 'True' or s == 'true': return True
    if s == 'False' or s == 'false': return False
    if s == 'None' or s == 'null': return None
    
    is_sq = s.startswith("'") and s.endswith("'")
    is_dq = s.startswith('"') and s.endswith('"')
    if is_sq or is_dq:
        val = s[1:-1]
        try:
            return val.encode('utf-8').decode('unicode_escape').encode('latin-1').decode('utf-8')
        except Exception as e:
            return val.replace('\\n', '\n').replace('\\t', '\t').replace('\\r', '\r')
            
    try: return int(s)
    except ValueError: pass
    try: return float(s)
    except ValueError: pass
    
    return s

def tokenize_line(line):
    in_q = False
    q_char = ''
    clean = []
    i = 0
    n = len(line)
    while i < n:
        c = line[i]
        if in_q:
            if c == q_char:
                in_q = False
            clean.append(c)
        else:
            if c == '"' or c == "'":
                in_q = True
                q_char = c
                clean.append(c)
            elif c == ';':
                break
            else:
                clean.append(c)
        i = i + 1
    
    clean_str = ''.join(clean).strip()
    if not clean_str:
        return []

    tokens = []
    pat = r"'[^']*'|\"[^\"]*\"|\S+"
    for m in re.finditer(pat, clean_str):
        tokens.append(m.group())
    return tokens

def parse_line(line):
    line = line.strip()
    if not line or line.startswith(';'):
        return None

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
    def __init__(self, op, arg1, arg2, result, lineno):
        self.op = op
        self.arg1 = arg1
        self.arg2 = arg2
        self.result = result
        self.lineno = lineno
    def __repr__(self):
        return "<" + str(self.op) + " " + str(self.arg1) + " " + str(self.arg2) + " " + str(self.result) + ">"

def load_program(path):
    instructions = []
    f = open(path, 'r', encoding='utf-8')
    lineno = 1
    for raw in f:
        parsed = parse_line(raw)
        if parsed is not None:
            op = parsed[0]
            arg1 = parsed[1]
            arg2 = parsed[2]
            result = parsed[3]
            instructions.append(Instruction(op, arg1, arg2, result, lineno))
        lineno = lineno + 1
    f.close()

    label_map = {}
    idx = 0
    for inst in instructions:
        if inst.op == 'LABEL':
            label_map[inst.arg1] = idx
        idx = idx + 1

    function_map = {}
    func_stack = []
    idx = 0
    for inst in instructions:
        if inst.op == 'FUNCTION': 
            func_stack.append((inst.result, idx))
        elif inst.op == 'FUNCTION_END':
            if len(func_stack) > 0:
                popped = func_stack.pop()
                fname = popped[0]
                start = popped[1]
                function_map[fname] = (start, idx)
        idx = idx + 1

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
        self.locals = {}
        self.pc = 0
        self.return_value = None
        
        self.arg_buffer = []     
        self.list_buffer = []    
        self.dict_buffer = []
        
        # 例外處理疊代
        self.try_handlers = []
        self.current_exception = None

    def resolve(self, token):
        if token is None or token == '_': 
            return None
        if token in self.locals: 
            return self.locals[token]
        if token in self.globals_env: 
            return self.globals_env[token]
        val = parse_value(token)
        if type(val) is str and val == token:
            print("未定義的名稱: " + str(token))
            sys.exit(1)
        return val

    def set_local(self, name, value):
        if name and name != '_': 
            self.locals[name] = value

# ─────────────────────────────────────────────
# VM 主體
# ─────────────────────────────────────────────

class QD0VM:
    def __init__(self, instructions, label_map, function_map):
        self.instructions = instructions
        self.label_map = label_map
        self.function_map = function_map

        # 注入完整的 Python 基礎環境
        self.globals = {
            'print': print,
            'len': len,
            'range': range,
            'int': int,
            'float': float,
            'str': str,
            'bool': bool,
            'list': list,
            'dict': dict,
            'set': set,
            'tuple': tuple,
            'sum': sum,
            'format': format,
            'getattr': getattr,
            'setattr': setattr,
            'hasattr': hasattr,
            'type': type,
            'open': open,
            'Exception': Exception,
            'ValueError': ValueError,
            'FileNotFoundError': FileNotFoundError,
            'StopIteration': StopIteration,
            'True': True,
            'False': False,
            'None': None,
            '__name__': '__main__',
            '__file__': sys.argv[0] if len(sys.argv) > 0 else 'unknown'
        }

        for fname in function_map:
            start_end = function_map[fname]
            self.globals[fname] = self._make_function(fname, start_end[0], start_end[1])

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
            if inst.op == 'ENTER_SCOPE': 
                pc = pc + 1
                continue
            if inst.op == 'PARAM':
                if param_idx < len(args):
                    frame.locals[inst.result] = args[param_idx]
                param_idx = param_idx + 1
                pc = pc + 1
                continue
            break

        frame.pc = pc
        return self._run_frame(frame, end_idx)

    def run(self):
        frame = Frame('__main__', self.instructions, self.label_map, self.globals)
        frame.pc = 0
        self._run_frame(frame, len(self.instructions) - 1, True)

    def _run_frame(self, frame, end_idx, top_level_only=False):
        func_ranges = set()
        for fname in self.function_map:
            s_e = self.function_map[fname]
            s = s_e[0]
            e = s_e[1]
            for i in range(s, e + 1):
                func_ranges.add(i)

        while frame.pc <= end_idx:
            idx = frame.pc
            if top_level_only and idx in func_ranges:
                frame.pc = frame.pc + 1
                continue

            inst = self.instructions[idx]
            op = inst.op

            if op in ('FUNCTION', 'FUNCTION_END', 'ENTER_SCOPE', 'EXIT_SCOPE', 'PARAM', 'LABEL'):
                frame.pc = frame.pc + 1
                continue

            # 例外監控機制
            if op == 'TRY_BEGIN':
                frame.try_handlers.append(inst.arg1)
                frame.pc = frame.pc + 1
                continue
            elif op == 'TRY_END':
                if len(frame.try_handlers) > 0:
                    frame.try_handlers.pop()
                frame.pc = frame.pc + 1
                continue

            try:
                ret = self._exec(inst, frame)
            except SystemExit as se:
                sys.exit(se.code)
            except Exception as e:
                # 攔截例外並跳躍至對應的 handler
                if len(frame.try_handlers) > 0:
                    catch_label = frame.try_handlers.pop()
                    if catch_label in frame.label_map:
                        frame.pc = frame.label_map[catch_label]
                        frame.current_exception = e
                        continue
                print("[執行錯誤] 第 " + str(inst.lineno) + " 行 " + str(inst) + ": " + str(e))
                sys.exit(1)

            if ret and ret[0] == 'RETURN': 
                return ret[1]
            if ret and ret[0] == 'JUMP':
                if ret[1] not in frame.label_map: 
                    print("未知標籤: " + str(ret[1]))
                    sys.exit(1)
                frame.pc = frame.label_map[ret[1]]
                continue

            frame.pc = frame.pc + 1
        return frame.return_value

    def _exec(self, inst, frame):
        op = inst.op
        a1 = inst.arg1
        a2 = inst.arg2
        r = inst.result

        def V(t):
            if t is None or t == '_': return None
            return frame.resolve(t)

        def lit(t):
            if t is None or t == '_': return None
            return parse_value(t)

        if op == 'LOAD_CONST': frame.set_local(r, lit(a1))
        elif op == 'LOAD_NAME': frame.set_local(r, V(a1))
        elif op == 'LOAD_ATTR': frame.set_local(r, getattr(V(a1), a2))
        elif op == 'STORE':
            frame.globals_env[r] = V(a1)
            frame.set_local(r, V(a1))

        # ── 模組匯入與例外處理 (新增) ──
        elif op == 'IMPORT':
            mod = __import__(a1)
            frame.globals_env[a1] = mod
            frame.set_local(a1, mod)

        elif op == 'IMPORT_FROM':
            mod = __import__(a1, fromlist=[a2])
            val = getattr(mod, a2)
            frame.globals_env[a2] = val
            frame.set_local(a2, val)

        elif op == 'IMPORT_STAR':
            mod = __import__(a1, fromlist=['*'])
            for k in dir(mod):
                if not k.startswith('_'):
                    val = getattr(mod, k)
                    frame.globals_env[k] = val
                    frame.set_local(k, val)

        elif op == 'MATCH_EXC': pass
        elif op == 'EXCEPT_VAR':
            frame.set_local(r, frame.current_exception)
        elif op == 'RAISE':
            raise Exception(V(a1))

        # ── 容器操作 ──
        elif op == 'DICT_INSERT':
            frame.dict_buffer.append((V(a1), V(a2)))

        elif op == 'BUILD_DICT':
            count = int(lit(a1))
            if count > 0:
                items = frame.dict_buffer[-count:] 
                del frame.dict_buffer[-count:]
            else: items = []
            d = {}
            for kv in items:
                d[kv[0]] = kv[1]
            frame.set_local(r, d)
            
        elif op == 'LIST_APPEND':
            frame.list_buffer.append((int(lit(a2)), V(a1)))

        elif op == 'BUILD_LIST':
            count = int(lit(a1))
            if count > 0:
                items_raw = frame.list_buffer[-count:] 
                del frame.list_buffer[-count:]
            else: items_raw = []
            
            n_items = len(items_raw)
            for i in range(n_items):
                for j in range(0, n_items - i - 1):
                    if items_raw[j][0] > items_raw[j + 1][0]:
                        tmp = items_raw[j]
                        items_raw[j] = items_raw[j + 1]
                        items_raw[j + 1] = tmp
                        
            lst = []
            for item in items_raw: lst.append(item[1])
            frame.set_local(r, lst)

        elif op == 'TUPLE_APPEND':
            frame.list_buffer.append((int(lit(a2)), V(a1)))

        elif op == 'BUILD_TUPLE':
            count = int(lit(a1))
            if count > 0:
                items_raw = frame.list_buffer[-count:] 
                del frame.list_buffer[-count:]
            else: items_raw = []
            
            n_items = len(items_raw)
            for i in range(n_items):
                for j in range(0, n_items - i - 1):
                    if items_raw[j][0] > items_raw[j + 1][0]:
                        tmp = items_raw[j]
                        items_raw[j] = items_raw[j + 1]
                        items_raw[j + 1] = tmp
                        
            lst = []
            for item in items_raw: lst.append(item[1])
            frame.set_local(r, tuple(lst))

        elif op == 'SUBSCRIPT':
            frame.set_local(r, V(a1)[V(a2)])
        elif op == 'SUBSCRIPT_SET':
            obj = V(a1)
            key = V(a2)
            val = V(r)
            obj[key] = val

        # ── 運算子 ──
        elif op == 'ADD': frame.set_local(r, V(a1) + V(a2))
        elif op == 'SUB': frame.set_local(r, V(a1) - V(a2))
        elif op == 'MUL': frame.set_local(r, V(a1) * V(a2))
        elif op == 'DIV': frame.set_local(r, V(a1) / V(a2))
        elif op == 'FLOOR_DIV': frame.set_local(r, V(a1) // V(a2))
        elif op == 'MOD': frame.set_local(r, V(a1) % V(a2))
        elif op == 'NEG': frame.set_local(r, -V(a1))
        elif op == 'POS': frame.set_local(r, +V(a1))
        elif op == 'BIT_NOT': frame.set_local(r, ~V(a1))

        elif op == 'CMP_EQ': frame.set_local(r, V(a1) == V(a2))
        elif op == 'CMP_NE': frame.set_local(r, V(a1) != V(a2))
        elif op == 'CMP_LT': frame.set_local(r, V(a1) < V(a2))
        elif op == 'CMP_LE': frame.set_local(r, V(a1) <= V(a2))
        elif op == 'CMP_GT': frame.set_local(r, V(a1) > V(a2))
        elif op == 'CMP_GE': frame.set_local(r, V(a1) >= V(a2))
        
        elif op == 'CMP_IS': frame.set_local(r, V(a1) is V(a2))
        elif op == 'CMP_IS_NOT': frame.set_local(r, V(a1) is not V(a2))
        elif op == 'CMP_IN': frame.set_local(r, V(a1) in V(a2))
        elif op == 'CMP_NOT_IN': frame.set_local(r, V(a1) not in V(a2))

        elif op == 'OR': frame.set_local(r, V(a1) or V(a2))
        elif op == 'AND': frame.set_local(r, V(a1) and V(a2))
        elif op == 'NOT': frame.set_local(r, not V(a1))

        # ── 流程控制 ──
        elif op == 'JUMP': return ('JUMP', a1)
        elif op == 'BRANCH_IF_TRUE':
            if V(a1): return ('JUMP', r)
        elif op == 'BRANCH_IF_FALSE':
            if not V(a1): return ('JUMP', r)

        # ── 疊代 ──
        elif op == 'GET_ITER' or op == 'UNPACK_ITER':
            frame.set_local(r, iter(V(a1)))
        elif op == 'ITER_NEXT':
            try:
                frame.set_local(r, next(V(a1)))
                frame.locals['__exhaust_' + str(a1)] = False
            except StopIteration:
                frame.locals['__exhaust_' + str(a1)] = True
        elif op == 'BRANCH_IF_EXHAUST':
            if frame.locals.get('__exhaust_' + str(a1), False):
                return ('JUMP', r)

        # ── 函數呼叫 ──
        elif op == 'ARG_PUSH':
            frame.arg_buffer.append((lit(a2), V(a1)))

        elif op == 'CALL':
            func = V(a1)
            if a2 and a2 != '_': argc = int(lit(a2))
            else: argc = len(frame.arg_buffer)
            
            if argc > 0:
                raw_args = frame.arg_buffer[-argc:] 
                del frame.arg_buffer[-argc:]
            else: raw_args = []
            
            n_args = len(raw_args)
            for i in range(n_args):
                for j in range(0, n_args - i - 1):
                    idx1 = raw_args[j][0]
                    idx2 = raw_args[j + 1][0]
                    val1 = idx1 if idx1 is not None else 999999
                    val2 = idx2 if idx2 is not None else 999999
                    if val1 > val2:
                        tmp = raw_args[j]
                        raw_args[j] = raw_args[j + 1]
                        raw_args[j + 1] = tmp
                        
            args = []
            for item in raw_args: args.append(item[1])
            frame.set_local(r, func(*args))

        elif op == 'RETURN':
            return ('RETURN', V(a1))

        else:
            print("未知指令: " + str(op))
            sys.exit(1)
            
        return None

def main():
    if len(sys.argv) < 2:
        print("用法: python qd0vm.py <file.qd> [args...]")
        sys.exit(1)

    path = sys.argv[1]
    
    # 聰明地切換 argv，讓客座腳本（Guest Script）能正確接收到它自己的參數
    sys.argv = [sys.argv[0]] + sys.argv[2:]

    try:
        instructions, label_map, function_map = load_program(path)
    except FileNotFoundError:
        print("找不到檔案: " + str(path))
        sys.exit(1)

    vm = QD0VM(instructions, label_map, function_map)
    vm.run()

if __name__ == '__main__':
    main()