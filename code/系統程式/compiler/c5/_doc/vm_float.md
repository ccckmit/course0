# C4 浮點數擴充 VM 指令

## 新增指令集

### 資料傳送指令

| 助記符 | 說明 |
|--------|------|
| FLI | 載入浮點數：`a = *(double*)a` |
| FLD | 載入雙精度：`a = *(double*)a` |
| FST | 儲存浮點數：`*sp++ = a` |
| FSD | 儲存雙精度：`*sp++ = a` |
| PSHF | 推入浮點數：`*--sp = a` |

### 算術指令

| 助記符 | C 對應 | 說明 |
|--------|--------|------|
| FADD | `a = *sp++ + a` | 浮點加法 |
| FSUB | `a = *sp++ - a` | 浮點減法 |
| FMUL | `a = *sp++ * a` | 浮點乘法 |
| FDIV | `a = *sp++ / a` | 浮點除法 |

### 比較指令

| 助記符 | C 對應 | 說明 |
|--------|--------|------|
| FEQ | `*sp++ == a` | 浮點相等 |
| FNE | `*sp++ != a` | 浮點不等 |
| FLT | `*sp++ < a` | 浮點小於 |
| FGT | `*sp++ > a` | 浮點大於 |
| FLE | `*sp++ <= a` | 浮點小於等於 |
| FGE | `*sp++ >= a` | 浮點大於等於 |

### 類型轉換指令

| 助記符 | C 對應 | 說明 |
|--------|--------|------|
| ITF | `(double)a` | 整數轉浮點 |
| FTI | `(long)a` | 浮點轉整數 |
| ITD | `(double)a` | 整數轉雙精度 |
| DTI | `(long)a` | 雙精度轉整數 |

## 擴充 enum 定義

```c
enum {
  ITF, FTI, ITD, DTI
};
```
```

## 執行時期需要的函式庫

```c
// 需要連結 math library (-lm)
#include <math.h>
```

## 程式範例

```c
// C4 浮點數範例
double area(double r) {
    return 3.14159 * r * r;
}

int main() {
    double r = 5.0;
    double a = area(r);
    printf("Area: %f\n", a);
    return 0;
}
```

編譯產生的 bytecode：
```
IMM 5        // r = 5.0
PSH
IMM 314159  // 3.14159 (定點數表示)
ITF          // 轉浮點
PSH
IMM 2        // r 參數
PSH
JSR [area]   // 呼叫 area
...
```