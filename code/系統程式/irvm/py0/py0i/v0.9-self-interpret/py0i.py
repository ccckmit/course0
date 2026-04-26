"""
py0i.py - A Python interpreter written in Python.
Usage: python py0i.py <script.py> [args...]

Supports a practical subset of Python sufficient for typical programs
including factorial, fibonacci, sorting, etc.
"""

import sys
import os
import math

# 使用本地手寫的 ast.py 和 operator.py，不依賴標準函式庫
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import ast
import operator

# ─── Environment (scope chain) ───────────────────────────────────────────────

class Environment:
    def __init__(self, parent=None):
        self.vars = {}
        self.parent = parent

    def get(self, name):
        if name in self.vars:
            return self.vars[name]
        if self.parent:
            return self.parent.get(name)
        raise NameError(f"name '{name}' is not defined")

    def set(self, name, value):
        self.vars[name] = value

    def assign(self, name, value):
        """Assign to existing variable in nearest scope that has it, else create locally."""
        if name in self.vars:
            self.vars[name] = value
            return
        if self.parent and self.parent._has(name):
            self.parent.assign(name, value)
            return
        self.vars[name] = value

    def assign_global(self, name, value):
        env = self
        while env.parent:
            env = env.parent
        env.vars[name] = value

    def get_global(self):
        env = self
        while env.parent:
            env = env.parent
        return env

    def _has(self, name):
        if name in self.vars:
            return True
        if self.parent:
            return self.parent._has(name)
        return False

    def force_local(self, name, value):
        self.vars[name] = value


# ─── Special control-flow signals ─────────────────────────────────────────────

class ReturnSignal(Exception):
    def __init__(self, value):
        self.value = value

class BreakSignal(Exception):
    pass

class ContinueSignal(Exception):
    pass


# ─── Callable wrappers ────────────────────────────────────────────────────────

class PyFunction:
    def __init__(self, node, closure_env, interpreter):
        self.node = node          # ast.FunctionDef
        self.closure_env = closure_env
        self.interpreter = interpreter
        self.name = node.name

    def __call__(self, *args, **kwargs):
        interp = self.interpreter
        func_env = Environment(parent=self.closure_env)

        node = self.node
        defaults = [interp.eval_expr(d, self.closure_env) for d in node.args.defaults]
        all_args = node.args.args
        n_defaults = len(defaults)
        n_args = len(all_args)
        default_start = n_args - n_defaults

        # positional args
        for i, arg in enumerate(all_args):
            if i < len(args):
                func_env.force_local(arg.arg, args[i])
            elif arg.arg in kwargs:
                func_env.force_local(arg.arg, kwargs[arg.arg])
            else:
                di = i - default_start
                if di >= 0:
                    func_env.force_local(arg.arg, defaults[di])
                else:
                    raise TypeError(f"{self.name}() missing required argument: '{arg.arg}'")

        # *args
        if node.args.vararg:
            extra = args[len(all_args):]
            func_env.force_local(node.args.vararg.arg, extra)

        # **kwargs
        if node.args.kwarg:
            extra_kw = {k: v for k, v in kwargs.items() if k not in {a.arg for a in all_args}}
            func_env.force_local(node.args.kwarg.arg, extra_kw)

        # globals declared in function
        global_names = set()
        for stmt in ast.walk(node):
            if isinstance(stmt, ast.Global):
                for n in stmt.names:
                    global_names.add(n)

        try:
            for stmt in node.body:
                interp.exec_stmt(stmt, func_env, global_names=global_names)
        except ReturnSignal as r:
            return r.value
        return None

    def __repr__(self):
        return f"<function {self.name}>"


class PyClass:
    def __init__(self, name, bases, namespace):
        self.name = name
        self.bases = bases
        self.namespace = namespace  # dict

    def __call__(self, *args, **kwargs):
        instance = PyInstance(self)
        # call __init__ if present
        init = self._lookup('__init__')
        if init:
            init(instance, *args, **kwargs)
        return instance

    def _lookup(self, name):
        if name in self.namespace:
            return self.namespace[name]
        for base in self.bases:
            if isinstance(base, PyClass):
                found = base._lookup(name)
                if found is not None:
                    return found
        return None

    def __repr__(self):
        return f"<class '{self.name}'>"


