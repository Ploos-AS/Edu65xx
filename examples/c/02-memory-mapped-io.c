#include <stdint.h>

/* Edu65xx teaching serial interface from M3. */
#define SERIAL_DATA (*(volatile uint8_t *)0x8010u)
#define SERIAL_STATUS (*(volatile uint8_t *)0x8011u)
#define SERIAL_TX_READY 0x02u

void serial_putc(uint8_t value)
{
    while ((SERIAL_STATUS & SERIAL_TX_READY) == 0u) {
        /* Wait until the teaching UART can accept a byte. */
    }
    SERIAL_DATA = value;
}

int main(void)
{
    serial_putc((uint8_t)'C');
    return 0;
}
