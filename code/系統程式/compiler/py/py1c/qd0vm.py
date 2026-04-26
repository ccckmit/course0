#!/usr/bin/env python3
"""qd0vm.py — QD0 IR Virtual Machine"""
import sys
import re
import importlib
import os

# 關閉 Python 3.11+ 的精細錯誤波浪線標示
os.environ['PYTHONNODEBUGRANGES'] = '1'

OP = 0
A1 = 1
A2 = 2
RR = 3
LN = 4

_TOKEN_RE = re.compile(r"'[^']*'|\"[^\"]*\"|\S+")
_LABEL_RE  = re.compile(r'^[A-Za-z_]\w*:{1,2}$')

def parse_value(s):
    if s == '_': return None
    if s == 'True' or s == 'true': return True
    if s == 'False' or s == 'false': return False
    if s == 'None' or s == 'null': return None
    if not s: return s
    c0 = s[0]
    if c0 == "'" or c0 == '"':
        val = s[1:-1]
        result = []
        i = 0
        n = len(val)
        while i < n:
            if val[i] == '\\' and i + 1 < n:
                nxt = val[i + 1]
                if nxt == 'n': result.append('\n'); i = i + 2
                elif nxt == 't': result.append('\t'); i = i + 2
                elif nxt == 'r': result.append('\r'); i = i + 2
                elif nxt == '\\': result.append('\\'); i = i + 2
                elif nxt == '"': result.append('"'); i = i + 2
                elif nxt == "'": result.append("'"); i = i + 2
                else: result.append(val[i]); i = i + 1
            else:
                result.append(val[i])
                i = i + 1
        return ''.join(result)
    try: return int(s)
    except ValueError: pass
    try: return float(s)
    except ValueError: pass
    return s

def tokenize_line(line):
    tokens = []
    i = 0
    n = len(line)
    while n > 0 and (line[n - 1] == '\n' or line[n - 1] == '\r'):
        n = n - 1
        
    while i < n:
        c = line[i]
        if c == ' ' or c == '\t':
            i = i + 1
        elif c == ';':
            break
        elif c == "'" or c == '"':
            q = c
            buf = [c]
            i = i + 1
            in_string = True
            while i < n and in_string:
                ch = line[i]
                if ch == '\\' and i + 1 < n:
                    buf.append(ch)
                    buf.append(line[i + 1])
                    i = i + 2
                elif ch == q:
                    buf.append(ch)
                    i = i + 1
                    in_string = False
                else:
                    buf.append(ch)
                    i = i + 1
            tokens.append(''.join(buf))
        else:
            buf = []
            in_word = True
            while i < n and in_word:
                ch = line[i]
                if ch == ' ' or ch == '\t' or ch == ';':
                    in_word = False
                else:
                    buf.append(ch)
                    i = i + 1
            if buf:
                tokens.append(''.join(buf))
    return tokens

def parse_line(line):
    line = line.strip()
    if not line or line[0] == ';': return None
    if _LABEL_RE.match(line): return ('LABEL', line.rstrip(':'), '_', '_')
    tokens = tokenize_line(line)
    if not tokens: return None
    while len(tokens) < 4: tokens.append('_')
    return (tokens[0].upper(), tokens[1], tokens[2], tokens[3])

def load_program(path):
    instructions = []
    label_map = {}
    function_map = {}
    class_map = {}
    func_stack = []
    cls_stack = []
    f = open(path, 'r', encoding='utf-8')
    lineno = 0
    idx = 0
    for raw in f:
        lineno = lineno + 1
        parsed = parse_line(raw)
        if parsed is not None:
            op, a1, a2, r = parsed[0], parsed[1], parsed[2], parsed[3]
            inst = [op, a1, a2, r, lineno]
            instructions.append(inst)
            if op == 'LABEL': label_map[a1] = idx
            elif op == 'FUNCTION': func_stack.append((r, idx))
            elif op == 'FUNCTION_END':
                if len(func_stack) > 0:
                    p = func_stack.pop()
                    function_map[p[0]] = (p[1], idx)
            elif op == 'CLASS': cls_stack.append((a1, idx))
            elif op == 'CLASS_END':
                if len(cls_stack) > 0:
                    p = cls_stack.pop()
                    class_map[p[0]] = (p[1], idx)
            idx = idx + 1
    f.close()
    return instructions, label_map, function_map, class_map

