#include <cstdint>

extern "C" void asm_nop_loop(uint32_t cycles);

// Bus addresses start at 0x7Exx'xxxx.
// Memory-mapped physical addresses start at 0x3Fxx'xxxx.
#define GPFSEL0             reinterpret_cast<volatile uint32_t*>(0x3F20'0000)
#define GPFSEL1             reinterpret_cast<volatile uint32_t*>(0x3F20'0004)
#define GPFSEL2             reinterpret_cast<volatile uint32_t*>(0x3F20'0008)
#define GPFSEL3             reinterpret_cast<volatile uint32_t*>(0x3F20'000C)
#define GPFSEL4             reinterpret_cast<volatile uint32_t*>(0x3F20'0010)
#define GPFSEL5             reinterpret_cast<volatile uint32_t*>(0x3F20'0014)
#define GPSET0              reinterpret_cast<volatile uint32_t*>(0x3F20'001C)
#define GPSET1              reinterpret_cast<volatile uint32_t*>(0x3F20'0020)
#define GPCLR0              reinterpret_cast<volatile uint32_t*>(0x3F20'0028)
#define GPCLR1              reinterpret_cast<volatile uint32_t*>(0x3F20'002C)
#define GPLEV0              reinterpret_cast<volatile uint32_t*>(0x3F20'0034)
#define GPLEV1              reinterpret_cast<volatile uint32_t*>(0x3F20'0038)
#define GPEDS0              reinterpret_cast<volatile uint32_t*>(0x3F20'0040)
#define GPEDS1              reinterpret_cast<volatile uint32_t*>(0x3F20'0044)
#define GPREN0              reinterpret_cast<volatile uint32_t*>(0x3F20'004C)
#define GPREN1              reinterpret_cast<volatile uint32_t*>(0x3F20'0050)
#define GPFEN0              reinterpret_cast<volatile uint32_t*>(0x3F20'0058)
#define GPFEN1              reinterpret_cast<volatile uint32_t*>(0x3F20'005C)
#define GPHEN0              reinterpret_cast<volatile uint32_t*>(0x3F20'0064)
#define GPLEN0              reinterpret_cast<volatile uint32_t*>(0x3F20'0070)
#define GPLEN1              reinterpret_cast<volatile uint32_t*>(0x3F20'0074)
#define GPAREN0             reinterpret_cast<volatile uint32_t*>(0x3F20'007C)
#define GPAREN1             reinterpret_cast<volatile uint32_t*>(0x3F20'0080)
#define GPAFEN0             reinterpret_cast<volatile uint32_t*>(0x3F20'0088)
#define GPAFEN1             reinterpret_cast<volatile uint32_t*>(0x3F20'008C)
#define GPPUD               reinterpret_cast<volatile uint32_t*>(0x3F20'0094)
#define GPPUDCLK0           reinterpret_cast<volatile uint32_t*>(0x3F20'0098)
#define GPPUDCLK1           reinterpret_cast<volatile uint32_t*>(0x3F20'009C)
#define AUX_IRQ             reinterpret_cast<volatile uint32_t*>(0x3F21'5000)
#define AUX_ENABLES         reinterpret_cast<volatile uint32_t*>(0x3F21'5004)
#define AUX_MU_IO_REG       reinterpret_cast<volatile uint32_t*>(0x3F21'5040)
#define AUX_MU_IER_REG      reinterpret_cast<volatile uint32_t*>(0x3F21'5044)
#define AUX_MU_IIR_REG      reinterpret_cast<volatile uint32_t*>(0x3F21'5048)
#define AUX_MU_LCR_REG      reinterpret_cast<volatile uint32_t*>(0x3F21'504C)
#define AUX_MU_MCR_REG      reinterpret_cast<volatile uint32_t*>(0x3F21'5050)
#define AUX_MU_LSR_REG      reinterpret_cast<volatile uint32_t*>(0x3F21'5054)
#define AUX_MU_MSR_REG      reinterpret_cast<volatile uint32_t*>(0x3F21'5058)
#define AUX_MU_SCRATCH      reinterpret_cast<volatile uint32_t*>(0x3F21'505C)
#define AUX_MU_CNTL_REG     reinterpret_cast<volatile uint32_t*>(0x3F21'5060)
#define AUX_MU_STAT_REG     reinterpret_cast<volatile uint32_t*>(0x3F21'5064)
#define AUX_MU_BAUD_REG     reinterpret_cast<volatile uint32_t*>(0x3F21'5068)
#define AUX_SPI0_CNTL0_REG  reinterpret_cast<volatile uint32_t*>(0x3F21'5080)
#define AUX_SPI0_CNTL1_REG  reinterpret_cast<volatile uint32_t*>(0x3F21'5084)
#define AUX_SPI0_STAT_REG   reinterpret_cast<volatile uint32_t*>(0x3F21'5088)
#define AUX_SPI0_IO_REG     reinterpret_cast<volatile uint32_t*>(0x3F21'5090)
#define AUX_SPI0_PEEK_REG   reinterpret_cast<volatile uint32_t*>(0x3F21'5094)
#define AUX_SPI1_CNTL0_REG  reinterpret_cast<volatile uint32_t*>(0x3F21'50C0)
#define AUX_SPI1_CNTL1_REG  reinterpret_cast<volatile uint32_t*>(0x3F21'50C4)
#define AUX_SPI1_STAT_REG   reinterpret_cast<volatile uint32_t*>(0x3F21'50C8)
#define AUX_SPI1_IO_REG     reinterpret_cast<volatile uint32_t*>(0x3F21'50D0)
#define AUX_SPI1_PEEK_REG   reinterpret_cast<volatile uint32_t*>(0x3F21'50D4)

