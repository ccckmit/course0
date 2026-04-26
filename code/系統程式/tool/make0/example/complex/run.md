```
(venv) cccuser@cccimacdeiMac complex % make0                                         
make0:   Building target: makedirs
  $ mkdir -p obj lib bin
make0:     Building target: obj/calc.o
  $ gcc -Wall -Wextra -Iinclude -g -c src/calc.c -o obj/calc.o
make0:     Building target: obj/utils.o
  $ gcc -Wall -Wextra -Iinclude -g -c src/utils.c -o obj/utils.o
make0:   Building target: lib/libcalc.a
  $ ar rcs lib/libcalc.a obj/calc.o obj/utils.o
make0:     Building target: obj/main.o
  $ gcc -Wall -Wextra -Iinclude -g -c src/main.c -o obj/main.o
make0:   Building target: bin/calculator
  $ gcc -Wall -Wextra -Iinclude -g obj/main.o lib/libcalc.a -o bin/calculator
(venv) cccuser@cccimacdeiMac complex % make0 clean
make0: Building target: clean
  $ rm -f obj/*.o lib/libcalc.a bin/calculator
(venv) cccuser@cccimacdeiMac complex % make0 run
make0:       Building target: obj/calc.o
  $ gcc -Wall -Wextra -Iinclude -g -c src/calc.c -o obj/calc.o
make0:       Building target: obj/utils.o
  $ gcc -Wall -Wextra -Iinclude -g -c src/utils.c -o obj/utils.o
make0:     Building target: lib/libcalc.a
  $ ar rcs lib/libcalc.a obj/calc.o obj/utils.o
make0:     Building target: obj/main.o
  $ gcc -Wall -Wextra -Iinclude -g -c src/main.c -o obj/main.o
make0:   Building target: bin/calculator
  $ gcc -Wall -Wextra -Iinclude -g obj/main.o lib/libcalc.a -o bin/calculator
make0: Building target: run
  $ ./bin/calculator
[LOG] Calculator Demo
10 + 5 = 15
10 - 5 = 5
10 * 5 = 50
10 / 5 = 2
```