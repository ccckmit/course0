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
    li t0, 10
    sext.w t0, t0
    sd t0, -40(s0)
    li t0, 3
    sext.w t0, t0
    sd t0, -48(s0)
    ld t0, -40(s0)
    ld t1, -48(s0)
    div t0, t0, t1
    sd t0, -56(s0)
    ld t0, -56(s0)
    ld t1, -24(s0)
    sd t0, 0(t1)
    ld t1, -24(s0)
    ld t0, 0(t1)
    sd t0, -64(s0)
    ld t0, -64(s0)
    sext.w t0, t0
    sd t0, -72(s0)
    ld t0, -72(s0)
    sext.w t0, t0
    sd t0, -80(s0)
    ld a0, -80(s0)
    li t2, 0
    sd t2, -88(s0)
    j main_epilogue
.L_main_L0:
    li a0, 0
    li t2, 1
    sd t2, -88(s0)
    j main_epilogue
main_epilogue:
    ld ra, 1016(sp)
    ld s0, 1008(sp)
    addi sp, sp, 1024
    ret
