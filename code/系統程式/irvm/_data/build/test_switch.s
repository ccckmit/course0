.text

.global main
main:
    addi sp, sp, -1024
    sd ra, 1016(sp)
    sd s0, 1008(sp)
    addi s0, sp, 1024
.L_main_entry:
    addi t0, s0, -32
    sd t0, -24(s0)
    li t0, 3
    sext.w t0, t0
    sd t0, -40(s0)
    ld t0, -40(s0)
    ld t1, -24(s0)
    sd t0, 0(t1)
    addi t0, s0, -56
    sd t0, -48(s0)
    li t0, 0
    sext.w t0, t0
    sd t0, -64(s0)
    ld t0, -64(s0)
    ld t1, -48(s0)
    sd t0, 0(t1)
    ld t1, -24(s0)
    ld t0, 0(t1)
    sd t0, -72(s0)
    ld t0, -72(s0)
    sext.w t0, t0
    sd t0, -80(s0)
    ld t0, -80(s0)
    li t1, 0
    add t0, t0, t1
    sd t0, -88(s0)
    ld t0, -88(s0)
    li t1, 1
    sub t1, t0, t1
    beqz t1, .L_main_L1
    li t1, 2
    sub t1, t0, t1
    beqz t1, .L_main_L2
    li t1, 3
    sub t1, t0, t1
    beqz t1, .L_main_L3
    j .L_main_L4
.L_main_L1:
    li t0, 1
    sext.w t0, t0
    sd t0, -96(s0)
    ld t0, -96(s0)
    ld t1, -48(s0)
    sd t0, 0(t1)
    li t2, 1
    sd t2, -104(s0)
    j .L_main_L0
.L_main_L5:
    li t2, 2
    sd t2, -104(s0)
    j .L_main_L2
.L_main_L2:
    li t0, 2
    sext.w t0, t0
    sd t0, -112(s0)
    ld t0, -112(s0)
    ld t1, -48(s0)
    sd t0, 0(t1)
    li t2, 3
    sd t2, -104(s0)
    j .L_main_L0
.L_main_L6:
    li t2, 4
    sd t2, -104(s0)
    j .L_main_L3
.L_main_L3:
    li t0, 4
    sext.w t0, t0
    sd t0, -120(s0)
    ld t0, -120(s0)
    ld t1, -48(s0)
    sd t0, 0(t1)
    li t2, 5
    sd t2, -104(s0)
    j .L_main_L0
.L_main_L7:
    li t2, 6
    sd t2, -104(s0)
    j .L_main_L0
.L_main_L4:
    li t0, 9
    sext.w t0, t0
    sd t0, -128(s0)
    ld t0, -128(s0)
    ld t1, -48(s0)
    sd t0, 0(t1)
    li t2, 7
    sd t2, -104(s0)
    j .L_main_L0
.L_main_L8:
    li t2, 8
    sd t2, -104(s0)
    j .L_main_L0
.L_main_L0:
    ld t1, -48(s0)
    ld t0, 0(t1)
    sd t0, -136(s0)
    ld t0, -136(s0)
    sext.w t0, t0
    sd t0, -144(s0)
    ld t0, -144(s0)
    sext.w t0, t0
    sd t0, -152(s0)
    ld a0, -152(s0)
    li t2, 9
    sd t2, -104(s0)
    j main_epilogue
.L_main_L9:
    li a0, 0
    li t2, 10
    sd t2, -104(s0)
    j main_epilogue
main_epilogue:
    ld ra, 1016(sp)
    ld s0, 1008(sp)
    addi sp, sp, 1024
    ret
