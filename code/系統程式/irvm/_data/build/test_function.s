.text

.global add
add:
    addi sp, sp, -1024
    sd ra, 1016(sp)
    sd s0, 1008(sp)
    addi s0, sp, 1024
    sd a0, -24(s0)
    sd a1, -32(s0)
.L_add_entry:
    ld t0, -24(s0)
    ld t1, -32(s0)
    add t0, t0, t1
    sd t0, -40(s0)
    ld t0, -40(s0)
    sext.w t0, t0
    sd t0, -48(s0)
    ld a0, -48(s0)
    li t2, 0
    sd t2, -56(s0)
    j add_epilogue
.L_add_L0:
    li a0, 0
    li t2, 1
    sd t2, -56(s0)
    j add_epilogue
add_epilogue:
    ld ra, 1016(sp)
    ld s0, 1008(sp)
    addi sp, sp, 1024
    ret

.global mul
mul:
    addi sp, sp, -1024
    sd ra, 1016(sp)
    sd s0, 1008(sp)
    addi s0, sp, 1024
    sd a0, -24(s0)
    sd a1, -32(s0)
.L_mul_entry:
    ld t0, -24(s0)
    ld t1, -32(s0)
    mul t0, t0, t1
    sd t0, -40(s0)
    ld t0, -40(s0)
    sext.w t0, t0
    sd t0, -48(s0)
    ld a0, -48(s0)
    li t2, 0
    sd t2, -56(s0)
    j mul_epilogue
.L_mul_L0:
    li a0, 0
    li t2, 1
    sd t2, -56(s0)
    j mul_epilogue
mul_epilogue:
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
    addi t0, s0, -32
    sd t0, -24(s0)
    li t0, 10
    mv a0, t0
    li t0, 11
    mv a1, t0
    call add
    sd a0, -40(s0)
    ld t0, -40(s0)
    sext.w t0, t0
    sd t0, -48(s0)
    ld t0, -48(s0)
    ld t1, -24(s0)
    sd t0, 0(t1)
    addi t0, s0, -64
    sd t0, -56(s0)
    ld t1, -24(s0)
    ld t0, 0(t1)
    sd t0, -72(s0)
    ld t0, -72(s0)
    mv a0, t0
    li t0, 2
    mv a1, t0
    call mul
    sd a0, -80(s0)
    ld t0, -80(s0)
    sext.w t0, t0
    sd t0, -88(s0)
    ld t0, -88(s0)
    ld t1, -56(s0)
    sd t0, 0(t1)
    ld t1, -56(s0)
    ld t0, 0(t1)
    sd t0, -96(s0)
    ld t0, -96(s0)
    sext.w t0, t0
    sd t0, -104(s0)
    ld t0, -104(s0)
    sext.w t0, t0
    sd t0, -112(s0)
    ld a0, -112(s0)
    li t2, 0
    sd t2, -120(s0)
    j main_epilogue
.L_main_L0:
    li a0, 0
    li t2, 1
    sd t2, -120(s0)
    j main_epilogue
main_epilogue:
    ld ra, 1016(sp)
    ld s0, 1008(sp)
    addi sp, sp, 1024
    ret
