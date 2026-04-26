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
    li t0, 5
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
    li t0, 0
    sext.w t0, t0
    sd t0, -88(s0)
    ld t0, -88(s0)
    ld t1, -72(s0)
    sd t0, 0(t1)
    ld t1, -24(s0)
    ld t0, 0(t1)
    sd t0, -96(s0)
    ld t1, -48(s0)
    ld t0, 0(t1)
    sd t0, -104(s0)
    ld t0, -96(s0)
    sext.w t0, t0
    sd t0, -112(s0)
    ld t0, -104(s0)
    sext.w t0, t0
    sd t0, -120(s0)
    ld t0, -112(s0)
    ld t1, -120(s0)
    slt t0, t0, t1
    sd t0, -128(s0)
    ld t0, -128(s0)
    andi t0, t0, 1
    sd t0, -136(s0)
    ld t0, -136(s0)
    li t1, 0
    sub t0, t0, t1
    snez t0, t0
    sd t0, -144(s0)
    ld t0, -144(s0)
    li t2, 0
    sd t2, -152(s0)
    bnez t0, .L_main_L0
    j .L_main_L2
.L_main_L0:
    ld t1, -72(s0)
    ld t0, 0(t1)
    sd t0, -160(s0)
    ld t0, -160(s0)
    sext.w t0, t0
    sd t0, -168(s0)
    ld t0, -168(s0)
    li t1, 1
    add t0, t0, t1
    sd t0, -176(s0)
    ld t0, -176(s0)
    ld t1, -72(s0)
    sd t0, 0(t1)
    li t2, 1
    sd t2, -152(s0)
    j .L_main_L2
.L_main_L2:
    ld t1, -24(s0)
    ld t0, 0(t1)
    sd t0, -184(s0)
    ld t1, -48(s0)
    ld t0, 0(t1)
    sd t0, -192(s0)
    ld t0, -184(s0)
    sext.w t0, t0
    sd t0, -200(s0)
    ld t0, -192(s0)
    sext.w t0, t0
    sd t0, -208(s0)
    ld t0, -200(s0)
    ld t1, -208(s0)
    slt t0, t1, t0
    xori t0, t0, 1
    sd t0, -216(s0)
    ld t0, -216(s0)
    andi t0, t0, 1
    sd t0, -224(s0)
    ld t0, -224(s0)
    li t1, 0
    sub t0, t0, t1
    snez t0, t0
    sd t0, -232(s0)
    ld t0, -232(s0)
    li t2, 2
    sd t2, -152(s0)
    bnez t0, .L_main_L3
    j .L_main_L5
.L_main_L3:
    ld t1, -72(s0)
    ld t0, 0(t1)
    sd t0, -240(s0)
    ld t0, -240(s0)
    sext.w t0, t0
    sd t0, -248(s0)
    ld t0, -248(s0)
    li t1, 1
    add t0, t0, t1
    sd t0, -256(s0)
    ld t0, -256(s0)
    ld t1, -72(s0)
    sd t0, 0(t1)
    li t2, 3
    sd t2, -152(s0)
    j .L_main_L5
.L_main_L5:
    ld t1, -48(s0)
    ld t0, 0(t1)
    sd t0, -264(s0)
    ld t1, -24(s0)
    ld t0, 0(t1)
    sd t0, -272(s0)
    ld t0, -264(s0)
    sext.w t0, t0
    sd t0, -280(s0)
    ld t0, -272(s0)
    sext.w t0, t0
    sd t0, -288(s0)
    ld t0, -280(s0)
    ld t1, -288(s0)
    slt t0, t1, t0
    sd t0, -296(s0)
    ld t0, -296(s0)
    andi t0, t0, 1
    sd t0, -304(s0)
    ld t0, -304(s0)
    li t1, 0
    sub t0, t0, t1
    snez t0, t0
    sd t0, -312(s0)
    ld t0, -312(s0)
    li t2, 4
    sd t2, -152(s0)
    bnez t0, .L_main_L6
    j .L_main_L8
.L_main_L6:
    ld t1, -72(s0)
    ld t0, 0(t1)
    sd t0, -320(s0)
    ld t0, -320(s0)
    sext.w t0, t0
    sd t0, -328(s0)
    ld t0, -328(s0)
    li t1, 1
    add t0, t0, t1
    sd t0, -336(s0)
    ld t0, -336(s0)
    ld t1, -72(s0)
    sd t0, 0(t1)
    li t2, 5
    sd t2, -152(s0)
    j .L_main_L8
