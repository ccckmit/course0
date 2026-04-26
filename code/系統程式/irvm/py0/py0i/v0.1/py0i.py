import sys
import py0ast as ast

class ReturnException(Exception):
    def __init__(self, value):
        self.value = value

class Interpreter:
    def __init__(self):
        self.env = {}

    def run(self, node):
        return self.visit(node, self.env)

    def visit(self, node, env):
        if hasattr(node, '__class__'):
            method_name = 'visit_' + node.__class__.__name__
            visitor = getattr(self, method_name, self.generic_visit)
            return visitor(node, env)
        return node

    def generic_visit(self, node, env):
        raise NotImplementedError(f"No visit_{node.__class__.__name__} method")

    def visit_Module(self, node, env):
        result = None
        for stmt in node.body:
            result = self.visit(stmt, env)
        return result

    def visit_FunctionDef(self, node, env):
        env[node.name] = node
        return None

    def visit_If(self, node, env):
        if self.visit(node.test, env):
            for stmt in node.body:
                self.visit(stmt, env)
        else:
            for stmt in node.orelse:
                self.visit(stmt, env)

    def visit_Return(self, node, env):
        value = self.visit(node.value, env) if node.value else None
        raise ReturnException(value)

    def visit_Assign(self, node, env):
        value = self.visit(node.value, env)
        for target in node.targets:
            if isinstance(target, ast.Name):
                env[target.id] = value

    def visit_ExprStmt(self, node, env):
        return self.visit(node.value, env)

    def visit_Call(self, node, env):
        func = self.visit(node.func, env)
        args = [self.visit(arg, env) for arg in node.args]

        if func == print:
            print(*args)
            return None
        elif isinstance(func, ast.FunctionDef):
            # Create a new local environment for the function call
            local_env = dict(self.env) # capture globals/functions
            
            # Map passed arguments to the function's parameter names
            for param_name, arg_val in zip(func.args, args):
                local_env[param_name] = arg_val
                
            try:
                for stmt in func.body:
                    self.visit(stmt, local_env)
            except ReturnException as ret:
                return ret.value
            return None
        else:
            raise TypeError(f"Object {func} is not callable")

    def visit_Name(self, node, env):
        if node.id == 'print':
            return print
        if node.id in env:
            return env[node.id]
        raise NameError(f"name '{node.id}' is not defined")

    def visit_Constant(self, node, env):
        return node.value

    def visit_BinOp(self, node, env):
        left = self.visit(node.left, env)
        right = self.visit(node.right, env)
        op = node.op
        
        if op == '+': return left + right
        elif op == '-': return left - right
        elif op == '*': return left * right
        elif op == '/': return left / right
        elif op == '%': return left % right
        elif op == 'and':
            return left and right
        elif op == 'or':
            return left or right
        else:
            raise NotImplementedError(f"Unsupported binary operator: {op}")

    def visit_Compare(self, node, env):
        left = self.visit(node.left, env)
        right = self.visit(node.right, env)
        op = node.op

        if op == '==': return left == right
        elif op == '!=': return left != right
        elif op == '<': return left < right
        elif op == '<=': return left <= right
        elif op == '>': return left > right
        elif op == '>=': return left >= right
        else:
            raise NotImplementedError(f"Unsupported comparison operator: {op}")

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage: python3 py0i.py <script.py>")
        sys.exit(1)

    filename = sys.argv[1]
    with open(filename, 'r') as f:
        source = f.read()

    tree = ast.parse(source)
    interpreter = Interpreter()
    interpreter.run(tree)
