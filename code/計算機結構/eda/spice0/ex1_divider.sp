* 範例 1：電壓分壓器 (Voltage Divider) — .op 直流工作點
* 電路：V1(10V) → R1(1kΩ) → R2(1kΩ) → GND
* 預期：V(mid) = 5V，I(V1) = -10mA（電流流入正端）

Voltage Divider Example

V1  in   0   10
R1  in  mid  1k
R2  mid  0   1k

.op
.print V(in) V(mid) I(V1)
.end