.L_main_L8:
    ld t1, -48(s0)
    ld t0, 0(t1)
    sd t0, -344(s0)
    ld t1, -24(s0)
    ld t0, 0(t1)
    sd t0, -352(s0)
    ld t0, -344(s0)
    sext.w t0, t0
    sd t0, -360(s0)
    ld t0, -352(s0)
    sext.w t0, t0
    sd t0, -368(s0)
    ld t0, -360(s0)
    ld t1, -368(s0)
    slt t0, t0, t1
    xori t0, t0, 1
    sd t0, -376(s0)
    ld t0, -376(s0)
    andi t0, t0, 1
    sd t0, -384(s0)
    ld t0, -384(s0)
    li t1, 0
    sub t0, t0, t1
    snez t0, t0
    sd t0, -392(s0)
    ld t0, -392(s0)
    li t2, 6
    sd t2, -152(s0)
    bnez t0, .L_main_L9
    j .L_main_L11
.L_main_L9:
    ld t1, -72(s0)
    ld t0, 0(t1)
    sd t0, -400(s0)
    ld t0, -400(s0)
    sext.w t0, t0
    sd t0, -408(s0)
    ld t0, -408(s0)
    li t1, 1
    add t0, t0, t1
    sd t0, -416(s0)
    ld t0, -416(s0)
    ld t1, -72(s0)
    sd t0, 0(t1)
    li t2, 7
    sd t2, -152(s0)
    j .L_main_L11
.L_main_L11:
    ld t1, -24(s0)
    ld t0, 0(t1)
    sd t0, -424(s0)
    ld t1, -48(s0)
    ld t0, 0(t1)
    sd t0, -432(s0)
    ld t0, -424(s0)
    sext.w t0, t0
    sd t0, -440(s0)
    ld t0, -432(s0)
    sext.w t0, t0
    sd t0, -448(s0)
    ld t0, -440(s0)
    ld t1, -448(s0)
    sub t0, t0, t1
    snez t0, t0
    sd t0, -456(s0)
    ld t0, -456(s0)
    andi t0, t0, 1
    sd t0, -464(s0)
    ld t0, -464(s0)
    li t1, 0
    sub t0, t0, t1
    snez t0, t0
    sd t0, -472(s0)
    ld t0, -472(s0)
    li t2, 8
    sd t2, -152(s0)
    bnez t0, .L_main_L12
    j .L_main_L14
.L_main_L12:
    ld t1, -72(s0)
    ld t0, 0(t1)
    sd t0, -480(s0)
    ld t0, -480(s0)
    sext.w t0, t0
    sd t0, -488(s0)
    ld t0, -488(s0)
    li t1, 1
    add t0, t0, t1
    sd t0, -496(s0)
    ld t0, -496(s0)
    ld t1, -72(s0)
    sd t0, 0(t1)
    li t2, 9
    sd t2, -152(s0)
    j .L_main_L14
.L_main_L14:
    ld t1, -24(s0)
    ld t0, 0(t1)
    sd t0, -504(s0)
    ld t0, -504(s0)
    sext.w t0, t0
    sd t0, -512(s0)
    li t0, 5
    sext.w t0, t0
    sd t0, -520(s0)
    ld t0, -512(s0)
    ld t1, -520(s0)
    sub t0, t0, t1
    seqz t0, t0
    sd t0, -528(s0)
    ld t0, -528(s0)
    andi t0, t0, 1
    sd t0, -536(s0)
    ld t0, -536(s0)
    li t1, 0
    sub t0, t0, t1
    snez t0, t0
    sd t0, -544(s0)
    ld t0, -544(s0)
    li t2, 10
    sd t2, -152(s0)
    bnez t0, .L_main_L15
    j .L_main_L17
.L_main_L15:
    ld t1, -72(s0)
    ld t0, 0(t1)
    sd t0, -552(s0)
    ld t0, -552(s0)
    sext.w t0, t0
    sd t0, -560(s0)
    ld t0, -560(s0)
    li t1, 1
    add t0, t0, t1
    sd t0, -568(s0)
    ld t0, -568(s0)
    ld t1, -72(s0)
    sd t0, 0(t1)
    li t2, 11
    sd t2, -152(s0)
    j .L_main_L17
.L_main_L17:
    ld t1, -72(s0)
    ld t0, 0(t1)
    sd t0, -576(s0)
    ld t0, -576(s0)
    sext.w t0, t0
    sd t0, -584(s0)
    ld t0, -584(s0)
    sext.w t0, t0
    sd t0, -592(s0)
    ld a0, -592(s0)
    li t2, 12
    sd t2, -152(s0)
    j main_epilogue
.L_main_L18:
    li a0, 0
    li t2, 13
    sd t2, -152(s0)
    j main_epilogue
main_epilogue:
    ld ra, 1016(sp)
    ld s0, 1008(sp)
    addi sp, sp, 1024
    ret
