"""
py0c.py - A Python to Quadruple compiler.
Usage: python py0c.py <script.py> [-o output.qd]

Compiles Python source code to QD (quadruple) IR format.
"""

import sys
import os

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import ast
import operator

class QuadEmitter:
    def __init__(self):
        self.output = []
        self.temp_counter = 0
        self.label_counter = 0
        self.var_counter = 0
    
    def new_temp(self):
        t = f"t{self.temp_counter}"
        self.temp_counter += 1
        return t
    
    def new_label(self, prefix="L"):
        l = f"{prefix}{self.label_counter}"
        self.label_counter += 1
        return l
    
    def emit(self, op, arg1="_", arg2="_", result="_", comment=""):
        if comment:
            comment = f"  ; {comment}"
        self.output.append(f"{op:16}{arg1:12}{arg2:12}{result:12}{comment}")
    
    def emit_label(self, label):
        self.output.append(f"{label}:")
    
    def emit_blank(self):
        self.output.append("")


class Compiler:
    def __init__(self):
        self.emitter = QuadEmitter()
        self.globals = set()
        self.functions = {}
        self.classes = {}
        self.current_scope = None
    
    def compile(self, source, filename="<input>"):
        tree = ast.parse(source, filename=filename)
        self.compile_module(tree)
        return "\n".join(self.emitter.output)
    
    def compile_module(self, tree):
        for stmt in tree.body:
            self.compile_stmt(stmt, is_top_level=True)
    
    def compile_stmt(self, node, is_top_level=False, local_vars=None):
        if local_vars is None:
            local_vars = set()
        
        t = type(node)
        
        if t is ast.Expr:
            self.compile_expr(node.value)
        
        elif t is ast.Assign:
            src = self.compile_expr(node.value)
            for target in node.targets:
                self.compile_assign(target, src, local_vars)
        
        elif t is ast.AnnAssign:
            if node.value:
                src = self.compile_expr(node.value)
                self.compile_assign(node.target, src, local_vars)
        
        elif t is ast.If:
            self.compile_if(node, local_vars)
        
        elif t is ast.While:
            self.compile_while(node, local_vars)
        
        elif t is ast.For:
            self.compile_for(node, local_vars)
        
        elif t is ast.FunctionDef:
            self.compile_function_def(node, local_vars)
        
        elif t is ast.ClassDef:
            self.compile_class_def(node, local_vars)
        
        elif t is ast.Return:
            if node.value:
                ret = self.compile_expr(node.value)
                self.emitter.emit("RETURN", ret, "_", "_", "return value")
            else:
                self.emitter.emit("RETURN", "_", "_", "_", "return None")
        
        elif t is ast.Break:
            self.emitter.emit("BREAK", "_", "_", "_")
        
        elif t is ast.Continue:
            self.emitter.emit("CONTINUE", "_", "_", "_")
        
        elif t is ast.Pass:
            self.emitter.emit("PASS", "_", "_", "_")
        
        elif t is ast.Global:
            for name in node.names:
                local_vars.add(name)
                self.globals.add(name)
        
        elif t is ast.Nonlocal:
            pass
        
        elif t is ast.Delete:
            for target in node.targets:
                if isinstance(target, ast.Name):
                    self.emitter.emit("DELETE_NAME", target.id, "_", "_")
        
        elif t is ast.Import:
            self.compile_import(node)
        
        elif t is ast.ImportFrom:
            self.compile_import_from(node)
        
        elif t is ast.Try:
            self.compile_try(node, local_vars)
        
        elif t is ast.With:
            self.compile_with(node, local_vars)
        
        elif t is ast.Raise:
            if node.exc:
                exc = self.compile_expr(node.exc)
                self.emitter.emit("RAISE", exc, "_", "_")
            else:
                self.emitter.emit("RAISE_REUSE", "_", "_", "_")
        
        elif t is ast.Assert:
            self.compile_assert(node)
        
        else:
            self.emitter.emit("#", "_", "_", "_", f"Unsupported: {t.__name__}")
    
    def compile_expr(self, node):
        t = type(node)
        
        if t is ast.Constant:
            return self._compile_constant(node)
        
        elif t is ast.Name:
            return self._compile_name(node)
        
        elif t is ast.Attribute:
            return self._compile_attribute(node)
        
        elif t is ast.BinOp:
            return self._compile_binop(node)
        
        elif t is ast.UnaryOp:
            return self._compile_unaryop(node)
        
        elif t is ast.BoolOp:
            return self._compile_boolop(node)
        
        elif t is ast.Compare:
            return self._compile_compare(node)
        
        elif t is ast.Call:
            return self._compile_call(node)
        
        elif t is ast.IfExp:
            return self._compile_ternary(node)
        
        elif t is ast.Lambda:
            return self._compile_lambda(node)
        
        elif t is ast.List:
            return self._compile_list(node)
        
        elif t is ast.Tuple:
            return self._compile_tuple(node)
        
        elif t is ast.Set:
            return self._compile_set(node)
        
        elif t is ast.Dict:
            return self._compile_dict(node)
        
        elif t is ast.Subscript:
            return self._compile_subscript(node)
        
        elif t is ast.Slice:
            return self._compile_slice(node)
        
        elif t is ast.ListComp:
            return self._compile_listcomp(node)
        
        elif t is ast.SetComp:
            return self._compile_setcomp(node)
        
        elif t is ast.DictComp:
            return self._compile_dictcomp(node)
        
        elif t is ast.GeneratorExp:
            return self._compile_generator(node)
        
        elif t is ast.JoinedStr:
            return self._compile_fstring(node)
        
        elif t is ast.Starred:
            return self.compile_expr(node.value)
        
        elif t is ast.NamedExpr:
            return self._compile_named_expr(node)
        
        else:
            dest = self.emitter.new_temp()
            self.emitter.emit("#", "_", "_", dest, f"Expr: {t.__name__}")
            return dest
    
    def _compile_constant(self, node):
        val = node.value
        dest = self.emitter.new_temp()
        if val is None:
            self.emitter.emit("LOAD_CONST", "None", "_", dest, f"load None")
        elif val is True:
            self.emitter.emit("LOAD_CONST", "True", "_", dest, f"load True")
        elif val is False:
            self.emitter.emit("LOAD_CONST", "False", "_", dest, f"load False")
        elif isinstance(val, (int, float)):
            self.emitter.emit("LOAD_CONST", str(val), "_", dest, f"load {val}")
        elif isinstance(val, str):
            self.emitter.emit("LOAD_CONST", repr(val), "_", dest, f"load string")
        else:
            self.emitter.emit("LOAD_CONST", repr(val), "_", dest, f"load constant")
        return dest
    
    def _compile_name(self, node):
        dest = self.emitter.new_temp()
        self.emitter.emit("LOAD_NAME", node.id, "_", dest, f"load {node.id}")
        return dest
    
    def _compile_attribute(self, node):
        obj = self.compile_expr(node.value)
        dest = self.emitter.new_temp()
        self.emitter.emit("LOAD_ATTR", obj, node.attr, dest, f"getattr {node.attr}")
        return dest
    
    def _compile_binop(self, node):
        left = self.compile_expr(node.left)
        right = self.compile_expr(node.right)
        dest = self.emitter.new_temp()
        op = self._binop_op(node.op)
        self.emitter.emit(op, left, right, dest)
        return dest
    
    def _binop_op(self, op):
        op_type = type(op)
        if op_type is ast.Add:
            return "ADD"
        elif op_type is ast.Sub:
            return "SUB"
        elif op_type is ast.Mult:
            return "MUL"
        elif op_type is ast.Div:
            return "DIV"
        elif op_type is ast.FloorDiv:
            return "FLOOR_DIV"
        elif op_type is ast.Mod:
            return "MOD"
        elif op_type is ast.Pow:
            return "POW"
        elif op_type is ast.BitAnd:
            return "BIT_AND"
        elif op_type is ast.BitOr:
            return "BIT_OR"
        elif op_type is ast.BitXor:
            return "BIT_XOR"
        elif op_type is ast.LShift:
            return "LSHIFT"
        elif op_type is ast.RShift:
            return "RSHIFT"
        elif op_type is ast.MatMult:
            return "MATMUL"
        else:
            return "BINOP"
    
    def _compile_unaryop(self, node):
        operand = self.compile_expr(node.operand)
        dest = self.emitter.new_temp()
        op = type(node.op)
        if op is ast.USub:
            self.emitter.emit("NEG", operand, "_", dest)
        elif op is ast.UAdd:
            self.emitter.emit("POS", operand, "_", dest)
        elif op is ast.Not:
            self.emitter.emit("NOT", operand, "_", dest)
        elif op is ast.Invert:
            self.emitter.emit("BIT_NOT", operand, "_", dest)
        return dest
    
    def _compile_boolop(self, node):
        if isinstance(node.op, ast.Or):
            result = self.compile_expr(node.values[0])
            for val in node.values[1:]:
                next_val = self.compile_expr(val)
                dest = self.emitter.new_temp()
                self.emitter.emit("OR", result, next_val, dest)
                result = dest
            return result
        else:
            result = self.compile_expr(node.values[0])
            for val in node.values[1:]:
                next_val = self.compile_expr(val)
                dest = self.emitter.new_temp()
                self.emitter.emit("AND", result, next_val, dest)
                result = dest
            return result
    
    def _compile_compare(self, node):
        left = self.compile_expr(node.left)
        result = None
        for op, comparator in zip(node.ops, node.comparators):
            right = self.compile_expr(comparator)
            dest = self.emitter.new_temp()
            cmp_op = self._cmp_op(op)
            self.emitter.emit(cmp_op, left, right, dest)
            if result is None:
                result = dest
            else:
                new_dest = self.emitter.new_temp()
                self.emitter.emit("AND", result, dest, new_dest)
                result = new_dest
            left = right
        return result
    
    def _cmp_op(self, op):
        op_type = type(op)
        if op_type is ast.Eq:
            return "CMP_EQ"
        elif op_type is ast.NotEq:
            return "CMP_NE"
        elif op_type is ast.Lt:
            return "CMP_LT"
        elif op_type is ast.LtE:
            return "CMP_LE"
        elif op_type is ast.Gt:
            return "CMP_GT"
        elif op_type is ast.GtE:
            return "CMP_GE"
        elif op_type is ast.Is:
            return "CMP_IS"
        elif op_type is ast.IsNot:
            return "CMP_IS_NOT"
        elif op_type is ast.In:
            return "CMP_IN"
        elif op_type is ast.NotIn:
            return "CMP_NOT_IN"
        else:
            return "CMP"
    
    def _compile_call(self, node):
        func = self.compile_expr(node.func)
        args = []
        for a in node.args:
            if isinstance(a, ast.Starred):
                args.extend(self.compile_expr(a.value))
            else:
                args.append(self.compile_expr(a))
        for kw in node.keywords:
            if kw.arg is None:
                args.extend(self.compile_expr(kw.value))
        for i, arg in enumerate(args):
            self.emitter.emit("ARG_PUSH", arg, str(i), "_")
        argc = len(args)
        dest = self.emitter.new_temp()
        self.emitter.emit("CALL", func, str(argc), dest)
        return dest
    
    def _compile_ternary(self, node):
        test = self.compile_expr(node.test)
        body = self.compile_expr(node.body)
        orelse = self.compile_expr(node.orelse)
        dest = self.emitter.new_temp()
        self.emitter.emit("TERNARY", test, body, dest, f"test ? body : orelse")
        return dest
    
    def _compile_lambda(self, node):
        dest = self.emitter.new_temp()
        self.emitter.emit("LAMBDA", "_", "_", dest, "anonymous lambda")
        return dest
    
    def _compile_list(self, node):
        elts = []
        for e in node.elts:
            elts.append(self.compile_expr(e))
        count = len(elts)
        for i, e in enumerate(elts):
            self.emitter.emit("LIST_APPEND", e, str(i), "_")
        dest = self.emitter.new_temp()
        self.emitter.emit("BUILD_LIST", str(count), "_", dest)
        return dest
    
    def _compile_tuple(self, node):
        elts = []
        for e in node.elts:
            elts.append(self.compile_expr(e))
        count = len(elts)
        for i, e in enumerate(elts):
            self.emitter.emit("TUPLE_APPEND", e, str(i), "_")
        dest = self.emitter.new_temp()
        self.emitter.emit("BUILD_TUPLE", str(count), "_", dest)
        return dest
    
    def _compile_set(self, node):
        elts = []
        for e in node.elts:
            elts.append(self.compile_expr(e))
        for i, e in enumerate(elts):
            self.emitter.emit("SET_APPEND", e, str(i), "_")
        dest = self.emitter.new_temp()
        self.emitter.emit("BUILD_SET", str(len(elts)), "_", dest)
        return dest
    
    def _compile_dict(self, node):
        for k, v in zip(node.keys, node.values):
            if k is None:
                self.emitter.emit("DICT_UPDATE", "_", "_", "_", "**spread")
                continue
            key = self.compile_expr(k)
            val = self.compile_expr(v)
            self.emitter.emit("DICT_INSERT", key, val, "_")
        dest = self.emitter.new_temp()
        self.emitter.emit("BUILD_DICT", str(len(node.keys)), "_", dest)
        return dest
    
    def _compile_subscript(self, node):
        obj = self.compile_expr(node.value)
        key = self._compile_subscript_key(node.slice)
        dest = self.emitter.new_temp()
        self.emitter.emit("SUBSCRIPT", obj, key, dest)
        return dest
    
    def _compile_subscript_key(self, node):
        if isinstance(node, ast.Slice):
            return self._compile_slice(node)
        elif isinstance(node, ast.Tuple):
            keys = []
            for e in node.elts:
                keys.append(self.compile_expr(e))
            return ",".join(keys)
        else:
            return self.compile_expr(node)
    
    def _compile_slice(self, node):
        parts = []
        if node.lower:
            parts.append(self.compile_expr(node.lower))
        else:
            parts.append("_")
        if node.upper:
            parts.append(self.compile_expr(node.upper))
        else:
            parts.append("_")
        if node.step:
            parts.append(self.compile_expr(node.step))
        else:
            parts.append("_")
        return ":".join(parts)
    
    def _compile_listcomp(self, node):
        dest = self.emitter.new_temp()
        self.emitter.emit("LIST_COMP", "_", "_", dest, "list comprehension")
        return dest
    
    def _compile_setcomp(self, node):
        dest = self.emitter.new_temp()
        self.emitter.emit("SET_COMP", "_", "_", dest, "set comprehension")
        return dest
    
    def _compile_dictcomp(self, node):
        dest = self.emitter.new_temp()
        self.emitter.emit("DICT_COMP", "_", "_", dest, "dict comprehension")
        return dest
    
    def _compile_generator(self, node):
        dest = self.emitter.new_temp()
        self.emitter.emit("GENERATOR", "_", "_", dest, "generator expression")
        return dest
    
    def _compile_fstring(self, node):
        parts = []
        for v in node.values:
            if isinstance(v, ast.Constant):
                parts.append(repr(v.value))
            elif isinstance(v, ast.FormattedValue):
                val = self.compile_expr(v.value)
                parts.append(f"{{{val}}}")
        dest = self.emitter.new_temp()
        self.emitter.emit("FSTRING", "_", "_", dest, f"f-string")
        return dest
    
    def _compile_named_expr(self, node):
        val = self.compile_expr(node.value)
        dest = self.emitter.new_temp()
        self.emitter.emit("STORE", val, "_", node.target.id, f"walrus: {node.target.id}")
        return dest
    
    def compile_assign(self, target, src, local_vars):
        t = type(target)
        if t is ast.Name:
            name = target.id
            self.emitter.emit("STORE", src, "_", name, f"assign {name}")
            local_vars.add(name)
        elif t is ast.Tuple or t is ast.List:
            self.compile_unpack(target, src, local_vars)
        elif t is ast.Attribute:
            obj = self.compile_expr(target.value)
            self.emitter.emit("STORE_ATTR", obj, target.attr, src)
        elif t is ast.Subscript:
            obj = self.compile_expr(target.value)
            key = self._compile_subscript_key(target.slice)
            self.emitter.emit("SUBSCRIPT_SET", obj, key, src)
    
    def compile_unpack(self, target, src, local_vars):
        temp = self.emitter.new_temp()
        self.emitter.emit("UNPACK_ITER", src, str(len(target.elts)), temp)
        for i, elt in enumerate(target.elts):
            if isinstance(elt, ast.Starred):
                self.compile_assign(elt.value, temp, local_vars)
            else:
                t = self.emitter.new_temp()
                self.emitter.emit("ITER_NEXT", temp, str(i), t)
                self.compile_assign(elt, t, local_vars)
    
    def compile_if(self, node, local_vars):
        test = self.compile_expr(node.test)
        else_label = self.emitter.new_label("L_else")
        end_label = self.emitter.new_label("L_end")
        self.emitter.emit("BRANCH_IF_FALSE", test, "_", else_label, "if not test, go to else")
        for s in node.body:
            self.compile_stmt(s, local_vars=local_vars)
        self.emitter.emit("JUMP", end_label, "_", "_", "skip else")
        self.emitter.emit_label(else_label + ":")
        for s in node.orelse:
            self.compile_stmt(s, local_vars=local_vars)
        self.emitter.emit_label(end_label + ":")
    
    def compile_while(self, node, local_vars):
        loop_start = self.emitter.new_label("L_while_start")
        loop_end = self.emitter.new_label("L_while_end")
        self.emitter.emit_label(loop_start + ":")
        test = self.compile_expr(node.test)
        self.emitter.emit("BRANCH_IF_FALSE", test, "_", loop_end, "while test false")
        for s in node.body:
            self.compile_stmt(s, local_vars=local_vars)
        self.emitter.emit("JUMP", loop_start, "_", "_", "loop back")
        self.emitter.emit_label(loop_end + ":")
    
    def compile_for(self, node, local_vars):
        iter_temp = self.compile_expr(node.iter)
        iter_var = self.emitter.new_temp()
        self.emitter.emit("GET_ITER", iter_temp, "_", iter_var)
        loop_start = self.emitter.new_label("L_for_start")
        loop_end = self.emitter.new_label("L_for_end")
        self.emitter.emit_label(loop_start + ":")
        item = self.emitter.new_temp()
        self.emitter.emit("ITER_NEXT", iter_var, "_", item)
        self.emitter.emit("BRANCH_IF_EXHAUST", iter_var, "_", loop_end)
        self.compile_assign(node.target, item, local_vars)
        for s in node.body:
            self.compile_stmt(s, local_vars=local_vars)
        self.emitter.emit("JUMP", loop_start, "_", "_", "loop back")
        self.emitter.emit_label(loop_end + ":")
    
    def compile_function_def(self, node, local_vars):
        func_name = node.name
        self.emitter.emit("FUNCTION", "_", "_", func_name, f"function {func_name}")
        self.emitter.emit("ENTER_SCOPE", "_", "_", "_")
        func_vars = set()
        for arg in node.args.args:
            self.emitter.emit("PARAM", "_", "_", arg.arg)
            func_vars.add(arg.arg)
        if node.args.vararg:
            self.emitter.emit("VARARG", "_", "_", node.args.vararg.arg)
            func_vars.add(node.args.vararg.arg)
        if node.args.kwarg:
            self.emitter.emit("KWARG", "_", "_", node.args.kwarg.arg)
            func_vars.add(node.args.kwarg.arg)
        for stmt in node.body:
            self.compile_stmt(stmt, local_vars=func_vars)
        self.emitter.emit("EXIT_SCOPE", "_", "_", "_")
        self.emitter.emit("FUNCTION_END", "_", "_", func_name)
    
    def compile_class_def(self, node, local_vars):
        class_name = node.name
        bases = []
        for b in node.bases:
            bases.append(self.compile_expr(b))
        self.emitter.emit("CLASS", class_name, "_", "_", f"class {class_name}")
        class_vars = set()
        for stmt in node.body:
            self.compile_stmt(stmt, local_vars=class_vars)
        self.emitter.emit("CLASS_END", "_", "_", class_name)
    
    def compile_import(self, node):
        for alias in node.names:
            self.emitter.emit("IMPORT", alias.name, "_", "_", f"import {alias.name}")
    
    def compile_import_from(self, node):
        for alias in node.names:
            if alias.name == '*':
                self.emitter.emit("IMPORT_STAR", node.module, "_", "_", f"from {node.module} import *")
            else:
                self.emitter.emit("IMPORT_FROM", node.module, alias.name, "_", f"from {node.module} import {alias.name}")
    
    def compile_try(self, node, local_vars):
        try_label = self.emitter.new_label("L_try")
        except_label = self.emitter.new_label("L_except")
        finally_label = self.emitter.new_label("L_finally")
        end_label = self.emitter.new_label("L_end")
        self.emitter.emit("TRY_BEGIN", except_label, "_", "_")
        for stmt in node.body:
            self.compile_stmt(stmt, local_vars=local_vars)
        self.emitter.emit("TRY_END", "_", "_", "_")
        self.emitter.emit("JUMP", end_label, "_", "_", "skip except")
        self.emitter.emit_label(except_label + ":")
        for handler in node.handlers:
            if handler.type:
                exc_type = self.compile_expr(handler.type)
                self.emitter.emit("MATCH_EXC", exc_type, "_", "_", "check exception type")
            if handler.name:
                self.emitter.emit("EXCEPT_VAR", "_", "_", handler.name)
            for stmt in handler.body:
                self.compile_stmt(stmt, local_vars=local_vars)
        self.emitter.emit_label(end_label + ":")
        if node.finalbody:
            self.emitter.emit_label(finally_label + ":")
            for stmt in node.finalbody:
                self.compile_stmt(stmt, local_vars=local_vars)
    
    def compile_with(self, node, local_vars):
        mgr = self.compile_expr(node.items[0].context_expr)
        if node.items[0].optional_vars:
            var = node.items[0].optional_vars.id
            self.emitter.emit("WITH_ENTER", mgr, "_", var)
        else:
            self.emitter.emit("WITH_ENTER", mgr, "_", "_")
        for stmt in node.body:
            self.compile_stmt(stmt, local_vars=local_vars)
        self.emitter.emit("WITH_EXIT", "_", "_", "_")
    
    def compile_assert(self, node):
        test = self.compile_expr(node.test)
        self.emitter.emit("ASSERT", test, "_", "_", "assert test")
        if node.msg:
            msg = self.compile_expr(node.msg)
            self.emitter.emit("ASSERT_MSG", msg, "_", "_")


def main():
    if len(sys.argv) < 2:
        print("Usage: python py0c.py <script.py> [-o output.qd]", file=sys.stderr)
        sys.exit(1)
    
    script_path = sys.argv[1]
    output_path = None
    
    for i, arg in enumerate(sys.argv[1:], 1):
        if arg == "-o" and i + 1 < len(sys.argv):
            output_path = sys.argv[i + 1]
    
    if not os.path.exists(script_path):
        print(f"py0c: can't open file '{script_path}'", file=sys.stderr)
        sys.exit(1)
    
    with open(script_path, 'r', encoding='utf-8') as f:
        source = f.read()
    
    compiler = Compiler()
    result = compiler.compile(source, script_path)
    
    if output_path:
        with open(output_path, 'w', encoding='utf-8') as f:
            f.write(result)
    else:
        print(result)


if __name__ == '__main__':
    main()