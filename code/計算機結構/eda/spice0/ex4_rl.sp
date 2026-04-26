* 範例 4：RL 電路暫態分析
* 電路：V1(12V) → R1(120Ω) → L1(10mH) → GND
* 時間常數 τ = L/R = 10m/120 ≈ 83.3µs
* 預期：I(L1) 從 0 指數上升趨近 100mA，V(vl) 從 12V 衰減至 0

RL Transient Circuit

V1  vcc  0   12
R1  vcc  vl  120
L1  vl   0   10m

.tran 10u 500u
.print V(vl) I(V1)
.end
