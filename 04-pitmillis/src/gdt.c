#include <stdint.h>

#include <gdt.h>

gdtr_t gdt_gdtr;
gdt_entry_t gdt[3]; // 3 segments

void gdt_set(uint16_t seg, uint32_t off, uint32_t lim, uint8_t access, uint8_t flags) {
    int ent = seg >> 3;

    gdt[ent][0] = lim & 0xff;
    gdt[ent][1] = (lim >> 8) & 0xff;
    gdt[ent][6] = (lim >> 16) & 0xf;

    gdt[ent][2] = off & 0xff;
    gdt[ent][3] = (off>>8) & 0xff;
    gdt[ent][4] = (off>>16) & 0xff;
    gdt[ent][7] = (off>>24) & 0xff;

    gdt[ent][5] = access;

    gdt[ent][6] |= flags << 4;
}

void gdt_init(void) {
    gdt_set(0x0,0x0,0xfffff,0,0); // null segment
    gdt_set(GDT_KERNEL_CS,0x0,0xfffff,GDT_PRESENT | GDT_CODE | GDT_RW, GDT_PAGE | GDT_32BIT); // code segment
    gdt_set(GDT_KERNEL_DS,0x0,0xfffff,GDT_PRESENT | GDT_DATA | GDT_RW, GDT_PAGE | GDT_32BIT); // data segment

    gdt_gdtr.offset = gdt;
    gdt_gdtr.size = sizeof(gdt);
}