class PyInstance:
    def __init__(self, cls):
        self.__dict__['_cls'] = cls
        self.__dict__['_attrs'] = {}

    def __getattr__(self, name):
        attrs = object.__getattribute__(self, '_attrs')
        if name in attrs:
            val = attrs[name]
            return val
        cls = object.__getattribute__(self, '_cls')
        found = cls._lookup(name)
        if found is not None:
            # bind method
            if isinstance(found, PyFunction):
                def bound(*args, **kwargs):
                    return found(self, *args, **kwargs)
                return bound
            return found
        raise AttributeError(f"'{cls.name}' object has no attribute '{name}'")

    def __setattr__(self, name, value):
        self.__dict__['_attrs'][name] = value

    def __repr__(self):
        cls = object.__getattribute__(self, '_cls')
        # check for __repr__
        try:
            r = self.__getattr__('__repr__')
            return r()
        except AttributeError:
            return f"<{cls.name} object>"

    def __str__(self):
        try:
            s = self.__getattr__('__str__')
            return s()
        except AttributeError:
            return self.__repr__()


# ─── Interpreter ──────────────────────────────────────────────────────────────

class Interpreter:
    def __init__(self, script_argv=None):
        self.global_env = Environment()
        self._setup_builtins()
        if script_argv is not None:
            self.global_env.set('__name__', '__main__')
            # expose sys.argv
            import sys as _sys
            self.global_env.set('sys', _sys)
            _sys.argv = script_argv

    # ── Built-ins ─────────────────────────────────────────────────────────────

    def _setup_builtins(self):
        env = self.global_env
        builtins = {
            'print': self._builtin_print,
            'input': input,
            'len': len,
            'range': range,
            'int': int,
            'float': float,
            'str': str,
            'bool': bool,
            'list': list,
            'tuple': tuple,
            'dict': dict,
            'set': set,
            'frozenset': frozenset,
            'type': type,
            'isinstance': isinstance,
            'issubclass': issubclass,
            'abs': abs,
            'max': max,
            'min': min,
            'sum': sum,
            'round': round,
            'sorted': sorted,
            'reversed': lambda x: list(reversed(x)),
            'enumerate': enumerate,
            'zip': zip,
            'map': map,
            'filter': filter,
            'any': any,
            'all': all,
            'chr': chr,
            'ord': ord,
            'hex': hex,
            'oct': oct,
            'bin': bin,
            'id': id,
            'hash': hash,
            'repr': repr,
            'open': open,
            'hasattr': hasattr,
            'getattr': getattr,
            'setattr': setattr,
            'delattr': delattr,
            'callable': callable,
            'vars': vars,
            'dir': dir,
            'iter': iter,
            'next': next,
            'format': format,
            'pow': pow,
            'divmod': divmod,
            'globals': lambda: self.global_env.vars,
            'locals': lambda: self.global_env.vars,
            'exec': lambda code, g=None, l=None: exec(code, g, l),
            'eval': lambda expr, g=None, l=None: eval(expr, g, l),
            'compile': compile,
            'staticmethod': staticmethod,
            'classmethod': classmethod,
            'property': property,
            'super': super,
            'object': object,
            'Exception': Exception,
            'ValueError': ValueError,
            'TypeError': TypeError,
            'NameError': NameError,
            'IndexError': IndexError,
            'KeyError': KeyError,
            'AttributeError': AttributeError,
            'StopIteration': StopIteration,
            'RuntimeError': RuntimeError,
            'NotImplementedError': NotImplementedError,
            'ZeroDivisionError': ZeroDivisionError,
            'OverflowError': OverflowError,
            'AssertionError': AssertionError,
            'OSError': OSError,
            'IOError': IOError,
            'FileNotFoundError': FileNotFoundError,
            'ImportError': ImportError,
            'ModuleNotFoundError': ModuleNotFoundError,
            'True': True,
            'False': False,
            'None': None,
            'NotImplemented': NotImplemented,
            'Ellipsis': ...,
            '__import__': self._builtin_import,
            'math': math,
        }
        for k, v in builtins.items():
            env.set(k, v)

    def _builtin_print(self, *args, **kwargs):
        sep = kwargs.get('sep', ' ')
        end = kwargs.get('end', '\n')
        file = kwargs.get('file', sys.stdout)
        print(*args, sep=sep, end=end, file=file)

    def _builtin_import(self, name, *args, **kwargs):
        return __import__(name, *args, **kwargs)

    # ── Public API ────────────────────────────────────────────────────────────

    def exec_file(self, path):
        with open(path, 'r', encoding='utf-8') as f:
            source = f.read()
        tree = ast.parse(source, filename=path)
        self.exec_module(tree, self.global_env)

    def exec_module(self, tree, env):
        for stmt in tree.body:
            self.exec_stmt(stmt, env)

    # ── Statement execution ───────────────────────────────────────────────────

    def exec_stmt(self, node, env, global_names=None):
        t = type(node)

        if t is ast.Expr:
            self.eval_expr(node.value, env)

        elif t is ast.Assign:
            value = self.eval_expr(node.value, env)
            for target in node.targets:
                self.assign_target(target, value, env, global_names)

        elif t is ast.AugAssign:
            value = self.eval_expr(node.value, env)
            current = self.eval_expr(node.target, env)
            result = self.apply_augop(node.op, current, value)
            self.assign_target(node.target, result, env, global_names)

        elif t is ast.AnnAssign:
            if node.value is not None:
                value = self.eval_expr(node.value, env)
                self.assign_target(node.target, value, env, global_names)

        elif t is ast.If:
            cond = self.eval_expr(node.test, env)
            if cond:
                for s in node.body:
                    self.exec_stmt(s, env, global_names)
            else:
                for s in node.orelse:
                    self.exec_stmt(s, env, global_names)

        elif t is ast.While:
            while self.eval_expr(node.test, env):
                try:
                    for s in node.body:
                        self.exec_stmt(s, env, global_names)
                except BreakSignal:
                    break
                except ContinueSignal:
                    continue
            else:
                for s in node.orelse:
                    self.exec_stmt(s, env, global_names)

        elif t is ast.For:
            iterable = self.eval_expr(node.iter, env)
            broke = False
            for item in iterable:
                self.assign_target(node.target, item, env, global_names)
                try:
                    for s in node.body:
                        self.exec_stmt(s, env, global_names)
                except BreakSignal:
                    broke = True
                    break
                except ContinueSignal:
                    continue
            if not broke:
                for s in node.orelse:
                    self.exec_stmt(s, env, global_names)

        elif t is ast.FunctionDef:
            fn = PyFunction(node, env, self)
            # apply decorators
            decorators = [self.eval_expr(d, env) for d in reversed(node.decorator_list)]
            val = fn
            for d in decorators:
                val = d(val)
            if global_names and node.name in global_names:
                env.assign_global(node.name, val)
            else:
                env.set(node.name, val)

        elif t is ast.AsyncFunctionDef:
            # treat like regular function (no real async)
            fake = ast.FunctionDef(
                name=node.name,
                args=node.args,
                body=node.body,
                decorator_list=node.decorator_list,
                returns=node.returns,
            )
            ast.copy_location(fake, node)
            self.exec_stmt(fake, env, global_names)

        elif t is ast.ClassDef:
            bases = [self.eval_expr(b, env) for b in node.bases]
            class_env = Environment(parent=env)
            for s in node.body:
                self.exec_stmt(s, class_env)
            cls = PyClass(node.name, bases, class_env.vars)
            env.set(node.name, cls)

        elif t is ast.Return:
            value = self.eval_expr(node.value, env) if node.value else None
            raise ReturnSignal(value)

        elif t is ast.Break:
            raise BreakSignal()

        elif t is ast.Continue:
            raise ContinueSignal()

        elif t is ast.Pass:
            pass

        elif t is ast.Global:
            pass  # handled via global_names set

        elif t is ast.Nonlocal:
            pass

        elif t is ast.Delete:
            for target in node.targets:
                self.delete_target(target, env)

        elif t is ast.Import:
            for alias in node.names:
                mod = __import__(alias.name)
                # get deepest module
                parts = alias.name.split('.')
                obj = mod
                for part in parts[1:]:
                    obj = getattr(obj, part)
                name = alias.asname if alias.asname else parts[0]
                env.set(name, __import__(alias.name) if not alias.asname else obj)
                if not alias.asname:
                    env.set(parts[0], mod)

        elif t is ast.ImportFrom:
            mod = __import__(node.module, fromlist=[a.name for a in node.names])
            for alias in node.names:
                if alias.name == '*':
                    for attr in dir(mod):
                        if not attr.startswith('_'):
                            env.set(attr, getattr(mod, attr))
                else:
                    obj = getattr(mod, alias.name)
                    name = alias.asname if alias.asname else alias.name
                    env.set(name, obj)

        elif t is ast.Try:
            try:
                for s in node.body:
                    self.exec_stmt(s, env, global_names)
            except BreakSignal:
                raise
            except ContinueSignal:
                raise
            except ReturnSignal:
                raise
            except Exception as e:
                handled = False
                for handler in node.handlers:
                    if handler.type is None:
                        # bare except
                        if handler.name:
                            env.set(handler.name, e)
                        for s in handler.body:
                            self.exec_stmt(s, env, global_names)
                        handled = True
                        break
                    else:
                        exc_type = self.eval_expr(handler.type, env)
                        if isinstance(e, exc_type):
                            if handler.name:
                                env.set(handler.name, e)
                            for s in handler.body:
                                self.exec_stmt(s, env, global_names)
                            handled = True
                            break
                if not handled:
                    raise
            else:
                for s in node.orelse:
                    self.exec_stmt(s, env, global_names)
            finally:
                for s in node.finalbody:
                    self.exec_stmt(s, env, global_names)

        elif t is ast.With:
            mgr = self.eval_expr(node.items[0].context_expr, env)
            val = mgr.__enter__()
            if node.items[0].optional_vars:
                self.assign_target(node.items[0].optional_vars, val, env, global_names)
            try:
                for s in node.body:
                    self.exec_stmt(s, env, global_names)
            except Exception as e:
                if not mgr.__exit__(type(e), e, None):
                    raise
            else:
                mgr.__exit__(None, None, None)

        elif t is ast.Raise:
            if node.exc:
                exc = self.eval_expr(node.exc, env)
                raise exc
            else:
                raise

        elif t is ast.Assert:
            cond = self.eval_expr(node.test, env)
            if not cond:
                msg = self.eval_expr(node.msg, env) if node.msg else None
                raise AssertionError(msg)

        else:
            raise NotImplementedError(f"Statement not supported: {type(node).__name__}")

    # ── Target assignment ─────────────────────────────────────────────────────

    def assign_target(self, target, value, env, global_names=None):
        t = type(target)
        if t is ast.Name:
            name = target.id
            if global_names and name in global_names:
                env.assign_global(name, value)
            else:
                env.assign(name, value)
        elif t is ast.Tuple or t is ast.List:
            # unpack
            items = list(value)
            # handle starred
            starred_idx = None
            for i, elt in enumerate(target.elts):
                if isinstance(elt, ast.Starred):
                    starred_idx = i
                    break
            if starred_idx is not None:
                n_before = starred_idx
                n_after = len(target.elts) - starred_idx - 1
                before = items[:n_before]
                after = items[len(items)-n_after:] if n_after else []
                middle = items[n_before:len(items)-n_after if n_after else len(items)]
                for elt, val in zip(target.elts[:n_before], before):
                    self.assign_target(elt, val, env, global_names)
                self.assign_target(target.elts[starred_idx].value, middle, env, global_names)
                for elt, val in zip(target.elts[starred_idx+1:], after):
                    self.assign_target(elt, val, env, global_names)
            else:
                if len(items) != len(target.elts):
                    raise ValueError(f"not enough values to unpack (expected {len(target.elts)}, got {len(items)})")
                for elt, val in zip(target.elts, items):
                    self.assign_target(elt, val, env, global_names)
        elif t is ast.Attribute:
            obj = self.eval_expr(target.value, env)
            setattr(obj, target.attr, value)
        elif t is ast.Subscript:
            obj = self.eval_expr(target.value, env)
            key = self.eval_subscript_key(target.slice, env)
            obj[key] = value
        else:
            raise NotImplementedError(f"Assign target not supported: {t.__name__}")

    def delete_target(self, target, env):
        if isinstance(target, ast.Name):
            del env.vars[target.id]
        elif isinstance(target, ast.Subscript):
            obj = self.eval_expr(target.value, env)
            key = self.eval_subscript_key(target.slice, env)
            del obj[key]
        elif isinstance(target, ast.Attribute):
            obj = self.eval_expr(target.value, env)
            delattr(obj, target.attr)

    def apply_augop(self, op, left, right):
        ops = {
            ast.Add: operator.add,
            ast.Sub: operator.sub,
            ast.Mult: operator.mul,
            ast.Div: operator.truediv,
            ast.FloorDiv: operator.floordiv,
            ast.Mod: operator.mod,
            ast.Pow: operator.pow,
            ast.BitAnd: operator.and_,
            ast.BitOr: operator.or_,
            ast.BitXor: operator.xor,
            ast.LShift: operator.lshift,
            ast.RShift: operator.rshift,
            ast.MatMult: operator.matmul,
        }
        return ops[type(op)](left, right)

    # ── Expression evaluation ─────────────────────────────────────────────────

    def eval_expr(self, node, env):
        t = type(node)

        if t is ast.Constant:
            return node.value

        elif t is ast.Name:
            return env.get(node.id)

        elif t is ast.Attribute:
            obj = self.eval_expr(node.value, env)
            return getattr(obj, node.attr)

        elif t is ast.BinOp:
            left = self.eval_expr(node.left, env)
            right = self.eval_expr(node.right, env)
            return self.apply_binop(node.op, left, right)

        elif t is ast.UnaryOp:
            operand = self.eval_expr(node.operand, env)
            return self.apply_unaryop(node.op, operand)

        elif t is ast.BoolOp:
            if type(node.op) is ast.And:
                result = True
                for val in node.values:
                    result = self.eval_expr(val, env)
                    if not result:
                        return result
                return result
            else:  # Or
                result = False
                for val in node.values:
                    result = self.eval_expr(val, env)
                    if result:
                        return result
                return result

        elif t is ast.Compare:
            left = self.eval_expr(node.left, env)
            for op, comparator in zip(node.ops, node.comparators):
                right = self.eval_expr(comparator, env)
                if not self.apply_cmpop(op, left, right):
                    return False
                left = right
            return True

        elif t is ast.Call:
            func = self.eval_expr(node.func, env)
            args = [self.eval_expr(a, env) for a in node.args
                    if not isinstance(a, ast.Starred)]
            # handle *args
            for a in node.args:
                if isinstance(a, ast.Starred):
                    args.extend(self.eval_expr(a.value, env))
            kwargs = {}
            for kw in node.keywords:
                if kw.arg is None:  # **kwargs
                    kwargs.update(self.eval_expr(kw.value, env))
                else:
                    kwargs[kw.arg] = self.eval_expr(kw.value, env)
            return func(*args, **kwargs)

        elif t is ast.IfExp:
            cond = self.eval_expr(node.test, env)
            return self.eval_expr(node.body, env) if cond else self.eval_expr(node.orelse, env)

        elif t is ast.Lambda:
            class LambdaFunc:
                def __init__(lf, node, closure, interp):
                    lf.node = node
                    lf.closure = closure
                    lf.interp = interp
                def __call__(lf, *args, **kwargs):
                    lenv = Environment(parent=lf.closure)
                    for i, arg in enumerate(lf.node.args.args):
                        if i < len(args):
                            lenv.force_local(arg.arg, args[i])
                    return lf.interp.eval_expr(lf.node.body, lenv)
            return LambdaFunc(node, env, self)

        elif t is ast.List:
            return [self.eval_expr(e, env) for e in node.elts]

        elif t is ast.Tuple:
            return tuple(self.eval_expr(e, env) for e in node.elts)

        elif t is ast.Set:
            return {self.eval_expr(e, env) for e in node.elts}

        elif t is ast.Dict:
            d = {}
            for k, v in zip(node.keys, node.values):
                if k is None:  # **spread
                    d.update(self.eval_expr(v, env))
                else:
                    d[self.eval_expr(k, env)] = self.eval_expr(v, env)
            return d

        elif t is ast.Subscript:
            obj = self.eval_expr(node.value, env)
            key = self.eval_subscript_key(node.slice, env)
            return obj[key]

        elif t is ast.Slice:
            start = self.eval_expr(node.lower, env) if node.lower else None
            stop = self.eval_expr(node.upper, env) if node.upper else None
            step = self.eval_expr(node.step, env) if node.step else None
            return slice(start, stop, step)

        elif t is ast.ListComp:
            return self.eval_comprehension(list, node, env)

        elif t is ast.SetComp:
            return self.eval_comprehension(set, node, env)

        elif t is ast.GeneratorExp:
            return self.eval_comprehension(iter, node, env)

        elif t is ast.DictComp:
            return self.eval_dictcomp(node, env)

        elif t is ast.JoinedStr:  # f-string
            parts = []
            for v in node.values:
                if isinstance(v, ast.Constant):
                    parts.append(str(v.value))
                elif isinstance(v, ast.FormattedValue):
                    val = self.eval_expr(v.value, env)
                    if v.format_spec:
                        spec = ''.join(
                            str(c.value) if isinstance(c, ast.Constant)
                            else str(self.eval_expr(c.value, env))
                            for c in v.format_spec.values
                        )
                        parts.append(format(val, spec))
                    else:
                        if v.conversion == ord('r'):
                            parts.append(repr(val))
                        elif v.conversion == ord('a'):
                            parts.append(ascii(val))
                        else:
                            parts.append(str(val))
            return ''.join(parts)

        elif t is ast.Starred:
            return self.eval_expr(node.value, env)

        elif t is ast.NamedExpr:  # walrus operator :=
            value = self.eval_expr(node.value, env)
            env.assign(node.target.id, value)
            return value

        else:
            raise NotImplementedError(f"Expression not supported: {t.__name__}")

    def eval_subscript_key(self, node, env):
        # In Python 3.9+, slice is just the node directly
        if isinstance(node, ast.Slice):
            return self.eval_expr(node, env)
        elif isinstance(node, ast.Tuple):
            return tuple(
                self.eval_expr(e, env) if not isinstance(e, ast.Slice)
                else self.eval_expr(e, env)
                for e in node.elts
            )
        else:
            return self.eval_expr(node, env)

    def eval_comprehension(self, container, node, env):
        results = []
        self._eval_comp_generators(node.generators, 0, env, node.elt, results)
        if container is set:
            return set(results)
        elif container is iter:
            return iter(results)
        return results

    def _eval_comp_generators(self, generators, idx, env, elt, results):
        if idx >= len(generators):
            results.append(self.eval_expr(elt, env))
            return
        gen = generators[idx]
        comp_env = Environment(parent=env)
        iterable = self.eval_expr(gen.iter, env)
        for item in iterable:
            self.assign_target(gen.target, item, comp_env)
            if all(self.eval_expr(cond, comp_env) for cond in gen.ifs):
                self._eval_comp_generators(generators, idx + 1, comp_env, elt, results)

    def eval_dictcomp(self, node, env):
        result = {}
        self._eval_dictcomp_gen(node.generators, 0, env, node, result)
        return result

    def _eval_dictcomp_gen(self, generators, idx, env, node, result):
        if idx >= len(generators):
            k = self.eval_expr(node.key, env)
            v = self.eval_expr(node.value, env)
            result[k] = v
            return
        gen = generators[idx]
        comp_env = Environment(parent=env)
        for item in self.eval_expr(gen.iter, env):
            self.assign_target(gen.target, item, comp_env)
            if all(self.eval_expr(cond, comp_env) for cond in gen.ifs):
                self._eval_dictcomp_gen(generators, idx + 1, comp_env, node, result)

    # ── Operator helpers ──────────────────────────────────────────────────────

    def apply_binop(self, op, left, right):
        ops = {
            ast.Add: operator.add,
            ast.Sub: operator.sub,
            ast.Mult: operator.mul,
            ast.Div: operator.truediv,
            ast.FloorDiv: operator.floordiv,
            ast.Mod: operator.mod,
            ast.Pow: operator.pow,
            ast.BitAnd: operator.and_,
            ast.BitOr: operator.or_,
            ast.BitXor: operator.xor,
            ast.LShift: operator.lshift,
            ast.RShift: operator.rshift,
            ast.MatMult: operator.matmul,
        }
        return ops[type(op)](left, right)

    def apply_unaryop(self, op, operand):
        if isinstance(op, ast.UAdd):  return +operand
        if isinstance(op, ast.USub):  return -operand
        if isinstance(op, ast.Not):   return not operand
        if isinstance(op, ast.Invert): return ~operand
        raise NotImplementedError(f"Unary op: {type(op).__name__}")

    def apply_cmpop(self, op, left, right):
        ops = {
            ast.Eq: operator.eq,
            ast.NotEq: operator.ne,
            ast.Lt: operator.lt,
            ast.LtE: operator.le,
            ast.Gt: operator.gt,
            ast.GtE: operator.ge,
            ast.Is: lambda a, b: a is b,
            ast.IsNot: lambda a, b: a is not b,
            ast.In: lambda a, b: a in b,
            ast.NotIn: lambda a, b: a not in b,
        }
        return ops[type(op)](left, right)


# ─── Entry point ──────────────────────────────────────────────────────────────

def main():
    if len(sys.argv) < 2:
        print("Usage: python py0i.py <script.py> [args...]", file=sys.stderr)
        sys.exit(1)

    script_path = sys.argv[1]
    script_argv = sys.argv[1:]   # script sees its own path as argv[0]

    if not os.path.exists(script_path):
        print(f"py0i: can't open file '{script_path}': [Errno 2] No such file or directory",
              file=sys.stderr)
        sys.exit(1)

    interp = Interpreter(script_argv=script_argv)
    try:
        interp.exec_file(script_path)
    except SystemExit as e:
        sys.exit(e.code)
    except Exception as e:
        import traceback
        traceback.print_exc()
        sys.exit(1)


if __name__ == '__main__':
    main()
