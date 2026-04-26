* 範例 3：直流掃描 (DC Sweep)
* 電路：V1 → R1(100Ω) → R2(100Ω) → GND
* 掃描 V1 從 0V 到 5V，步進 1V
* 預期：V(mid) = V1/2，I(V1) = -V1/200

DC Sweep Example

V1  in   0   0
R1  in  mid  100
R2  mid  0   100

.dc V1 0 5 1
.print V(in) V(mid) I(V1)
.end
