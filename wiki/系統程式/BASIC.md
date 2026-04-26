# BASIC (Beginner's All-purpose Symbolic Instruction Code)

## 概述

BASIC（Beginner's All-purpose Symbolic Instruction Code）是一種為初學者設計的高階程式語言，由 John G. Kemeny 和 Thomas E. Kurtz 於 1964 年在達特茅斯學院開發。BASIC 的設計目標是讓非理科背景的學生也能夠輕鬆學習程式設計，它語法簡單、易於理解，解釋型實現使得使用者可以立即看到程式執行結果。BASIC 是歷史上影響力最大的程式語言之一，它讓數百萬人第一次接觸了程式設計，並催生了個人電腦革命。

BASIC 的設計哲學強調易用性和互動性。解釋型的設計讓使用者可以逐步輸入和執行命令，即時看到結果，這種 immediate feedback 的特性非常適合初學者。BASIC 的語法相對自由，沒有嚴格的格式要求，降低了學習門檻。這種設計理念影響了後來許多腳本語言和互動式環境。

## 歷史背景

1964 年 5 月 1 日，BASIC 在達特茅斯學院的 DTSS（Dartmouth Time Sharing System）上首次運行。最初只有 John Kemeny 和他的團隊使用，但很快就推廣到達特茅斯的所有師生。1970 年代，Steve Wozniak 在 Apple II 上實現了 Integer BASIC，Bill Gates 和 Paul Allen 為 Altair 8800 開發了 Altair BASIC，這些事件標誌著 BASIC 進入個人電腦時代。

1984 年，BASIC 誕生二十周年之際，微軟發布了 GW-BASIC 和 QuickBASIC。1991 年，微軟推出 Visual Basic，將 BASIC 帶入 Windows 圖形化時代，Visual Basic 成為最流行的 Windows 開發工具之一。1998 年，微軟發布 VB.NET，BASIC 進入 .NET 時代。2000 年後，FreeBASIC、BASIC-256 等開源實現繼續保持著 BASIC 的活力。

## 核心特性

### 簡單的語法

```basic
' 列印 Hello World
PRINT "Hello, World!"

' 變數賦值
LET X = 10
LET Y = 20

' 算術運算
LET Z = X + Y
PRINT Z

' 輸入
INPUT "Enter your name: ", NAME$
PRINT "Hello, "; NAME$
```

### 控制結構

```basic
' 條件判斷
IF X > 10 THEN
    PRINT "X is greater than 10"
ELSE
    PRINT "X is 10 or less"
END IF

' 多重條件
IF SCORE >= 90 THEN
    GRADE$ = "A"
ELSE IF SCORE >= 80 THEN
    GRADE$ = "B"
ELSE IF SCORE >= 70 THEN
    GRADE$ = "C"
ELSE
    GRADE$ = "F"
END IF

' FOR 迴圈
FOR I = 1 TO 10
    PRINT I; I * I
NEXT I

' WHILE 迴圈
WHILE I <= 100
    PRINT I
    I = I + 1
WEND

' GOTO (傳統 BASIC)
10 PRINT "Hello"
20 GOTO 10
```

### 陣列

```basic
' 一維陣列
DIM NUMBERS(10)
FOR I = 1 TO 10
    NUMBERS(I) = I * I
NEXT I

' 二維陣列
DIM MATRIX(3, 3)
FOR I = 1 TO 3
    FOR J = 1 TO 3
        MATRIX(I, J) = I * J
    NEXT J, I

' 字串陣列
DIM NAMES$(5)
NAMES$(1) = "Alice"
NAMES$(2) = "Bob"
```

### 子程式

```basic
' 函數
FUNCTION FACTORIAL(N)
    IF N <= 1 THEN
        FACTORIAL = 1
    ELSE
        FACTORIAL = N * FACTORIAL(N - 1)
    END IF
END FUNCTION

' 副程式
SUB GREET(NAME$)
    PRINT "Hello, "; NAME$; "!"
END SUB

' 呼叫
PRINT FACTORIAL(5)
CALL GREET("World")
```

### 檔案操作

```basic
' 寫入檔案
OPEN "OUTPUT.TXT" FOR OUTPUT AS #1
PRINT #1, "Hello, World!"
CLOSE #1

' 讀取檔案
OPEN "INPUT.TXT" FOR INPUT AS #1
DO WHILE NOT EOF(1)
    LINE INPUT #1, TEXT$
    PRINT TEXT$
LOOP
CLOSE #1
```

