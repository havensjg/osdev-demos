Adapted from 02-printf

The GRUB bootloader provides information to the kernel in a structure whose pointer is passed to the kernel entry point in the EBX register. This example will read and parse that structure.

[Multiboot specification](https://www.gnu.org/software/grub/manual/multiboot/multiboot.html)