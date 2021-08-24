.section ".text.boot"

.globl _start
_start:
  // Hang if core ID is non-zero.
  mrs x0, mpidr_el1
  and x0, x0, #0xFF
  cbnz x0, hang

  // Initialize stack pointer.
  mov sp, 2 << 21

  // Initialize bss section.
  ldr x0, =__bss_start
  ldr x1, =__bss_end
1:
  str xzr, [x0], #8
  cmp x0, x1
  bge 1b

  bl main

hang:
  b hang