## 方言與版本

### 傳統 BASIC

Microsoft BASIC（MBASIC）是 DOS 時代的主流，影響了後續所有 BASIC 方言。IBM 的 BASICA 和 GW-BASIC 是其典型實現。這些版本支援行號、有限的功能支援，但奠定了 BASIC 的基本語法。

### QuickBASIC

1985 年發布的 QuickBASIC 是 BASIC 的重大進化。它引入了結構化程式設計（if/then/else、for/next、while/wend），拋棄了行號和 goto，支援模組化程式設計。QuickBASIC 附帶了完整的 IDE，提供了編譯功能，可以生成可執行檔案。

### Visual Basic

1991 年發布的 Visual Basic 是 BASIC 進入圖形化時代的標誌。它引入了表單（Form）設計器、控制項（Button、TextBox、ListBox 等）、事件驅動程式設計模型。VB 大大降低了 Windows 程式設計的門檻，培育了一代 Windows 程式設計師。

```vb
' Visual Basic 示例
Public Class Form1
    Private Sub Button1_Click(sender As Object, e As EventArgs) Handles Button1.Click
        Dim name As String = TextBox1.Text
        MessageBox.Show("Hello, " & name)
    End Sub
End Class
```

### 現代 BASIC

FreeBASIC 是開源的 BASIC 編譯器，支援 Windows、Linux、 DOS，語法相容 QuickBASIC 並進行了擴展。Blitz Basic、Construct 等專注於遊戲開發。 Gambas 是 Linux 上的 Visual Basic 風格環境。

## 經典遊戲示例

```basic
' 井字棋遊戲
10 CLS
20 PRINT "Tic-Tac-Toe"
30 PRINT "Enter position 1-9"
40 DIM BOARD$(9)
50 FOR I = 1 TO 9: BOARD$(I) = STR$(I): NEXT I
60 PLAYER = 1
70 GOSUB 200  ' Display board
80 INPUT "Player "; PLAYER; " choose: ", POS
90 IF BOARD$(POS) = "X" OR BOARD$(POS) = "O" THEN GOTO 80
100 BOARD$(POS) = IIF$(PLAYER = 1, "X", "O")
110 GOSUB 300  ' Check win
120 IF WIN THEN PRINT "Player "; PLAYER; " wins!": END
130 PLAYER = 3 - PLAYER
140 GOTO 70

200 ' Display
210 FOR ROW = 0 TO 2
220   FOR COL = 0 TO 2
230     I = ROW * 3 + COL + 1
240     PRINT BOARD$(I); " ";
250   NEXT COL
260   PRINT
270 NEXT ROW
280 RETURN

300 ' Check win
310 WIN = 0
320 FOR P = 1 TO 2
330   MARK = IIF$(P = 1, "X", "O")
340   ' Check rows and cols
350   FOR I = 1 TO 3
360     IF BOARD$(I) = MARK AND BOARD$(I+3) = MARK AND BOARD$(I+6) = MARK THEN WIN = 1
370     IF BOARD$(I*3-2) = MARK AND BOARD$(I*3-1) = MARK AND BOARD$(I*3) = MARK THEN WIN = 1
380   NEXT I
390   ' Check diagonals
400   IF BOARD$(1) = MARK AND BOARD$(5) = MARK AND BOARD$(9) = MARK THEN WIN = 1
410   IF BOARD$(3) = MARK AND BOARD$(5) = MARK AND BOARD$(7) = MARK THEN WIN = 1
420 NEXT P
430 RETURN
```

## 遺產與影響

BASIC 對計算機科學的影響是深遠的。作為第一個被廣泛使用的解釋型語言，BASIC 開創了互動式程式設計的先河。它的設計理念影響了 Python、Ruby 等現代腳本語言。BASIC 還催生了整個個人電腦軟體產業，Bill Gates 正是從 BASIC 開始了他的軟體生涯。

BASIC 在教育領域留下了深刻印記。1970-80 年代，無數人通過 BASIC 學習程式設計，程式設計不再是少數專家的專利。這種「民主化」對資訊產業的發展產生了不可估量的影響。

## 相關概念

- [軟體生命週期](軟體生命週期.md) - 軟體開發方法
- [直譯器](直譯器.md) - 解釋型語言實現

## Tags

#BASIC #VisualBasic #程式語言 #個人電腦 #教育