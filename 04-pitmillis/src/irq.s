.global irq0_wrap
.align 4
.type irq0_wrap, @function
irq0_wrap:
    pushal
    cld
    call pit_irq
    popal
    iret
.size irq0_wrap, . - irq0_wrap