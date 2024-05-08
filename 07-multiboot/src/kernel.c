#include <stdio.h>

#include <multiboot.h>
#include <terminal.h>

/* Check if the compiler thinks you are targeting the wrong operating system. */
#if defined(__linux__)
#error "You are not using a cross-compiler, you will most certainly run into trouble"
#endif
 
/* This tutorial will only work for the 32-bit ix86 targets. */
#if !defined(__i386__)
#error "This tutorial needs to be compiled with a ix86-elf compiler"
#endif

multiboot_info_t *multiboot_info;
 
void kernel_main(void) 
{
	/* Initialize terminal interface */
	terminal_initialize();

	/* Print something on the screen */
	printf("Hello, kernel World!\n");

	/* Print address of the structure and the flags */
	printf("Multiboot located at: %p\n", multiboot_info);
	printf(" Flags: %x\n", multiboot_info->flags);

	/* Print basic memory information */
	if (multiboot_info->flags & MULTIBOOT_INFO_MEMORY) {
		printf(" Mem lower: %x\n", multiboot_info->mem_lower);
		printf(" Mem upper: %x\n", multiboot_info->mem_upper);
	}

	/* Print boot device info */
	if (multiboot_info->flags & MULTIBOOT_INFO_BOOTDEV) {
		printf(" Boot device: %x\n", multiboot_info->boot_device);
	}

	/* Print command line */
	if (multiboot_info->flags & MULTIBOOT_INFO_CMDLINE) {
		printf(" Command line: [%p] \"%s\"\n", multiboot_info->cmdline, multiboot_info->cmdline);
	}

	/* Print module info */
	if (multiboot_info->flags & MULTIBOOT_INFO_MODS) {
		printf(" Module count: %d\n",multiboot_info->mods_count);
		printf(" Module address: %p\n",multiboot_info->mods_addr);
	}

	/* Print AOUT symbol info */
	if (multiboot_info->flags & MULTIBOOT_INFO_AOUT_SYMS) {
		printf(" AOUT tabsize: %d\n",multiboot_info->u.aout_sym.tabsize);
		printf(" AOUT symsize: %d\n",multiboot_info->u.aout_sym.strsize);
		printf(" AOUT addr: %x\n",multiboot_info->u.aout_sym.addr);
	}

	/* Print ELF symbol info */
	if (multiboot_info->flags & MULTIBOOT_INFO_ELF_SHDR) {
		printf(" ELF num: %d\n",multiboot_info->u.elf_sec.num);
		printf(" ELF size: %d\n",multiboot_info->u.elf_sec.size);
		printf(" ELF addr: %x\n",multiboot_info->u.elf_sec.addr);
		printf(" ELF shndx: %d\n",multiboot_info->u.elf_sec.shndx);
	}

	/* Print memory map info */
	if (multiboot_info->flags & MULTIBOOT_INFO_MEM_MAP) {
		printf(" Memory map length: %d\n",multiboot_info->mmap_length);
		printf(" Memory map addr: %p\n",multiboot_info->mmap_addr);
	}

	/* Print drive info */
	if (multiboot_info->flags & MULTIBOOT_INFO_DRIVE_INFO) {
		printf(" Drive info length: %d\n",multiboot_info->drives_length);
		printf(" Drive info addr: %x\n",multiboot_info->drives_addr);
	}

	/* Print ROM configuration table */
	if (multiboot_info->flags & MULTIBOOT_INFO_CONFIG_TABLE) {
		printf(" Configuration table: %x\n",multiboot_info->config_table);
	}

	/* Print bootloader name */
	if (multiboot_info->flags & MULTIBOOT_INFO_BOOT_LOADER_NAME) {
		printf(" Bootloader name: [%p] \"%s\"\n",multiboot_info->boot_loader_name,multiboot_info->boot_loader_name);
	}

	/* Print APM table info */
	if (multiboot_info->flags & MULTIBOOT_INFO_APM_TABLE) {
		printf(" APM table: %p\n",multiboot_info->apm_table);
	}

	/* Print VBE info */
	if (multiboot_info->flags & MULTIBOOT_INFO_VBE_INFO) {
		printf(" VBE: control_info=%x mode_info=%x mode=%x interface=%x:%x len=%x\n",
			multiboot_info->vbe_control_info,
			multiboot_info->vbe_mode_info,
			multiboot_info->vbe_mode,
			multiboot_info->vbe_interface_seg,
			multiboot_info->vbe_interface_off,
			multiboot_info->vbe_interface_len
			);
	}
	
	/* Print frame buffer info */
	if (multiboot_info->flags & MULTIBOOT_INFO_FRAMEBUFFER_INFO) {
		printf(" FB: addr=%x, pitch=%x, res=%dx%d, bpp=%d, type=%d\n",
			multiboot_info->framebuffer_addr,
			multiboot_info->framebuffer_pitch,
			multiboot_info->framebuffer_width,
			multiboot_info->framebuffer_height,
			multiboot_info->framebuffer_bpp,
			multiboot_info->framebuffer_type
			);
	}
}