.section ".text.boot"

.globl _start
_start:
  // Hang if core ID is non-zero.
  mrs x0, mpidr_el1
  and x0, x0, #0xFF
  cbnz x0, hang

  // Initialize stack pointer.
  mov sp, 2 << 21

  // Jump to the kernel.
  bl kernel_main

hang:
  b hang
