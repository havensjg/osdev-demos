.section .text

.global interrupt_disable
.type interrupt_disable, @function
interrupt_disable:
    cli
    ret
.size interrupt_disable, . - interrupt_disable

.global interrupt_dummy_isr
.type interrupt_dummy_isr, @function
.align 4
interrupt_dummy_isr:
    iretl
.size interrupt_dummy_isr, . - interrupt_dummy_isr

.global interrupt_enable
.type interrupt_enable, @function
interrupt_enable:
    sti
    ret
.size interrupt_enable, . - interrupt_enable

.global interrupt_load_idt
.type interrupt_load_idt, @function
interrupt_load_idt:
    lidt (interrupt_idtr)
    ret
.size interrupt_load_idt, . - interrupt_load_idt

.global interrupt_wait
.type interrupt_wait, @function
interrupt_wait:
    hlt
    ret
.size interrupt_wait, . - interrupt_wait
