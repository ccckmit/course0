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
    li t0, 12
    sext.w t0, t0
    sd t0, -40(s0)
    ld t0, -40(s0)
    ld t1, -24(s0)
    sd t0, 0(t1)
    addi t0, s0, -56
    sd t0, -48(s0)
    li t0, 10
    sext.w t0, t0
    sd t0, -64(s0)
    ld t0, -64(s0)
    ld t1, -48(s0)
    sd t0, 0(t1)
    addi t0, s0, -80
    sd t0, -72(s0)
    ld t1, -24(s0)
    ld t0, 0(t1)
    sd t0, -88(s0)
    ld t1, -48(s0)
    ld t0, 0(t1)
    sd t0, -96(s0)
    ld t0, -88(s0)
    sext.w t0, t0
    sd t0, -104(s0)
    ld t0, -96(s0)
    sext.w t0, t0
    sd t0, -112(s0)
    ld t0, -104(s0)
    ld t1, -112(s0)
    and t0, t0, t1
    sd t0, -120(s0)
    ld t0, -120(s0)
    ld t1, -72(s0)
    sd t0, 0(t1)
    addi t0, s0, -136
    sd t0, -128(s0)
    ld t1, -24(s0)
    ld t0, 0(t1)
    sd t0, -144(s0)
    ld t1, -48(s0)
    ld t0, 0(t1)
    sd t0, -152(s0)
    ld t0, -144(s0)
    sext.w t0, t0
    sd t0, -160(s0)
    ld t0, -152(s0)
    sext.w t0, t0
    sd t0, -168(s0)
    ld t0, -160(s0)
    ld t1, -168(s0)
    or t0, t0, t1
    sd t0, -176(s0)
    ld t0, -176(s0)
    ld t1, -128(s0)
    sd t0, 0(t1)
    addi t0, s0, -192
    sd t0, -184(s0)
    ld t1, -24(s0)
    ld t0, 0(t1)
    sd t0, -200(s0)
    ld t1, -48(s0)
    ld t0, 0(t1)
    sd t0, -208(s0)
    ld t0, -200(s0)
    sext.w t0, t0
    sd t0, -216(s0)
    ld t0, -208(s0)
    sext.w t0, t0
    sd t0, -224(s0)
    ld t0, -216(s0)
    ld t1, -224(s0)
    xor t0, t0, t1
    sd t0, -232(s0)
    ld t0, -232(s0)
    ld t1, -184(s0)
    sd t0, 0(t1)
    addi t0, s0, -248
    sd t0, -240(s0)
    ld t1, -128(s0)
    ld t0, 0(t1)
    sd t0, -256(s0)
    ld t0, -256(s0)
    sext.w t0, t0
    sd t0, -264(s0)
    li t0, 2
    sext.w t0, t0
    sd t0, -272(s0)
    ld t0, -264(s0)
    ld t1, -272(s0)
    sll t0, t0, t1
    sd t0, -280(s0)
    ld t0, -280(s0)
    ld t1, -240(s0)
    sd t0, 0(t1)
    ld t1, -72(s0)
    ld t0, 0(t1)
    sd t0, -288(s0)
    ld t1, -184(s0)
    ld t0, 0(t1)
    sd t0, -296(s0)
    ld t0, -288(s0)
    sext.w t0, t0
    sd t0, -304(s0)
    ld t0, -296(s0)
    sext.w t0, t0
    sd t0, -312(s0)
    ld t0, -304(s0)
    ld t1, -312(s0)
    add t0, t0, t1
    sd t0, -320(s0)
    ld t1, -240(s0)
    ld t0, 0(t1)
    sd t0, -328(s0)
    ld t0, -328(s0)
    sext.w t0, t0
    sd t0, -336(s0)
    li t0, 10
    sext.w t0, t0
    sd t0, -344(s0)
    ld t0, -336(s0)
    ld t1, -344(s0)
    sub t0, t0, t1
    sd t0, -352(s0)
    ld t0, -320(s0)
    ld t1, -352(s0)
    add t0, t0, t1
    sd t0, -360(s0)
    ld t0, -360(s0)
    sext.w t0, t0
    sd t0, -368(s0)
    ld a0, -368(s0)
    li t2, 0
    sd t2, -376(s0)
    j main_epilogue
.L_main_L0:
    li a0, 0
    li t2, 1
    sd t2, -376(s0)
    j main_epilogue
main_epilogue:
    ld ra, 1016(sp)
    ld s0, 1008(sp)
    addi sp, sp, 1024
    ret
