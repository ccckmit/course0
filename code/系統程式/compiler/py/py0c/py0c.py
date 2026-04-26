#!/usr/bin/env python3
import ast
import sys
import argparse

class QDCompiler(ast.NodeVisitor):
    def __init__(self):
        self.instructions = []
        self.tmp_count = 0
        self.label_count = 0

    def new_tmp(self):
        name = f"t{self.tmp_count}"
        self.tmp_count += 1
        return name

    def new_label(self, prefix="L"):
        name = f"{prefix}_{self.label_count}"
        self.label_count += 1
        return name

    def emit(self, op, arg1="_", arg2="_", result="_"):
        # 根據規格書，以空白分隔並對齊
        line = f"{op:<16} {str(arg1):<10} {str(arg2):<10} {str(result)}"
        self.instructions.append(line)

    def compile(self, node):
        self.visit(node)
        return "\n".join(self.instructions)

    # --- 模組與基本陳述式 ---
    def visit_Module(self, node):
        for stmt in node.body:
            self.visit(stmt)

    def visit_Expr(self, node):
        self.visit(node.value)

    def visit_Assign(self, node):
        t_val = self.visit(node.value)
        for target in node.targets:
            if isinstance(target, ast.Name):
                self.emit('STORE', t_val, '_', target.id)
            elif isinstance(target, ast.Attribute):
                t_obj = self.visit(target.value)
                self.emit('STORE_ATTR', t_obj, target.attr, t_val)
            elif isinstance(target, ast.Subscript):
                t_obj = self.visit(target.value)
                t_idx = self.visit(target.slice)
                self.emit('STORE_SUBSCRIPT', t_obj, t_idx, t_val) # 擴充指令
            else:
                raise NotImplementedError(f"Unsupported assignment target: {type(target)}")

    def visit_Pass(self, node):
        pass # Do nothing

    def visit_Return(self, node):
        if node.value is not None:
            t_val = self.visit(node.value)
            self.emit('RETURN', t_val, '_', '_')
        else:
            t_none = self.new_tmp()
            self.emit('LOAD_CONST', 'None', '_', t_none)
            self.emit('RETURN', t_none, '_', '_')

    # --- 控制流 (if, while) ---
    def visit_If(self, node):
        t_cond = self.visit(node.test)
        l_else = self.new_label("L_else")
        l_end = self.new_label("L_end")

        self.emit('BRANCH_IF_FALSE', t_cond, '_', l_else)
        
        for stmt in node.body:
            self.visit(stmt)
        self.emit('JUMP', l_end, '_', '_')
        
        self.emit('LABEL', l_else, '_', '_')
        for stmt in node.orelse:
            self.visit(stmt)
            
        self.emit('LABEL', l_end, '_', '_')

    def visit_While(self, node):
        l_start = self.new_label("L_while_start")
        l_end = self.new_label("L_while_end")

        self.emit('LABEL', l_start, '_', '_')
        t_cond = self.visit(node.test)
        self.emit('BRANCH_IF_FALSE', t_cond, '_', l_end)
        
        for stmt in node.body:
            self.visit(stmt)
            
        self.emit('JUMP', l_start, '_', '_')
        self.emit('LABEL', l_end, '_', '_')

    # --- 函式定義 ---
    def visit_FunctionDef(self, node):
        l_func = self.new_label(f"L_func_{node.name}")
        l_end = self.new_label(f"L_end_{node.name}")

        self.emit('JUMP', l_end, '_', '_')
        self.emit('LABEL', l_func, '_', '_')

        # 接收引數 (擴充指令：對稱於 ARG_PUSH 的 ARG_POP)
        for i, arg in enumerate(node.args.args):
            self.emit('ARG_POP', '_', i, arg.arg)

        for stmt in node.body:
            self.visit(stmt)

        # 預設回傳 None
        t_none = self.new_tmp()
        self.emit('LOAD_CONST', 'None', '_', t_none)
        self.emit('RETURN', t_none, '_', '_')

        self.emit('LABEL', l_end, '_', '_')

        # 建立閉包物件並儲存為變數
        t_closure = self.new_tmp()
        self.emit('MAKE_CLOSURE', l_func, '_', t_closure)
        self.emit('STORE', t_closure, '_', node.name)

    # --- 運算式 (Expressions) ---
    def visit_Constant(self, node):
        t_res = self.new_tmp()
        val = node.value
        if isinstance(val, str):
            # 字串加上引號，處理換行
            val_str = '"' + val.replace('\n', '\\n').replace('"', '\\"') + '"'
        elif val is None:
            val_str = 'None'
        elif isinstance(val, bool):
            val_str = 'True' if val else 'False'
        else:
            val_str = str(val)
            
        self.emit('LOAD_CONST', val_str, '_', t_res)
        return t_res

    def visit_Name(self, node):
        if isinstance(node.ctx, ast.Load):
            t_res = self.new_tmp()
            self.emit('LOAD_NAME', node.id, '_', t_res)
            return t_res

    def visit_BinOp(self, node):
        t_left = self.visit(node.left)
        t_right = self.visit(node.right)
        t_res = self.new_tmp()
        
        op_map = {
            ast.Add: 'ADD', ast.Sub: 'SUB', ast.Mult: 'MUL', 
            ast.Div: 'DIV', ast.Mod: 'MOD', ast.FloorDiv: 'FDIV'
        }
        op_type = type(node.op)
        if op_type not in op_map:
            raise NotImplementedError(f"Unsupported binary operator: {op_type}")
            
        self.emit(op_map[op_type], t_left, t_right, t_res)
        return t_res

    def visit_UnaryOp(self, node):
        t_op = self.visit(node.operand)
        t_res = self.new_tmp()
        
        if isinstance(node.op, ast.USub):
            self.emit('NEG', t_op, '_', t_res)
        elif isinstance(node.op, ast.Not):
            self.emit('NOT', t_op, '_', t_res)
        else:
            raise NotImplementedError(f"Unsupported unary operator: {type(node.op)}")
        return t_res

    def visit_Compare(self, node):
        t_left = self.visit(node.left)
        t_right = self.visit(node.comparators[0]) # 假設只支援基本的二元比較
        t_res = self.new_tmp()
        
        op_map = {
            ast.Eq: 'CMP_EQ', ast.NotEq: 'CMP_NE', 
            ast.Lt: 'CMP_LT', ast.LtE: 'CMP_LE', 
            ast.Gt: 'CMP_GT', ast.GtE: 'CMP_GE',
            ast.In: 'CMP_IN'
        }
        op_type = type(node.ops[0])
        if op_type not in op_map:
            raise NotImplementedError(f"Unsupported comparison operator: {op_type}")
            
        self.emit(op_map[op_type], t_left, t_right, t_res)
        return t_res

    def visit_Call(self, node):
        t_func = self.visit(node.func)
        
        # 處理引數
        for i, arg in enumerate(node.args):
            t_arg = self.visit(arg)
            self.emit('ARG_PUSH', t_arg, i, '_')
            
        t_res = self.new_tmp()
        self.emit('CALL', t_func, len(node.args), t_res)
        return t_res

    def visit_List(self, node):
        for i, el in enumerate(node.elts):
            t_el = self.visit(el)
            self.emit('ARG_PUSH', t_el, i, '_')
            
        t_res = self.new_tmp()
        self.emit('BUILD_LIST', len(node.elts), '_', t_res)
        return t_res

    def visit_Attribute(self, node):
        if isinstance(node.ctx, ast.Load):
            t_obj = self.visit(node.value)
            t_res = self.new_tmp()
            self.emit('LOAD_ATTR', t_obj, node.attr, t_res)
            return t_res

    def visit_Subscript(self, node):
        if isinstance(node.ctx, ast.Load):
            t_obj = self.visit(node.value)
            t_idx = self.visit(node.slice)
            t_res = self.new_tmp()
            self.emit('LOAD_SUBSCRIPT', t_obj, t_idx, t_res) # 擴充指令
            return t_res

