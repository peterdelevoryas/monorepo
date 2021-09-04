#include <cstdint>
#include <string_view>

using std::string_view;
using std::size_t;

static auto AUX_IRQ             = (volatile uint32_t*)0x3F21'5000;
static auto AUX_ENABLES         = (volatile uint32_t*)0x3F21'5004;
static auto AUX_MU_IO_REG       = (volatile uint32_t*)0x3F21'5040;
static auto AUX_MU_IER_REG      = (volatile uint32_t*)0x3F21'5044;
static auto AUX_MU_IIR_REG      = (volatile uint32_t*)0x3F21'5048;
static auto AUX_MU_LCR_REG      = (volatile uint32_t*)0x3F21'504C;
static auto AUX_MU_MCR_REG      = (volatile uint32_t*)0x3F21'5050;
static auto AUX_MU_LSR_REG      = (volatile uint32_t*)0x3F21'5054;
static auto AUX_MU_MSR_REG      = (volatile uint32_t*)0x3F21'5058;
static auto AUX_MU_SCRATCH      = (volatile uint32_t*)0x3F21'505C;
static auto AUX_MU_CNTL_REG     = (volatile uint32_t*)0x3F21'5060;
static auto AUX_MU_STAT_REG     = (volatile uint32_t*)0x3F21'5064;
static auto AUX_MU_BAUD_REG     = (volatile uint32_t*)0x3F21'5068;
static auto AUX_SPI0_CNTL0_REG  = (volatile uint32_t*)0x3F21'5080;
static auto AUX_SPI0_CNTL1_REG  = (volatile uint32_t*)0x3F21'5084;
static auto AUX_SPI0_STAT_REG   = (volatile uint32_t*)0x3F21'5088;
static auto AUX_SPI0_IO_REG     = (volatile uint32_t*)0x3F21'5090;
static auto AUX_SPI0_PEEK_REG   = (volatile uint32_t*)0x3F21'5094;
static auto AUX_SPI1_CNTL0_REG  = (volatile uint32_t*)0x3F21'50C0;
static auto AUX_SPI1_CNTL1_REG  = (volatile uint32_t*)0x3F21'50C4;
static auto AUX_SPI1_STAT_REG   = (volatile uint32_t*)0x3F21'50C8;
static auto AUX_SPI1_IO_REG     = (volatile uint32_t*)0x3F21'50D0;
static auto AUX_SPI1_PEEK_REG   = (volatile uint32_t*)0x3F21'50D4;
static auto GPFSEL0             = (volatile uint32_t*)0x3F20'0000;
static auto GPFSEL1             = (volatile uint32_t*)0x3F20'0004;
static auto GPFSEL2             = (volatile uint32_t*)0x3F20'0008;
static auto GPFSEL3             = (volatile uint32_t*)0x3F20'000C;
static auto GPFSEL4             = (volatile uint32_t*)0x3F20'0010;
static auto GPFSEL5             = (volatile uint32_t*)0x3F20'0014;
static auto GPPUD               = (volatile uint32_t*)0x3F20'0094;
static auto GPPUDCLK0           = (volatile uint32_t*)0x3F20'0098;
static auto GPPUDCLK1           = (volatile uint32_t*)0x3F20'009C;

constexpr uint32_t AUX_MU_LSR_TX_READY = 0b0010'0000;
constexpr uint32_t AUX_MU_LSR_RX_READY = 0b0000'0001;

static void uart1_init() {
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
}

static void uart1_init_gpio() {
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
}

static char uart1_getc() {
    for (;;) {
        auto rx_ready = *AUX_MU_LSR_REG & AUX_MU_LSR_RX_READY;
        if (rx_ready) {
            break;
        }
    }
    auto c = static_cast<char>(*AUX_MU_IO_REG);
    return c;
}

static void uart1_putc(char c) {
    for (;;) {
        auto tx_ready = *AUX_MU_LSR_REG & AUX_MU_LSR_TX_READY;
        if (tx_ready) {
            break;
        }
    }
    *AUX_MU_IO_REG = static_cast<uint32_t>(c);
}

static void uart1_puts(string_view s) {
    auto n = s.size();
    for (size_t i = 0; i < n; i++) {
        uart1_putc(s[i]);
    }
}

constexpr auto SPLASH_TEXT =
"\n"
"       Operating System\n"
"\n";

constexpr auto PROMPT = "> ";

extern "C"
int main() {
    uart1_init();
    uart1_puts(SPLASH_TEXT);

    while (1) {
        uart1_puts(PROMPT);
        char line[256];
        int i = 0;
        for (; i < sizeof(line) - 1; i++) {
            auto c = uart1_getc();
            if (c == '\r') {
                break;
            }
            line[i] = c;
            uart1_putc(c);
        }
        line[i] = '\0';
        uart1_putc('\n');
    }
}

extern "C"
size_t strlen(const char* s) {
    size_t i = 0;
    for (; s[i]; i++);
    return i;
}
