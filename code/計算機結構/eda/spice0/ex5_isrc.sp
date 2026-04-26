* 範例 5：電流源 + 電阻網路
* I1(2mA) → a 節點，R1(1kΩ) 並聯 [R2(1kΩ)+R3(1kΩ)]
* R_eq = 1k || 2k = 667Ω
* 預期：
*   V(a)   = 2mA × 667Ω ≈ 1.333V
*   V(mid) = V(a)/2 ≈ 0.667V
*   I(R1)  = V(a)/1k ≈ 1.333mA
*   I(R2)  = V(a)/2k ≈ 0.667mA

Current Source Network

* I1(n1=0, n2=a)：電流從外部 n1→n2，即注入 a 節點
I1   0   a   2m
R1   a   0   1k
R2   a  mid  1k
R3  mid  0   1k

.op
.print V(a) V(mid) I(R1) I(R2)
.end