static void putc(char c) {
  for (;;) {
    if (*AUX_MU_LSR_REG & 0b0010'0000) {
      break;
    }
  }
  *AUX_MU_IO_REG = c;
}

static char getc() {
  for (;;) {
    if (*AUX_MU_LSR_REG & 1) {
      break;
    }
  }
  return *AUX_MU_IO_REG & 0xFF;
}

static void print(const char* s) {
  for (int i = 0;; i++) {
    auto c = s[i];
    if (c == '\0') {
      break;
    }
    putc(c);
  }
}

extern "C" void kernel_main() {
  // https://datasheets.raspberrypi.org/bcm2835/bcm2835-peripherals.pdf, Page 91.
  auto v = *GPFSEL1;
  // Set GPIO14 to use alternate function 5.
  v &= ~(0b111 << 12);
  v |= 0b010 << 12;
  // Set GPIO15 to use alternate function 5.
  v &= ~(0b111 << 15);
  v |= 0b010 << 15;
  *GPFSEL1 = v;

  *GPPUD = 0;
  for (int i = 0; i < 150; i++) {
    asm volatile("nop");
  }
  *GPPUDCLK0 = (1 << 14) | (1 << 15);
  for (int i = 0; i < 150; i++) {
    asm volatile("nop");
  }
  *GPPUDCLK0 = 0;

  *AUX_ENABLES = 1;
  *AUX_MU_CNTL_REG = 0;
  *AUX_MU_IER_REG = 0;
  *AUX_MU_LCR_REG = 3;
  *AUX_MU_MCR_REG = 0;
  *AUX_MU_BAUD_REG = 270;
  *AUX_MU_CNTL_REG = 3;

  print("hello world\n");

  char line[256];
  int line_len = 0;

  for (;;) {
    auto c = getc();
    if (line_len >= 255) {
      line[line_len] = '\0';
      print("\r\n");
      print(line);
      print("\r\n");
      line_len = 0;
    }
    line[line_len++] = c;
    if (c == '\r') {
      line[line_len] = '\0';
      print("\r\n");
      print(line);
      print("\r\n");
      line_len = 0;
      continue;
    }
    putc(c);
  }
}
