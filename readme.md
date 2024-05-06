## OSDev demos

A collection of specific examples for x86 platform hardware or OS development concepts that I've written or adapted.

All examples are licensed under the MIT license, see LICENSE for details.

### Requirements:

- i686-elf GCC/G++ and binutils [(build/install instructions from OSDev Wiki)](https://wiki.osdev.org/GCC_Cross-Compiler)
    - The CMake file `toolchain-i686-elf.cmake` will expect to find this toolchain in `~/opt/cross/bin`
- grub-mkrescue (with PC/BIOS binaries)
- CMake
- (for testing) qemu or other virtual machine
    - To install qemu on Debian-based systems, run `apt install qemu-system-x86`

### Building any demo:

```
mkdir build
cd build
cmake ..
make livecd
```

The resulting live CD image will be in `build/myos.iso`

### Testing any demo:

Using qemu:

`qemu-system-i386 -cdrom build/myos.iso`