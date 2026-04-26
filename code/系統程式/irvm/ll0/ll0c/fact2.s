.text

.global factorial
factorial:
    addi sp, sp, -256
    sd ra, 248(sp)
    sd s0, 240(sp)
    addi s0, sp, 256
    sd a0, -24(s0)
.L_factorial_entry:
    addi t0, s0, -40
    sd t0, -32(s0)
    addi t0, s0, -56
    sd t0, -48(s0)
    ld t0, -24(s0)
    ld t1, -48(s0)
    sd t0, 0(t1)
    ld t1, -48(s0)
    ld t0, 0(t1)
    sd t0, -64(s0)
    ld t0, -64(s0)
    li t1, 0
    sub t0, t0, t1
    seqz t0, t0
    sd t0, -72(s0)
    ld t0, -72(s0)
    bnez t0, .L_factorial_6
    j .L_factorial_7
.L_factorial_6:
    li t0, 1
    ld t1, -32(s0)
    sd t0, 0(t1)
    j .L_factorial_13
.L_factorial_7:
    ld t1, -48(s0)
    ld t0, 0(t1)
    sd t0, -80(s0)
    ld t1, -48(s0)
    ld t0, 0(t1)
    sd t0, -88(s0)
    ld t0, -88(s0)
    li t1, 1
    sub t0, t0, t1
    sd t0, -96(s0)
    ld t0, -96(s0)
    mv a0, t0
    call factorial
    sd a0, -104(s0)
    ld t0, -80(s0)
    ld t1, -104(s0)
    mul t0, t0, t1
    sd t0, -112(s0)
    ld t0, -112(s0)
    ld t1, -32(s0)
    sd t0, 0(t1)
    j .L_factorial_13
.L_factorial_13:
    ld t1, -32(s0)
    ld t0, 0(t1)
    sd t0, -120(s0)
    ld a0, -120(s0)
    j factorial_epilogue
factorial_epilogue:
    ld ra, 248(sp)
    ld s0, 240(sp)
    addi sp, sp, 256
    ret

.global main
main:
    addi sp, sp, -256
    sd ra, 248(sp)
    sd s0, 240(sp)
    addi s0, sp, 256
.L_main_entry:
    addi t0, s0, -32
    sd t0, -24(s0)
    li t0, 0
    ld t1, -24(s0)
    sd t0, 0(t1)
    li t0, 5
    mv a0, t0
    call factorial
    sd a0, -40(s0)
    ld a0, -40(s0)
    j main_epilogue
main_epilogue:
    ld ra, 248(sp)
    ld s0, 240(sp)
    addi sp, sp, 256
    ret
