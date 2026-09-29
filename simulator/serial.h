#ifndef EDU65XX_SERIAL_H
#define EDU65XX_SERIAL_H
#include <stddef.h>
#include <stdint.h>

#define EDU65XX_SERIAL_BASE 0x8010u
#define EDU65XX_SERIAL_END  0x8011u
#define EDU65XX_SERIAL_DATA   0u
#define EDU65XX_SERIAL_STATUS 1u
#define EDU65XX_SERIAL_RX_READY 0x01u
#define EDU65XX_SERIAL_TX_READY 0x02u
#define EDU65XX_SERIAL_BUFFER 256u

typedef struct {
    uint8_t rx_data;
    uint8_t rx_ready;
    uint8_t tx[EDU65XX_SERIAL_BUFFER];
    size_t tx_count;
} edu65xx_serial_t;

void edu65xx_serial_reset(edu65xx_serial_t *serial);
uint8_t edu65xx_serial_read(edu65xx_serial_t *serial, uint8_t reg);
void edu65xx_serial_write(edu65xx_serial_t *serial, uint8_t reg, uint8_t value);
void edu65xx_serial_receive(edu65xx_serial_t *serial, uint8_t value);
#endif
