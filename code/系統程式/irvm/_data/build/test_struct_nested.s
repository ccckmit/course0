.text

.global test_struct_nested
test_struct_nested:
    addi sp, sp, -1024
    sd ra, 1016(sp)
    sd s0, 1008(sp)
    addi s0, sp, 1024
.L_test_struct_nested_entry:
    addi t0, s0, -32
    sd t0, -24(s0)
    ld t0, -24(s0)
    li t1, 0
    add t0, t0, t1
    sd t0, -40(s0)
    ld t0, -40(s0)
    li t1, 0
    add t0, t0, t1
    sd t0, -48(s0)
    li t0, 1
    sext.w t0, t0
    sd t0, -56(s0)
    ld t0, -56(s0)
    ld t1, -48(s0)
    sd t0, 0(t1)
    ld t0, -24(s0)
    li t1, 0
    add t0, t0, t1
    sd t0, -64(s0)
    ld t0, -64(s0)
    li t1, 0
    add t0, t0, t1
    sd t0, -72(s0)
    li t0, 2
    sext.w t0, t0
    sd t0, -80(s0)
    ld t0, -80(s0)
    ld t1, -72(s0)
    sd t0, 0(t1)
    ld t0, -24(s0)
    li t1, 0
    add t0, t0, t1
    sd t0, -88(s0)
    ld t0, -88(s0)
    li t1, 0
    add t0, t0, t1
    sd t0, -96(s0)
    li t0, 100
    sext.w t0, t0
    sd t0, -104(s0)
    ld t0, -104(s0)
    ld t1, -96(s0)
    sd t0, 0(t1)
    ld t0, -24(s0)
    li t1, 0
    add t0, t0, t1
    sd t0, -112(s0)
    ld t0, -112(s0)
    li t1, 0
    add t0, t0, t1
    sd t0, -120(s0)
    li t0, 200
    sext.w t0, t0
    sd t0, -128(s0)
    ld t0, -128(s0)
    ld t1, -120(s0)
    sd t0, 0(t1)
    ld t0, -24(s0)
    li t1, 0
    add t0, t0, t1
    sd t0, -136(s0)
    ld t0, -136(s0)
    li t1, 0
    add t0, t0, t1
    sd t0, -144(s0)
    ld t1, -144(s0)
    ld t0, 0(t1)
    sd t0, -152(s0)
    ld t0, -24(s0)
    li t1, 0
    add t0, t0, t1
    sd t0, -160(s0)
    ld t0, -160(s0)
    li t1, 0
    add t0, t0, t1
    sd t0, -168(s0)
    ld t1, -168(s0)
    ld t0, 0(t1)
    sd t0, -176(s0)
    ld t0, -152(s0)
    ld t1, -176(s0)
    add t0, t0, t1
    sd t0, -184(s0)
    ld t0, -184(s0)
    sext.w t0, t0
    sd t0, -192(s0)
    ld a0, -192(s0)
    li t2, 0
    sd t2, -200(s0)
    j test_struct_nested_epilogue
.L_test_struct_nested_L0:
    li a0, 0
    li t2, 1
    sd t2, -200(s0)
    j test_struct_nested_epilogue
test_struct_nested_epilogue:
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
    call test_struct_nested
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