class Frame:
    def __init__(self, name, instructions, label_map, globs):
        self.name = name
        self.instructions = instructions
        self.label_map = label_map
        self.globs = globs
        self.locs = {}
        self.pc = 0
        self.arg_buf = []
        self.kwarg_buf = {}
        self.list_buf = []
        self.tuple_buf = []
        self.set_buf = []
        self.dict_buf = []
        self.exhaust = {}
        self.try_stack = []
        self.current_exc = None

    def resolve(self, token):
        if not isinstance(token, str): return token
        if token == '_': return None
        if token in self.locs: return self.locs[token]
        if token in self.globs: return self.globs[token]
        val = parse_value(token)
        if type(val) is str and val == token:
            raise NameError('未定義: ' + token)
        return val

    def resolve_global_first(self, token):
        if not isinstance(token, str): return token
        if token == '_': return None
        if token in self.globs: return self.globs[token]
        if token in self.locs: return self.locs[token]
        val = parse_value(token)
        if type(val) is str and val == token:
            raise NameError('未定義: ' + token)
        return val

    def setv(self, name, value):
        if name and name != '_':
            self.locs[name] = value

class QD0VM:
    def __init__(self, instructions, label_map, function_map, class_map, script_argv=None):
        self.instructions = instructions
        self.label_map = label_map
        self.function_map = function_map
        self.class_map = class_map

        import types
        ss = types.SimpleNamespace()
        
        # 【關鍵修復】: 將 Tuple 改為 List，避免被 py0c 編譯為字串常數！
        for attr in ['stdin', 'stdout', 'stderr', 'exit', 'path', 'platform', 'version']:
            try: setattr(ss, attr, getattr(sys, attr))
            except Exception: pass
        
        if script_argv is not None: ss.argv = list(script_argv)
        else: ss.argv = list(sys.argv)

        self.globs = {
            'print': print, 'len': len, 'range': range,
            'int': int, 'float': float, 'str': str, 'bool': bool,
            'list': list, 'tuple': tuple, 'dict': dict, 'set': set,
            'abs': abs, 'max': max, 'min': min, 'sum': sum,
            'repr': repr, 'type': type, 'isinstance': isinstance,
            'issubclass': issubclass, 'hasattr': hasattr,
            'getattr': getattr, 'setattr': setattr, 'delattr': delattr,
            'callable': callable, 'iter': iter, 'next': next,
            'enumerate': enumerate, 'zip': zip, 'map': map, 'filter': filter,
            'sorted': sorted, 'reversed': reversed,
            'open': open, 'input': input,
            'id': id, 'hash': hash, 'hex': hex, 'oct': oct, 'bin': bin,
            'chr': chr, 'ord': ord, 'round': round, 'pow': pow,
            'divmod': divmod, 'format': format, 'vars': vars, 'dir': dir,
            'any': any, 'all': all, 'object': object, 'super': super,
            'property': property, 'staticmethod': staticmethod,
            'classmethod': classmethod,
            'Exception': Exception, 'ValueError': ValueError,
            'TypeError': TypeError, 'KeyError': KeyError,
            'IndexError': IndexError, 'AttributeError': AttributeError,
            'NameError': NameError, 'RuntimeError': RuntimeError,
            'StopIteration': StopIteration,
            'NotImplementedError': NotImplementedError,
            'OSError': OSError, 'IOError': IOError,
            'FileNotFoundError': FileNotFoundError,
            'ImportError': ImportError, 'AssertionError': AssertionError,
            'ZeroDivisionError': ZeroDivisionError,
            'OverflowError': OverflowError,
            'ArithmeticError': ArithmeticError,
            'LookupError': LookupError,
            'PermissionError': PermissionError,
            'TimeoutError': TimeoutError,
            'SystemExit': SystemExit,                
            'KeyboardInterrupt': KeyboardInterrupt,  
            'True': True, 'False': False, 'None': None,
            '__name__': '__main__', '__file__': '<qd0>',
            '__import__': __import__,
            'sys': ss, 're': re, 'os': os,
            'types': __import__('types'),
            'importlib': importlib,
            'traceback': __import__('traceback'),
        }

        for fname in function_map:
            se = function_map[fname]
            self.globs[fname] = self._make_fn(fname, se[0], se[1])

        for cname in class_map:
            se = class_map[cname]
            self.globs[cname] = self._make_class(cname, se[0], se[1])

        self._skip = set()
        for fname in function_map:
            se = function_map[fname]
            for i in range(se[0], se[1] + 1): self._skip.add(i)
        for cname in class_map:
            se = class_map[cname]
            for i in range(se[0], se[1] + 1): self._skip.add(i)

    def _make_class(self, cname, start_idx, end_idx):
        instructions = self.instructions
        bases = []
        start_check = start_idx - 8
        if start_check < 0: start_check = 0
        for i in range(start_check, start_idx):
            inst = instructions[i]
            if inst[OP] == 'LOAD_NAME' and inst[A1] in self.globs:
                candidate = self.globs[inst[A1]]
                if isinstance(candidate, type) and candidate is not type:
                    bases.append(candidate)
        base_tuple = tuple(bases) if len(bases) > 0 else (object,)
        methods = {}
        for base in reversed(base_tuple):
            for k, v in vars(base).items():
                if not str(k).startswith('__'): methods[k] = v
        cf = Frame(cname, instructions, self.label_map, self.globs)
        idx = start_idx + 1
        while idx <= end_idx:
            inst = instructions[idx]
            op = inst[OP]
            if op == 'FUNCTION':
                fname = inst[RR]
                depth = 1
                j = idx + 1
                searching = True
                while j <= end_idx and searching:
                    if instructions[j][OP] == 'FUNCTION': depth += 1
                    elif instructions[j][OP] == 'FUNCTION_END':
                        depth -= 1
                        if depth == 0: searching = False
                    if searching: j += 1
                methods[fname] = self._make_fn(fname, idx, j)
                idx = j + 1
            else:
                if op not in ('CLASS', 'CLASS_END', 'ENTER_SCOPE', 'EXIT_SCOPE',
                              'PARAM', 'VARARG', 'KWARG', 'LABEL', 'PASS', '#'):
                    try:
                        self._exec(inst, cf)
                        if op == 'STORE' and inst[RR] != '_':
                            if inst[RR] in cf.locs:
                                methods[inst[RR]] = cf.locs[inst[RR]]
                    except Exception:
                        pass
                idx += 1
        return type(cname, base_tuple, methods)

    def _make_fn(self, fname, start_idx, end_idx, captured_env=None):
        vm = self
        def qd_fn(*args, **kwargs):
            return vm._call_fn(fname, start_idx, end_idx, args, captured_env, kwargs)
        qd_fn.__name__ = fname
        return qd_fn

    def _call_fn(self, fname, start_idx, end_idx, args, captured_env=None, kwargs=None):
        frame = Frame(fname, self.instructions, self.label_map, self.globs)
        if captured_env:
            for k, v in captured_env.items(): frame.locs[k] = v
        pc = start_idx + 1
        pi = 0
        instructions = self.instructions
        
        parsing_params = True
        while pc <= end_idx and parsing_params:
            inst = instructions[pc]
            op = inst[OP]
            if op == 'ENTER_SCOPE' or op == 'PASS':
                pc += 1
            elif op == 'PARAM':
                if pi < len(args): frame.locs[inst[RR]] = args[pi]
                else: frame.locs[inst[RR]] = None
                pi += 1
                pc += 1
            elif op == 'VARARG':
                frame.locs[inst[RR]] = tuple(args[pi:])
                pc += 1
            elif op == 'KWARG':
                frame.locs[inst[RR]] = {}
                pc += 1
            else:
                parsing_params = False
                
        if kwargs:
            for k, v in kwargs.items(): frame.locs[k] = v
        frame.pc = pc
        
        inner_skip = set()
        i = start_idx + 1
        while i <= end_idx:
            inst = instructions[i]
            if inst[OP] == 'FUNCTION':
                depth = 1
                j = i + 1
                searching = True
                while j <= end_idx and searching:
                    if instructions[j][OP] == 'FUNCTION': depth += 1
                    elif instructions[j][OP] == 'FUNCTION_END':
                        depth -= 1
                        if depth == 0: searching = False
                    if searching: j += 1
                for k in range(i + 1, j + 1): inner_skip.add(k)
                i = j + 1
            elif inst[OP] == 'CLASS':
                depth = 1
                j = i + 1
                searching = True
                while j <= end_idx and searching:
                    if instructions[j][OP] == 'CLASS': depth += 1
                    elif instructions[j][OP] == 'CLASS_END':
                        depth -= 1
                        if depth == 0: searching = False
                    if searching: j += 1
                for k in range(i + 1, j + 1): inner_skip.add(k)
                i = j + 1
            else:
                i += 1
            
        if len(inner_skip) > 0: return self._run(frame, end_idx, inner_skip)
        return self._run(frame, end_idx)

    def run(self):
        frame = Frame('__main__', self.instructions, self.label_map, self.globs)
        frame.pc = 0
        self._run(frame, len(self.instructions) - 1, self._skip)

    def _run(self, frame, end_idx, skip=None):
        instructions = self.instructions
        label_map = self.label_map
        SKIP_OPS = ('FUNCTION_END', 'ENTER_SCOPE', 'EXIT_SCOPE',
                    'PARAM', 'VARARG', 'KWARG', 'LABEL',
                    'CLASS', 'CLASS_END', 'PASS', '#')
        _exec = self._exec
        
        while frame.pc <= end_idx:
            idx = frame.pc
            if skip and idx in skip:
                frame.pc = idx + 1
                continue
            inst = instructions[idx]
            if inst[OP] in SKIP_OPS:
                frame.pc = idx + 1
                continue
            try:
                ret = _exec(inst, frame)
