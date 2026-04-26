.text

.global factorial
factorial:
    addi sp, sp, -1024
    sd ra, 1016(sp)
    sd s0, 1008(sp)
    addi s0, sp, 1024
    sd a0, -24(s0)
.L_factorial_entry:
    li t0, 1
    sext.w t0, t0
    sd t0, -32(s0)
    ld t0, -24(s0)
    ld t1, -32(s0)
    slt t0, t1, t0
    xori t0, t0, 1
    sd t0, -40(s0)
    ld t0, -40(s0)
    andi t0, t0, 1
    sd t0, -48(s0)
    ld t0, -48(s0)
    li t1, 0
    sub t0, t0, t1
    snez t0, t0
    sd t0, -56(s0)
    ld t0, -56(s0)
    li t2, 0
    sd t2, -64(s0)
    bnez t0, .L_factorial_L0
    j .L_factorial_L2
.L_factorial_L0:
    li t0, 1
    sext.w t0, t0
    sd t0, -72(s0)
    ld a0, -72(s0)
    li t2, 1
    sd t2, -64(s0)
    j factorial_epilogue
.L_factorial_L3:
    li t2, 2
    sd t2, -64(s0)
    j .L_factorial_L2
.L_factorial_L2:
    li t0, 1
    sext.w t0, t0
    sd t0, -80(s0)
    ld t0, -24(s0)
    ld t1, -80(s0)
    sub t0, t0, t1
    sd t0, -88(s0)
    ld t0, -88(s0)
    mv a0, t0
    call factorial
    sd a0, -96(s0)
    ld t0, -24(s0)
    ld t1, -96(s0)
    mul t0, t0, t1
    sd t0, -104(s0)
    ld a0, -104(s0)
    li t2, 3
    sd t2, -64(s0)
    j factorial_epilogue
.L_factorial_L4:
    li a0, 0
    li t2, 4
    sd t2, -64(s0)
    j factorial_epilogue
factorial_epilogue:
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
    li t0, 5
    mv a0, t0
    call factorial
    sd a0, -24(s0)
    ld t0, -24(s0)
    li t1, 0
    add t0, t0, t1
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
