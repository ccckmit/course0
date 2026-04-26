```sh
(venv) cccuser@cccimacdeiMac rv0vm % ./rv0objdump -h -d fact.o
Sections:
Idx Name                 Size      VMA       LMA       File off  Algn
  0                      00000000  00000000  00000000  00000000  2**0
  1 .strtab              00000110  00000000  00000000  00001001  2**0
  2 .text                000000b8  00000000  00000000  00000040  2**2
  3 .rela.text           00000078  00000000  00000000  00000a00  2**3
  4 .debug_abbrev        0000006d  00000000  00000000  000000f8  2**0
  5 .debug_info          0000006d  00000000  00000000  00000165  2**0
  6 .rela.debug_info     000000f0  00000000  00000000  00000a78  2**3
  7 .debug_str_offsets   00000030  00000000  00000000  000001d2  2**0
  8 .rela.debug_str_offsets 000000f0  00000000  00000000  00000b68  2**3
  9 .debug_str           0000007e  00000000  00000000  00000202  2**0
 10 .debug_addr          00000018  00000000  00000000  00000280  2**0
 11 .rela.debug_addr     00000030  00000000  00000000  00000c58  2**3
 12 .comment             0000001f  00000000  00000000  00000298  2**0
 13 .note.GNU-stack      00000000  00000000  00000000  000002b7  2**0
 14 .riscv.attributes    00000061  00000000  00000000  000002b7  2**0
 15 .debug_frame         00000078  00000000  00000000  00000318  2**3
 16 .rela.debug_frame    00000120  00000000  00000000  00000c88  2**3
 17 .debug_line          000000c9  00000000  00000000  00000390  2**0
 18 .rela.debug_line     00000258  00000000  00000000  00000da8  2**3
 19 .debug_line_str      00000033  00000000  00000000  00000459  2**0
 20 .llvm_addrsig        00000001  00000000  00000000  00001000  2**0
 21 .symtab              00000570  00000000  00000000  00000490  2**3

Disassembly of section .text:

0000000000000000 <.text>:
                0:      fd010113        addi    sp, sp, -48
                4:      02113423        store   ra, 40(sp)
                8:      02813023        store   s0, 32(sp)
                c:      03010413        addi    s0, sp, 48
               10:      fea42223        sw      a0, -28(s0)
               14:      fe442583        lw      a1, -28(s0)
               18:      00100513        addi    a0, zero, 1
               1c:      00b54a63        branch  a0, a1, .+20
               20:      0040006f        jal     zero, .+4
               24:      00100513        addi    a0, zero, 1
               28:      fea43423        store   a0, -24(s0)
               2c:      0000006f        jal     zero, .+0
               30:      fe442503        lw      a0, -28(s0)
               34:      fca43c23        store   a0, -40(s0)
               38:      fff5051b        .word   0xfff5051b
               3c:      00000097        auipc   ra, 0
               40:      000080e7        jalr    ra, 0(ra)
               44:      00050593        addi    a1, a0, 0
               48:      fd843503        load    a0, -40(s0)
               4c:      02b50533        op      a0, a0, a1
               50:      fea43423        store   a0, -24(s0)
               54:      0040006f        jal     zero, .+4
               58:      fe843503        load    a0, -24(s0)
               5c:      02813083        load    ra, 40(sp)
               60:      02013403        load    s0, 32(sp)
               64:      03010113        addi    sp, sp, 48
               68:      00008067        jalr    zero, 0(ra)
               6c:      fd010113        addi    sp, sp, -48
               70:      02113423        store   ra, 40(sp)
               74:      02813023        store   s0, 32(sp)
               78:      03010413        addi    s0, sp, 48
               7c:      00000513        addi    a0, zero, 0
               80:      fca43c23        store   a0, -40(s0)
               84:      fea42623        sw      a0, -20(s0)
               88:      00500513        addi    a0, zero, 5
               8c:      fea42423        sw      a0, -24(s0)
               90:      fe842503        lw      a0, -24(s0)
               94:      00000097        auipc   ra, 0
               98:      000080e7        jalr    ra, 0(ra)
               9c:      00050593        addi    a1, a0, 0
               a0:      fd843503        load    a0, -40(s0)
               a4:      feb43023        store   a1, -32(s0)
               a8:      02813083        load    ra, 40(sp)
               ac:      02013403        load    s0, 32(sp)
               b0:      03010113        addi    sp, sp, 48
               b4:      00008067        jalr    zero, 0(ra)
```