import re
import sys

def convert_makefile(input_path, output_path):
    # 定義轉換規則 (正則表達式: 替換字串)
    rules = [
        # 1. 編譯器/連結器替換
        (re.compile(r'^\s*CC\s*[?+:]?=\s*gcc', re.MULTILINE), 'CC = cl'),
        (re.compile(r'^\s*CXX\s*[?+:]?=\s*g\+\+', re.MULTILINE), 'CXX = cl'),
        (re.compile(r'^\s*LD\s*[?+:]?=\s*ld', re.MULTILINE), 'LD = link'),
        
        # 2. 編譯選項替換
        (re.compile(r'-Wall'), '/W3'),          # 警告等級
        (re.compile(r'-Werror'), '/WX'),        # 警告視為錯誤
        (re.compile(r'-g\b'), '/Zi'),           # 偵錯資訊
        (re.compile(r'-O([0-3s])'), r'/O\1'),  # 最佳化 (保留 -O0~-O3)
        (re.compile(r'-c\b'), '/c'),            # 僅編譯不連結
        (re.compile(r'-I(\S+)'), r'/I\1'),      # 標頭檔路徑
        (re.compile(r'-L(\S+)'), r'/LIBPATH:\1'),# 函式庫路徑
        (re.compile(r'-l(\S+)'), r'\1.lib'),    # 連結函式庫 (e.g., -lm → m.lib)
        
        # 3. 輸出選項替換
        (re.compile(r'-o\s+(\S+)'), r'/Fe:\1'), # 輸出檔案名
        
        # 4. 檔案副檔名替換 (.o → .obj)
        (re.compile(r'(\S+)\.o\b'), r'\1.obj'),
        
        # 5. 自動變數替換
        (re.compile(r'\$\^'), '$**'),            # 所有依賴檔 ($^ → $**)
        (re.compile(r'\$<'), '$<'),              # 第一個依賴檔 (保留)
        (re.compile(r'\$@'), '$@'),              # 目標檔 (保留)
        
        # 6. 清理命令替換 (rm → del)
        (re.compile(r'^\s*rm\s+-rf\s+', re.MULTILINE), 'rd /s /q '),
        (re.compile(r'^\s*rm\s+-f\s+', re.MULTILINE), 'del '),
        (re.compile(r'^\s*rm\s+', re.MULTILINE), 'del '),
        
        # 7. 路徑分隔符替換 (僅限非選項的 / → \)
        (re.compile(r'(?<!/)\b(\S+?)/(\S+?)\b(?!/)'), r'\1\\\2'),
    ]

    # 讀取輸入檔案
    try:
        with open(input_path, 'r', encoding='utf-8') as f:
            content = f.read()
    except FileNotFoundError:
        print(f"錯誤：找不到檔案 {input_path}")
        sys.exit(1)

    # 套用所有轉換規則
    for pattern, repl in rules:
        content = pattern.sub(repl, content)

    # 寫入輸出檔案
    with open(output_path, 'w', encoding='utf-8') as f:
        f.write(content)
    
    print(f"轉換完成！輸出檔案：{output_path}")

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("用法：python make2ms.py <輸入GCC_Makefile> <輸出NMAKE_Makefile>")
        print("範例：python make2ms.py Makefile Makefile.nmake")
        sys.exit(1)
    
    input_file = sys.argv[1]
    output_file = sys.argv[2]
    convert_makefile(input_file, output_file)
