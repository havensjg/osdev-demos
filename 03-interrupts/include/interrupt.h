#pragma once

#include <stdint.h>

#define IDT_PRESENT 0x80

#define IDT_DPL(dpl) ((dpl & 0x3) << 5)
#define IDT_DPL_0 IDT_DPL(0)
#define IDT_DPL_1 IDT_DPL(1)
#define IDT_DPL_2 IDT_DPL(2)
#define IDT_DPL_3 IDT_DPL(3)

#define IDT_GATE_TYPE(gt) (gt & 0xf)
#define IDT_TASK_GATE IDT_GATE_TYPE(5)
#define IDT_16BIT_INTERRUPT IDT_GATE_TYPE(6)
#define IDT_16BIT_TRAP IDT_GATE_TYPE(7)
#define IDT_32BIT_INTERRUPT IDT_GATE_TYPE(0xe)
#define IDT_32BIT_TRAP IDT_GATE_TYPE(0xf)

#define INTERRUPT_MAX 0xFF

/* Structure of each entry in the IDT */
struct s_idt_entry {
    uint16_t offset_lo;
    uint16_t selector;
    uint8_t zero;
    uint8_t attributes;
    uint16_t offset_hi;
}__attribute__((packed));

typedef struct s_idt_entry idt_entry_t;

/* Structure of the IDTR register */
struct s_idtr {
    uint16_t size;
    idt_entry_t *offset;
}__attribute__((packed));

typedef struct s_idtr idtr_t;

/* Dummy ISR. Returns and does nothing */
void interrupt_dummy_isr(void);

/* Initialize IDT */
void interrupt_init(void);

/* Loads the IDTR register */
void interrupt_load_idt(void);

/* 
    Installs an ISR into the IDT 

    num - vector in table (0 to INTERRUPT_MAX - 1)
    sel - GDT selector for the ISR code
    off - pointer to the ISR
    attr - attributes of the IDT entry. consists of a gate type or'd with a privilege level or'd with IDT_PRESENT (if present) or nothing if entry is not present
        gate types:
        IDT_TASK_GATE
        IDT_16BIT_INTERRUPT
        IDT_16BIT_TRAP
        IDT_32BIT_INTERRUPT 
        IDT_32BIT_TRAP

        privilege levels:
        DPL0
        DPL1
        DPL2
        DPL3

*/
void interrupt_set(uint8_t num, uint16_t sel, void *off, uint8_t attr);