def main():
    # 使用 argparse 設定命令列參數
    parser = argparse.ArgumentParser(description="Compiler from py0 subset to qd0 IR")
    parser.add_argument("source", help="The source Python file to compile")
    parser.add_argument("-o", "--output", help="The output IR file (e.g. output.qd)", default=None)
    
    args = parser.parse_args()

    # 讀取輸入檔案
    try:
        with open(args.source, "r", encoding="utf-8") as f:
            source_code = f.read()
    except Exception as e:
        print(f"Error reading {args.source}: {e}")
        sys.exit(1)

    # 將 Python 程式碼解析為 AST
    try:
        parsed_ast = ast.parse(source_code, filename=args.source)
    except SyntaxError as e:
        print(f"Syntax Error in {args.source}: {e}")
        sys.exit(1)

    # 執行編譯
    compiler = QDCompiler()
    ir_output = compiler.compile(parsed_ast)

    # 決定輸出位置
    if args.output:
        try:
            with open(args.output, "w", encoding="utf-8") as f:
                f.write(ir_output + "\n")
            print(f"IR successfully written to {args.output}")
        except Exception as e:
            print(f"Error writing to {args.output}: {e}")
            sys.exit(1)
    else:
        # 未提供 -o 參數，直接輸出到終端機
        print(ir_output)

if __name__ == "__main__":
    main()