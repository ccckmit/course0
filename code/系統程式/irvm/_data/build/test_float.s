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
    lui t0, 261120
    addi t0, t0, 0
    fmv.w.x ft1, t0
    fcvt.d.s ft1, ft1
    fadd.d ft0, ft0, ft1
    fsd ft0, -40(s0)
    fld ft0, -40(s0)
    ld t1, -24(s0)
    fsd ft0, 0(t1)
    addi t0, s0, -56
    sd t0, -48(s0)
    li t0, 0
    fmv.w.x ft0, t0
    fcvt.d.s ft0, ft0
    li t0, 2
    fcvt.d.w ft1, t0
    fadd.d ft0, ft0, ft1
    fsd ft0, -64(s0)
    fld ft0, -64(s0)
    ld t1, -48(s0)
    fsd ft0, 0(t1)
    addi t0, s0, -80
    sd t0, -72(s0)
    ld t1, -24(s0)
    fld ft0, 0(t1)
    fsd ft0, -88(s0)
    ld t1, -48(s0)
    fld ft0, 0(t1)
    fsd ft0, -96(s0)
    fld ft0, -88(s0)
    fld ft1, -96(s0)
    fadd.d ft0, ft0, ft1
    fsd ft0, -104(s0)
    fld ft0, -104(s0)
    ld t1, -72(s0)
    fsd ft0, 0(t1)
    addi t0, s0, -120
    sd t0, -112(s0)
    ld t1, -24(s0)
    fld ft0, 0(t1)
    fsd ft0, -128(s0)
    ld t1, -48(s0)
    fld ft0, 0(t1)
    fsd ft0, -136(s0)
    fld ft0, -128(s0)
    fld ft1, -136(s0)
    fmul.d ft0, ft0, ft1
    fsd ft0, -144(s0)
    fld ft0, -144(s0)
    ld t1, -112(s0)
    fsd ft0, 0(t1)
    addi t0, s0, -160
    sd t0, -152(s0)
    ld t1, -48(s0)
    fld ft0, 0(t1)
    fsd ft0, -168(s0)
    ld t1, -24(s0)
    fld ft0, 0(t1)
    fsd ft0, -176(s0)
    fld ft0, -168(s0)
    fld ft1, -176(s0)
    fsub.d ft0, ft0, ft1
    fsd ft0, -184(s0)
    fld ft0, -184(s0)
    ld t1, -152(s0)
    fsd ft0, 0(t1)
    addi t0, s0, -200
    sd t0, -192(s0)
    ld t1, -72(s0)
    fld ft0, 0(t1)
    fsd ft0, -208(s0)
    fld ft0, -208(s0)
    fld ft1, -216(s0)
    flt.d t0, ft1, ft0
    sd t0, -224(s0)
    ld t0, -224(s0)
    andi t0, t0, 1
    sd t0, -232(s0)
    ld t0, -232(s0)
    li t1, 0
    sub t0, t0, t1
    snez t0, t0
    sd t0, -240(s0)
    ld t0, -240(s0)
    li t2, 0
    sd t2, -248(s0)
    bnez t0, .L_main_L0
    j .L_main_L1
.L_main_L0:
    li t0, 1
    sext.w t0, t0
    sd t0, -256(s0)
    li t2, 1
    sd t2, -248(s0)
    j .L_main_L2
.L_main_L1:
    li t0, 0
    sext.w t0, t0
    sd t0, -264(s0)
    li t2, 2
    sd t2, -248(s0)
    j .L_main_L2
.L_main_L2:
    ld t2, -248(s0)
    li t3, 1
    bne t2, t3, .L_main_L2_phi_else_0
    ld t0, -256(s0)
    j .L_main_L2_phi_end_0
.L_main_L2_phi_else_0:
    ld t0, -264(s0)
.L_main_L2_phi_end_0:
    sd t0, -272(s0)
    ld t0, -272(s0)
    ld t1, -192(s0)
    sd t0, 0(t1)
    ld t1, -192(s0)
    ld t0, 0(t1)
    sd t0, -280(s0)
    ld t0, -280(s0)
    sext.w t0, t0
    sd t0, -288(s0)
    ld t0, -288(s0)
    sext.w t0, t0
    sd t0, -296(s0)
    ld a0, -296(s0)
    li t2, 3
    sd t2, -248(s0)
    j main_epilogue
.L_main_L3:
    li a0, 0
    li t2, 4
    sd t2, -248(s0)
    j main_epilogue
main_epilogue:
    ld ra, 1016(sp)
    ld s0, 1008(sp)
    addi sp, sp, 1024
    ret
