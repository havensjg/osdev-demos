#pragma once

// Access bits
#define GDT_PRESENT 0x80
#define GDT_DPL_0 0x0
#define GDT_DPL_1 0x20
#define GDT_DPL_2 0x40
#define GDT_DPL_3 0x60
#define GDT_SYSTEM 0x0
#define GDT_CODE 0x18
#define GDT_DATA 0x10
#define GDT_GROW_DOWN 0x4
#define GDT_CONFORM 0x4
#define GDT_RW 0x2
#define GDT_ACCESSED 0x1

// System segment types
#define GDT_16BIT_TSS_AVAILABLE 0x1
#define GDT_LDT 0x2
#define GDT_16BIT_TSS_BUSY 0x3
#define GDT_32BIT_TSS_AVAILABLE_9 0x9
#define GDT_32BIT_TSS_AVAILABLE_B 0xb

// Flag bits
#define GDT_PAGE 0x8
#define GDT_BYTE 0x0
#define GDT_16BIT 0x0
#define GDT_32BIT 0x4
#define GDT_64BIT_CODE 0x2

// default segments
#define GDT_KERNEL_CS 0x08
#define GDT_KERNEL_DS 0x10

#include <stdint.h>

typedef uint8_t gdt_entry_t[8];

struct s_gdtr {
    uint16_t size;
    gdt_entry_t *offset;
} __attribute__((packed));

typedef struct s_gdtr gdtr_t;