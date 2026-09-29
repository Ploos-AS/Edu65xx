#include "serial.h"

void edu65xx_serial_reset(edu65xx_serial_t *s)
{
    s->rx_data = 0u;
    s->rx_ready = 0u;
    s->tx_count = 0u;
}

uint8_t edu65xx_serial_read(edu65xx_serial_t *s, uint8_t reg)
{
    if (reg == EDU65XX_SERIAL_DATA) {
        uint8_t value = s->rx_data;
        s->rx_ready = 0u;
        return value;
    }
    if (reg == EDU65XX_SERIAL_STATUS)
        return (uint8_t)(EDU65XX_SERIAL_TX_READY | (s->rx_ready ? EDU65XX_SERIAL_RX_READY : 0u));
    return 0u;
}

void edu65xx_serial_write(edu65xx_serial_t *s, uint8_t reg, uint8_t value)
{
    if (reg == EDU65XX_SERIAL_DATA && s->tx_count < EDU65XX_SERIAL_BUFFER)
        s->tx[s->tx_count++] = value;
}

void edu65xx_serial_receive(edu65xx_serial_t *s, uint8_t value)
{
    s->rx_data = value;
    s->rx_ready = 1u;
}
