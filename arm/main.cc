#include <cstdint>

constexpr auto MMIO_BASE = 0x3F000000;

#define REG(addr) ((volatile uint32_t*)(MMIO_BASE + addr))

#define AUX_IRQ             REG(0x21'5000)
#define AUX_ENABLES         REG(0x21'5004)
#define AUX_MU_IO_REG       REG(0x21'5040)
#define AUX_MU_IER_REG      REG(0x21'5044)
#define AUX_MU_IIR_REG      REG(0x21'5048)
#define AUX_MU_LCR_REG      REG(0x21'504C)
#define AUX_MU_MCR_REG      REG(0x21'5050)
#define AUX_MU_LSR_REG      REG(0x21'5054)
#define AUX_MU_MSR_REG      REG(0x21'5058)
#define AUX_MU_SCRATCH      REG(0x21'505C)
#define AUX_MU_CNTL_REG     REG(0x21'5060)
#define AUX_MU_STAT_REG     REG(0x21'5064)
#define AUX_MU_BAUD_REG     REG(0x21'5068)
#define AUX_SPI0_CNTL0_REG  REG(0x21'5080)
#define AUX_SPI0_CNTL1_REG  REG(0x21'5084)
#define AUX_SPI0_STAT_REG   REG(0x21'5088)
#define AUX_SPI0_IO_REG     REG(0x21'5090)
#define AUX_SPI0_PEEK_REG   REG(0x21'5094)
#define AUX_SPI1_CNTL0_REG  REG(0x21'50C0)
#define AUX_SPI1_CNTL1_REG  REG(0x21'50C4)
#define AUX_SPI1_STAT_REG   REG(0x21'50C8)
#define AUX_SPI1_IO_REG     REG(0x21'50D0)
#define AUX_SPI1_PEEK_REG   REG(0x21'50D4)

#define GPFSEL0   REG(0x20'0000)
#define GPFSEL1   REG(0x20'0004)
#define GPFSEL2   REG(0x20'0008)
#define GPFSEL3   REG(0x20'000C)
#define GPFSEL4   REG(0x20'0010)
#define GPFSEL5   REG(0x20'0014)
#define GPPUD     REG(0x20'0094)
#define GPPUDCLK0 REG(0x20'0098)
#define GPPUDCLK1 REG(0x20'009C)

static int strlen(const char* s) {
  int i = 0;
  while (s[i]) {
    i++;
  }
  return i;
}

void main() {
  // Only enable UART1, disable SPI 1 and 2.
  *AUX_IRQ = 1;
  *AUX_ENABLES = 1;
  // Disable UART1 interrupts.
  *AUX_MU_IIR_REG = 0;
  // Operate in 8-bit mode.
  *AUX_MU_LCR_REG = 1;
  // Set baud rate. 400_000_000 / (8 * (433 + 1)) = 115_200.
  *AUX_MU_BAUD_REG = 433;
  // Enable tx/rx.
  *AUX_MU_CNTL_REG = 3;

  auto r = *GPFSEL1;
  // Clear previous GPIO 14 and 15 function settings.
  r &= ~(0b111 << 12);
  r &= ~(0b111 << 15);
  // Set GPIO 14 and 15 to TX and RX UART1.
  r |= 0b010 << 12;
  r |= 0b010 << 15;
  *GPFSEL1 = r;
  *GPPUD = 0;
  for (int i = 0; i < 150; i++) {
    asm volatile("nop");
  }
  *GPPUDCLK0 = (1 << 14) | (1 << 15);
  for (int i = 0; i < 150; i++) {
    asm volatile("nop");
  }
  *GPPUDCLK0 = 0;

  auto message = "hello world";
  for (int i = 0; i < strlen(message); i++) {
    auto transmitter_empty = *AUX_MU_LSR_REG & 0b100000;
    while (!transmitter_empty) {
      asm volatile("nop");
    }
    *AUX_MU_IO_REG = message[i];
  }

  while (1) {
    asm volatile("nop");
  }
}
