.section .text

.global interrupt_dummy_isr
.type interrupt_dummy_isr, @function
.align 4
interrupt_dummy_isr:
    iretl
.size interrupt_dummy_isr, . - interrupt_dummy_isr

.global interrupt_load_idt
.type interrupt_load_idt, @function
interrupt_load_idt:
    lidt (interrupt_idtr)
    ret
.size interrupt_load_idt, . - interrupt_load_idt

