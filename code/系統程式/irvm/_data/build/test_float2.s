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
    li t0, 0
    fmv.w.x ft0, t0
    fcvt.d.s ft0, ft0
    li t0, 5
    fcvt.d.w ft1, t0
    fadd.d ft0, ft0, ft1
    fsd ft0, -40(s0)
    fld ft0, -40(s0)
    ld t1, -24(s0)
    fsd ft0, 0(t1)
    ld t1, -24(s0)
    fld ft0, 0(t1)
    fsd ft0, -48(s0)
    li t0, 0
    fmv.w.x ft0, t0
    fcvt.d.s ft0, ft0
    li t0, 0
    fcvt.d.w ft1, t0
    fadd.d ft0, ft0, ft1
    fsd ft0, -56(s0)
    fld ft0, -48(s0)
    fld ft1, -56(s0)
    flt.d t0, ft1, ft0
    sd t0, -64(s0)
    ld t0, -64(s0)
    andi t0, t0, 1
    sd t0, -72(s0)
    ld t0, -72(s0)
    li t1, 0
    sub t0, t0, t1
    snez t0, t0
    sd t0, -80(s0)
    ld t0, -80(s0)
    li t2, 0
    sd t2, -88(s0)
    bnez t0, .L_main_L0
    j .L_main_L2
.L_main_L0:
    li t0, 1
    sext.w t0, t0
    sd t0, -96(s0)
    ld t0, -96(s0)
    sext.w t0, t0
    sd t0, -104(s0)
    ld a0, -104(s0)
    li t2, 1
    sd t2, -88(s0)
    j main_epilogue
.L_main_L3:
    li t2, 2
    sd t2, -88(s0)
    j .L_main_L2
.L_main_L2:
    li t0, 0
    sext.w t0, t0
    sd t0, -112(s0)
    ld t0, -112(s0)
    sext.w t0, t0
    sd t0, -120(s0)
    ld a0, -120(s0)
    li t2, 3
    sd t2, -88(s0)
    j main_epilogue
.L_main_L4:
    li a0, 0
    li t2, 4
    sd t2, -88(s0)
    j main_epilogue
main_epilogue:
    ld ra, 1016(sp)
    ld s0, 1008(sp)
    addi sp, sp, 1024
    ret
