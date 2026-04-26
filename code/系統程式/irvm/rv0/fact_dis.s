
fact.o:	file format elf64-littleriscv

Disassembly of section .text:

0000000000000000 <factorial>:
; long long factorial(int n) {
       0: fd010113     	addi	sp, sp, -0x30
       4: 02113423     	sd	ra, 0x28(sp)
       8: 02813023     	sd	s0, 0x20(sp)
       c: 03010413     	addi	s0, sp, 0x30
      10: fea42223     	sw	a0, -0x1c(s0)
;     if (n <= 1) {
      14: fe442583     	lw	a1, -0x1c(s0)
      18: 00100513     	li	a0, 0x1
      1c: 00b54a63     	blt	a0, a1, 0x30 <factorial+0x30>
      20: 0040006f     	j	0x24 <factorial+0x24>
      24: 00100513     	li	a0, 0x1
;         return 1;
      28: fea43423     	sd	a0, -0x18(s0)
      2c: 0000006f     	j	0x2c <factorial+0x2c>
;     return n * factorial(n - 1);
      30: fe442503     	lw	a0, -0x1c(s0)
      34: fca43c23     	sd	a0, -0x28(s0)
      38: fff5051b     	addiw	a0, a0, -0x1
      3c: 00000097     	auipc	ra, 0x0
      40: 000080e7     	jalr	ra <factorial+0x3c>
      44: 00050593     	mv	a1, a0
      48: fd843503     	ld	a0, -0x28(s0)
      4c: 02b50533     	mul	a0, a0, a1
      50: fea43423     	sd	a0, -0x18(s0)
      54: 0040006f     	j	0x58 <factorial+0x58>
; }
      58: fe843503     	ld	a0, -0x18(s0)
      5c: 02813083     	ld	ra, 0x28(sp)
      60: 02013403     	ld	s0, 0x20(sp)
      64: 03010113     	addi	sp, sp, 0x30
      68: 00008067     	ret

000000000000006c <main>:
; int main() {
      6c: fe010113     	addi	sp, sp, -0x20
      70: 00113c23     	sd	ra, 0x18(sp)
      74: 00813823     	sd	s0, 0x10(sp)
      78: 02010413     	addi	s0, sp, 0x20
      7c: 00000513     	li	a0, 0x0
      80: fea42623     	sw	a0, -0x14(s0)
      84: 00500513     	li	a0, 0x5
;     int number = 5;
      88: fea42423     	sw	a0, -0x18(s0)
;     long long result = factorial(number);
      8c: fe842503     	lw	a0, -0x18(s0)
      90: 00000097     	auipc	ra, 0x0
      94: 000080e7     	jalr	ra <main+0x24>
      98: fea43023     	sd	a0, -0x20(s0)
;     return result;
      9c: fe042503     	lw	a0, -0x20(s0)
      a0: 01813083     	ld	ra, 0x18(sp)
      a4: 01013403     	ld	s0, 0x10(sp)
      a8: 02010113     	addi	sp, sp, 0x20
      ac: 00008067     	ret
