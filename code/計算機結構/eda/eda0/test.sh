#!/bin/bash

# 確保環境有必要的 python 套件
echo "Installing required Python packages..."
pip install sympy matplotlib -q

# 建立輸出資料夾
mkdir -p out/images

echo "==========================================="
echo "Generating Verilog Test Cases..."
echo "==========================================="

# 測試案例 1：出自你附圖的邏輯化簡範例
# x = a'bc + a'bc' => x = a'b
# y = b'c' + ab' + ac (這邊簡化為較易閱讀的形式)
cat << 'EOF' > v/test1.v
module test1(input a, b, c, output x, y);
    // x = (~a & b & c) | (~a & b & ~c) ---> simplifies to ~a & b
    assign x = (~a & b & c) | (~a & b & ~c);
    
    // y = (~b & ~c) | (a & ~b) | (a & c)
    assign y = (~b & ~c) | (a & ~b) | (a & c);
endmodule
EOF

# 測試案例 2：De Morgan 定律與冗餘消除
# z = (a & b) | (a & ~b) => z = a
cat << 'EOF' > v/test2.v
module test2(input a, b, output z);
    // z = (a & b) | (a & ~b) ---> simplifies to a
    assign z = (a & b) | (a & ~b);
endmodule
EOF

# 測試案例 3：XOR 邏輯與多餘項目消除
# out1 = (a | a) & b => a & b
# out2 = a ^ a => 0 (False)
cat << 'EOF' > v/test3.v
module test3(input a, b, output out1, out2);
    // out1 = (a | a) & b ---> simplifies to a & b
    assign out1 = (a | a) & b;
    
    // out2 = a ^ a ---> simplifies to False (0)
    assign out2 = a ^ a;
endmodule
EOF

echo "Test cases generated: test1.v, test2.v, test3.v"
echo ""
echo "==========================================="
echo "Running EDA Pipeline (eda0.py)..."
echo "==========================================="

python3 eda0.py v/test1.v
python3 eda0.py v/test2.v
python3 eda0.py v/test3.v

echo "==========================================="
echo "All done! Please check the 'out/' directory."
echo "You can open out/test1.md, out/test2.md, and out/test3.md in a Markdown viewer."