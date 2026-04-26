請用 C 語言寫一個可以自我編譯的程式，以下是其測試方式，請寫出這五個 c 程式


```markdown
c0c 是一個可以自我編譯的 c 語言編譯器

c0c 的輸入為 C 語言的 .c 程式，輸出為 LLVM IR 的 .ll 檔案
```


test.sh


```shellscript
set -x

make clean
make # 此時會產生 c0c

./c0c -c main.c -o main.ll
./c0c -c macro.c -o macro.ll
./c0c -c parser.c -o parser.ll
./c0c -c codegen.c -o codegen.ll
./c0c -c lexer.c -o lexer.ll

clang lexer.ll codegen.ll parser.ll macro.ll main.ll -o c0c2
./c0c2 -c main.c -o main.ll
...

```


Makefile


```makefile
CC ?= cc
CFLAGS ?= -O2 -Wall -Wextra

SRCS = main.c lexer.c parser.c ast.c codegen.c macro.c
OBJS = $(SRCS:.c=.o)

all: c0c

c0c: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) c0c

.PHONY: all clean

```