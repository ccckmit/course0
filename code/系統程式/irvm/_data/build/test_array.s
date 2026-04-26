.text

.global test_array_1d
test_array_1d:
    addi sp, sp, -1024
    sd ra, 1016(sp)
    sd s0, 1008(sp)
    addi s0, sp, 1024
.L_test_array_1d_entry:
    addi t0, s0, -32
    sd t0, -24(s0)
    ld t1, -24(s0)
    ld t0, 0(t1)
    sd t0, -40(s0)
    li t0, 0
    sext.w t0, t0
    sd t0, -48(s0)
    ld t0, -40(s0)
    ld t1, -48(s0)
    li t2, 8
    mul t1, t1, t2
    add t0, t0, t1
    sd t0, -56(s0)
    li t0, 10
    sext.w t0, t0
    sd t0, -64(s0)
    ld t0, -64(s0)
    ld t1, -56(s0)
    sd t0, 0(t1)
    ld t1, -24(s0)
    ld t0, 0(t1)
    sd t0, -72(s0)
    li t0, 1
    sext.w t0, t0
    sd t0, -80(s0)
    ld t0, -72(s0)
    ld t1, -80(s0)
    li t2, 8
    mul t1, t1, t2
    add t0, t0, t1
    sd t0, -88(s0)
    li t0, 20
    sext.w t0, t0
    sd t0, -96(s0)
    ld t0, -96(s0)
    ld t1, -88(s0)
    sd t0, 0(t1)
    ld t1, -24(s0)
    ld t0, 0(t1)
    sd t0, -104(s0)
    li t0, 2
    sext.w t0, t0
    sd t0, -112(s0)
    ld t0, -104(s0)
    ld t1, -112(s0)
    li t2, 8
    mul t1, t1, t2
    add t0, t0, t1
    sd t0, -120(s0)
    li t0, 30
    sext.w t0, t0
    sd t0, -128(s0)
    ld t0, -128(s0)
    ld t1, -120(s0)
    sd t0, 0(t1)
    ld t1, -24(s0)
    ld t0, 0(t1)
    sd t0, -136(s0)
    li t0, 0
    sext.w t0, t0
    sd t0, -144(s0)
    ld t0, -136(s0)
    ld t1, -144(s0)
    li t2, 8
    mul t1, t1, t2
    add t0, t0, t1
    sd t0, -152(s0)
    ld t1, -152(s0)
    ld t0, 0(t1)
    sd t0, -160(s0)
    ld t1, -24(s0)
    ld t0, 0(t1)
    sd t0, -168(s0)
    li t0, 1
    sext.w t0, t0
    sd t0, -176(s0)
    ld t0, -168(s0)
    ld t1, -176(s0)
    li t2, 8
    mul t1, t1, t2
    add t0, t0, t1
    sd t0, -184(s0)
    ld t1, -184(s0)
    ld t0, 0(t1)
    sd t0, -192(s0)
    ld t0, -160(s0)
    sd t0, -200(s0)
    ld t0, -192(s0)
    sd t0, -208(s0)
    ld t0, -200(s0)
    sd t0, -216(s0)
    ld t0, -216(s0)
    ld t1, -208(s0)
    add t0, t0, t1
    sd t0, -224(s0)
    ld t1, -24(s0)
    ld t0, 0(t1)
    sd t0, -232(s0)
    li t0, 2
    sext.w t0, t0
    sd t0, -240(s0)
    ld t0, -232(s0)
    ld t1, -240(s0)
    li t2, 8
    mul t1, t1, t2
    add t0, t0, t1
    sd t0, -248(s0)
    ld t1, -248(s0)
    ld t0, 0(t1)
    sd t0, -256(s0)
    ld t0, -224(s0)
    sd t0, -264(s0)
    ld t0, -256(s0)
    sd t0, -272(s0)
    ld t0, -264(s0)
    sd t0, -280(s0)
    ld t0, -280(s0)
    ld t1, -272(s0)
    add t0, t0, t1
    sd t0, -288(s0)
    ld t0, -288(s0)
    sd t0, -296(s0)
    ld t0, -296(s0)
    sext.w t0, t0
    sd t0, -304(s0)
    ld a0, -304(s0)
    li t2, 0
    sd t2, -312(s0)
    j test_array_1d_epilogue
.L_test_array_1d_L0:
    li a0, 0
    li t2, 1
    sd t2, -312(s0)
    j test_array_1d_epilogue
test_array_1d_epilogue:
    ld ra, 1016(sp)
    ld s0, 1008(sp)
    addi sp, sp, 1024
    ret

.global main
main:
    addi sp, sp, -1024
    sd ra, 1016(sp)
    sd s0, 1008(sp)
    addi s0, sp, 1024
.L_main_entry:
    call test_array_1d
    sd a0, -24(s0)
    ld t0, -24(s0)
    sext.w t0, t0
    sd t0, -32(s0)
    ld t0, -32(s0)
    sext.w t0, t0
    sd t0, -40(s0)
    ld a0, -40(s0)
    li t2, 0
    sd t2, -48(s0)
    j main_epilogue
.L_main_L0:
    li a0, 0
    li t2, 1
    sd t2, -48(s0)
    j main_epilogue
main_epilogue:
    ld ra, 1016(sp)
    ld s0, 1008(sp)
    addi sp, sp, 1024
    ret
