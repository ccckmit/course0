* 範例 2：RC 充電電路 — .tran 暫態分析
* 電路：V1(5V) → R1(1kΩ) → C1(1µF) → GND
* 時間常數 τ = RC = 1ms
* 預期：V(vc) 從 0V 指數上升趨近 5V

RC Charging Circuit

V1  vcc  0   5
R1  vcc  vc  1k
C1  vc   0   1u

.tran 0.2m 5m
.print V(vc) V(vcc) I(V1)
.end
