.global irq1_wrap
.align 4
.type irq1_wrap, @function
irq1_wrap:
    pushal
    cld
    call keyboard_irq
    popal
    iret
.size irq1_wrap, . - irq1_wrap
