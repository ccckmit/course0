.text

.global test_union_basic
test_union_basic:
    addi sp, sp, -1024
    sd ra, 1016(sp)
    sd s0, 1008(sp)
    addi s0, sp, 1024
.L_test_union_basic_entry:
    addi t0, s0, -32
    sd t0, -24(s0)
    ld t0, -24(s0)
    li t1, 0
    add t0, t0, t1
    sd t0, -40(s0)
    li t0, 1094861636
    sext.w t0, t0
    sd t0, -48(s0)
    ld t0, -48(s0)
    ld t1, -40(s0)
    sd t0, 0(t1)
    ld t0, -24(s0)
    li t1, 0
    add t0, t0, t1
    sd t0, -56(s0)
    ld t1, -56(s0)
    ld t0, 0(t1)
    sd t0, -64(s0)
    ld t0, -64(s0)
    sext.w t0, t0
    sd t0, -72(s0)
    ld a0, -72(s0)
    li t2, 0
    sd t2, -80(s0)
    j test_union_basic_epilogue
.L_test_union_basic_L0:
    li a0, 0
    li t2, 1
    sd t2, -80(s0)
    j test_union_basic_epilogue
test_union_basic_epilogue:
    ld ra, 1016(sp)
    ld s0, 1008(sp)
    addi sp, sp, 1024
    ret

.global test_union_cast
test_union_cast:
    addi sp, sp, -1024
    sd ra, 1016(sp)
    sd s0, 1008(sp)
    addi s0, sp, 1024
.L_test_union_cast_entry:
    addi t0, s0, -32
    sd t0, -24(s0)
    ld t0, -24(s0)
    li t1, 0
    add t0, t0, t1
    sd t0, -40(s0)
    li t0, 65
    sext.w t0, t0
    sd t0, -48(s0)
    ld t0, -48(s0)
    ld t1, -40(s0)
    sd t0, 0(t1)
    ld t0, -24(s0)
    li t1, 0
    add t0, t0, t1
    sd t0, -56(s0)
    ld t1, -56(s0)
    ld t0, 0(t1)
    sd t0, -64(s0)
    ld t0, -64(s0)
    sext.w t0, t0
    sd t0, -72(s0)
    ld a0, -72(s0)
    li t2, 0
    sd t2, -80(s0)
    j test_union_cast_epilogue
.L_test_union_cast_L0:
    li a0, 0
    li t2, 1
    sd t2, -80(s0)
    j test_union_cast_epilogue
test_union_cast_epilogue:
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
    call test_union_basic
    sd a0, -24(s0)
    ld t0, -24(s0)
    sext.w t0, t0
    sd t0, -32(s0)
    call test_union_cast
    sd a0, -40(s0)
    ld t0, -40(s0)
    sext.w t0, t0
    sd t0, -48(s0)
    ld t0, -32(s0)
    ld t1, -48(s0)
    add t0, t0, t1
    sd t0, -56(s0)
    ld t0, -56(s0)
    sext.w t0, t0
    sd t0, -64(s0)
    ld a0, -64(s0)
    li t2, 0
    sd t2, -72(s0)
    j main_epilogue
.L_main_L0:
    li a0, 0
    li t2, 1
    sd t2, -72(s0)
    j main_epilogue
main_epilogue:
    ld ra, 1016(sp)
    ld s0, 1008(sp)
    addi sp, sp, 1024
    ret