except SystemExit:
    raise
except Exception as e:
            print(f"EXCEPTION at pc={idx}: {inst} -> {e}")
            if len(frame.try_stack) > 0:
                h = frame.try_stack.pop()
                frame.current_exc = e
                if h in label_map:
                    frame.pc = label_map[h]
                    continue
            raise
            
            if ret is None:
                frame.pc = idx + 1
            elif ret[0] == 'R': 
                return ret[1]
            elif ret[0] == 'J':
                lbl = ret[1]
                if lbl not in label_map:
                    raise RuntimeError('未知標籤: ' + str(lbl))
                frame.pc = label_map[lbl]
            else:
                frame.pc = idx + 1
        return None

    def _exec(self, inst, frame):
        op, a1, a2, r = inst[OP], inst[A1], inst[A2], inst[RR]
        locs, globs, resolve, setv = frame.locs, frame.globs, frame.resolve, frame.setv

        def V(t):
            if t is None or t == '_': return None
            return resolve(t)

        def lit(t):
            if t is None or t == '_': return None
            return parse_value(t)

        if op == 'FUNCTION':
            fname2 = r
            if fname2 in self.function_map:
                se = self.function_map[fname2]
                captured = {}
                for k, v in locs.items():
                    if k not in globs: captured[k] = v
                fn = self._make_fn(fname2, se[0], se[1], captured if len(captured) > 0 else None)
                locs[fname2] = fn
                globs[fname2] = fn
            return None

        elif op == 'LOAD_CONST': setv(r, lit(a1))
        elif op == 'LOAD_NAME':
            if frame.name == '__main__': setv(r, frame.resolve_global_first(a1))
            else: setv(r, resolve(a1))
        elif op == 'LOAD_ATTR':
            obj = V(a1)
            attr = getattr(obj, a2)
            if isinstance(obj, Frame) and hasattr(attr, '__func__'):
                frame_obj = obj
                unbound_method = attr.__func__
                def make_wrapper(frm, meth):
                    def wrapper(*args):
                        return meth(frm, *args)
                    return wrapper
                setv(r, make_wrapper(frame_obj, unbound_method))
            else:
                setv(r, attr)
        elif op == 'STORE':
            val = V(a1)
            write_global = frame.name == '__main__' or (r in globs and r not in locs)
            setv(r, val)
            if write_global: globs[r] = val
        elif op == 'STORE_ATTR': setattr(V(a1), a2, V(r))
        elif op == 'DELETE_NAME':
            if a1 in locs: del locs[a1]
            if a1 in globs: del globs[a1]

        elif op == 'ADD': setv(r, V(a1) + V(a2))
        elif op == 'SUB': setv(r, V(a1) - V(a2))
        elif op == 'MUL': setv(r, V(a1) * V(a2))
        elif op == 'DIV': setv(r, V(a1) / V(a2))
        elif op == 'FLOOR_DIV': setv(r, V(a1) // V(a2))
        elif op == 'MOD': setv(r, V(a1) % V(a2))
        elif op == 'POW': setv(r, V(a1) ** V(a2))
        elif op == 'NEG': setv(r, -V(a1))
        elif op == 'POS': setv(r, +V(a1))
        elif op == 'BIT_AND': setv(r, V(a1) & V(a2))
        elif op == 'BIT_OR': setv(r, V(a1) | V(a2))
        elif op == 'BIT_XOR': setv(r, V(a1) ^ V(a2))
        elif op == 'BIT_NOT': setv(r, ~V(a1))
        elif op == 'LSHIFT': setv(r, V(a1) << V(a2))
        elif op == 'RSHIFT': setv(r, V(a1) >> V(a2))
        elif op == 'BINOP': setv(r, V(a1) + V(a2))

        elif op == 'CMP_EQ': setv(r, V(a1) == V(a2))
        elif op == 'CMP_NE': setv(r, V(a1) != V(a2))
        elif op == 'CMP_LT': setv(r, V(a1) < V(a2))
        elif op == 'CMP_LE': setv(r, V(a1) <= V(a2))
        elif op == 'CMP_GT': setv(r, V(a1) > V(a2))
        elif op == 'CMP_GE': setv(r, V(a1) >= V(a2))
        elif op == 'CMP_IS': setv(r, V(a1) is V(a2))
        elif op == 'CMP_IS_NOT': setv(r, V(a1) is not V(a2))
        elif op == 'CMP_IN': setv(r, V(a1) in V(a2))
        elif op == 'CMP_NOT_IN': setv(r, V(a1) not in V(a2))
        elif op == 'CMP': setv(r, V(a1) > V(a2))

        elif op == 'AND': setv(r, V(a1) and V(a2))
        elif op == 'OR': setv(r, V(a1) or V(a2))
        elif op == 'NOT': setv(r, not V(a1))

        elif op == 'JUMP': return ('J', a1)
        elif op == 'BRANCH_IF_TRUE':
            if V(a1): return ('J', r)
        elif op == 'BRANCH_IF_FALSE':
            if not V(a1): return ('J', r)
        elif op == 'BREAK': return ('J', r)
        elif op == 'CONTINUE': return ('J', r)

        elif op == 'GET_ITER' or op == 'UNPACK_ITER': setv(r, iter(V(a1)))
        elif op == 'ITER_NEXT':
            it = resolve(a1)
            try:
                setv(r, next(it))
                frame.exhaust[a1] = False
            except StopIteration:
                frame.exhaust[a1] = True
                setv(r, None)
        elif op == 'BRANCH_IF_EXHAUST':
            if a1 in frame.exhaust and frame.exhaust[a1]: return ('J', r)

        elif op == 'TUPLE_APPEND': frame.tuple_buf.append((lit(a2), V(a1)))
        elif op == 'BUILD_TUPLE':
            count = int(lit(a1))
            raw = frame.tuple_buf[-count:] if count > 0 else []
            if count > 0: del frame.tuple_buf[-count:]
            for i in range(len(raw)):
                for j in range(0, len(raw) - i - 1):
                    val1 = raw[j][0] if raw[j][0] is not None else 999999
                    val2 = raw[j + 1][0] if raw[j + 1][0] is not None else 999999
                    if val1 > val2:
                        tmp = raw[j]; raw[j] = raw[j + 1]; raw[j + 1] = tmp
            lst = []
            for item in raw: lst.append(item[1])
            setv(r, tuple(lst))
            
        elif op == 'LIST_APPEND': frame.list_buf.append((lit(a2), V(a1)))
        elif op == 'BUILD_LIST':
            count = int(lit(a1))
            raw = frame.list_buf[-count:] if count > 0 else []
            if count > 0: del frame.list_buf[-count:]
            for i in range(len(raw)):
                for j in range(0, len(raw) - i - 1):
                    val1 = raw[j][0] if raw[j][0] is not None else 999999
                    val2 = raw[j + 1][0] if raw[j + 1][0] is not None else 999999
                    if val1 > val2:
                        tmp = raw[j]; raw[j] = raw[j + 1]; raw[j + 1] = tmp
            lst = []
            for item in raw: lst.append(item[1])
            setv(r, lst)
            
        elif op == 'SET_APPEND': frame.set_buf.append(V(a1))
        elif op == 'BUILD_SET':
            count = int(lit(a1))
            items = frame.set_buf[-count:] if count > 0 else []
            if count > 0: del frame.set_buf[-count:]
            setv(r, set(items))
            
        elif op == 'DICT_INSERT': frame.dict_buf.append((V(a1), V(a2)))
        elif op == 'BUILD_DICT':
            count = int(lit(a1))
            items = frame.dict_buf[-count:] if count > 0 else []
            if count > 0: del frame.dict_buf[-count:]
            d = {}
            for kv in items: d[kv[0]] = kv[1]
            setv(r, d)
            
        elif op == 'DICT_UPDATE': pass

        elif op == 'SUBSCRIPT': setv(r, V(a1)[self._key(a2, frame)])
        elif op == 'SUBSCRIPT_SET': V(a1)[self._key(a2, frame)] = V(r)
        elif op == 'SUBSCRIPT_DEL': del V(a1)[self._key(a2, frame)]

        elif op == 'ARG_PUSH': frame.arg_buf.append((lit(a2), V(a1)))
        elif op == 'KWARG_PUSH': frame.kwarg_buf[a2] = V(a1)
        elif op == 'CALL':
            func = V(a1)
            if a2 and a2 != '_': argc = int(lit(a2))
            else: argc = len(frame.arg_buf)
            raw = frame.arg_buf[-argc:] if argc > 0 else []
            if argc > 0: del frame.arg_buf[-argc:]
            
            for i in range(len(raw)):
                for j in range(0, len(raw) - i - 1):
                    val1 = raw[j][0] if raw[j][0] is not None else 999999
                    val2 = raw[j + 1][0] if raw[j + 1][0] is not None else 999999
                    if val1 > val2:
                        tmp = raw[j]; raw[j] = raw[j + 1]; raw[j + 1] = tmp
            args = []
            for item in raw: args.append(item[1])
            
            if len(frame.kwarg_buf) > 0:
                kwargs = {}
                for k, v in frame.kwarg_buf.items(): kwargs[k] = v
                frame.kwarg_buf = {}
                setv(r, func(*args, **kwargs))
            else:
                setv(r, func(*args))
                
        elif op == 'RETURN': return ('R', V(a1))
        elif op == 'MAKE_CLOSURE': setv(r, resolve(a1))

        elif op == 'IMPORT':
            if a1 not in globs:
                try: mod = importlib.import_module(a1)
                except ImportError: mod = None
                globs[a1] = mod
            setv(a1, globs[a1])
        elif op == 'IMPORT_FROM':
            try:
                mod = importlib.import_module(a1)
                val = getattr(mod, a2)
            except Exception: val = None
            globs[a2] = val
            setv(a2, val)
        elif op == 'IMPORT_STAR':
            try:
                mod = importlib.import_module(a1)
                for k in dir(mod):
                    if not str(k).startswith('_'): globs[k] = getattr(mod, k)
            except Exception: pass

        elif op == 'TRY_BEGIN': frame.try_stack.append(a1)
        elif op == 'TRY_END':
            if len(frame.try_stack) > 0: frame.try_stack.pop()
        elif op == 'RAISE':
            exc = V(a1)
            if exc is None: exc = frame.current_exc
            if exc is None: exc = Exception()
            raise exc
        elif op == 'RAISE_REUSE':
            exc = frame.current_exc
            if exc is None: exc = Exception()
            raise exc
        elif op == 'MATCH_EXC':
            et = V(a1)
            if et: setv(r, isinstance(frame.current_exc, et))
            else: setv(r, True)
        elif op == 'EXCEPT_VAR': setv(r, frame.current_exc)

        elif op == 'WITH_ENTER':
            mgr = V(a1)
            val = mgr.__enter__()
            if r and r != '_':
                setv(r, val)
                globs[r] = val
        elif op == 'WITH_EXIT': pass

        elif op == 'ASSERT':
            if not V(a1): raise AssertionError()
        elif op == 'ASSERT_MSG': pass

        elif op == 'TERNARY':
            if V(a1): setv(r, V(a2))
            else: setv(r, None)

        elif op in ('ASSUME_TYPE', 'BOX', 'UNBOX'): setv(r, V(a1))
        return None

    def _key(self, key_str, frame):
        if key_str is None or key_str == '_': return None
        if ':' in str(key_str):
            parts = str(key_str).split(':')
            def p(s):
                if s == '_' or s == '': return None
                return frame.resolve(s)
            start = p(parts[0]) if len(parts) > 0 else None
            stop  = p(parts[1]) if len(parts) > 1 else None
            step  = p(parts[2]) if len(parts) > 2 else None
            return slice(start, stop, step)
        return frame.resolve(key_str)

def main():
    if len(sys.argv) < 2:
        print('用法: python qd0vm.py <file.qd> [args...]')
        sys.exit(1)
    qd_path = sys.argv[1]
    script_argv = sys.argv[1:]
    if not os.path.exists(qd_path):
        print("qd0vm: can't open file '" + qd_path + "'")
        sys.exit(1)
    try:
        instructions, label_map, function_map, class_map = load_program(qd_path)
    except Exception as e:
        print('qd0vm: 載入失敗: ' + str(e))
        sys.exit(1)
        
    vm = QD0VM(instructions, label_map, function_map, class_map, script_argv=script_argv)
    
    try:
        vm.run()
    except SystemExit as e:
        sys.exit(e.code)
    except Exception as e:
        print("\n==========================================")
        print("[QD0 虛擬機] 發生未預期的崩潰:")
        print("  例外類型: " + str(type(e).__name__))
        print("  錯誤訊息: " + str(e))
        print("==========================================\n")
        sys.exit(1)

if __name__ == '__main__':
    main()