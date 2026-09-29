#ifndef EDU65XX_VIA_H
#define EDU65XX_VIA_H
#include <stdint.h>
#define EDU65XX_VIA_BASE 0x8000u
#define EDU65XX_VIA_END  0x800Fu
#define EDU65XX_VIA_IFR_T1 0x40u
typedef struct {
 uint8_t orb, ora, ddrb, ddra, input_b, input_a;
 uint16_t t1_counter, t1_latch;
 uint8_t ifr, ier;
} edu65xx_via_t;
void edu65xx_via_reset(edu65xx_via_t *via);
uint8_t edu65xx_via_read(edu65xx_via_t *via,uint8_t reg);
void edu65xx_via_write(edu65xx_via_t *via,uint8_t reg,uint8_t value);
uint8_t edu65xx_via_porta_pins(const edu65xx_via_t *via);
uint8_t edu65xx_via_portb_pins(const edu65xx_via_t *via);
void edu65xx_via_tick(edu65xx_via_t *via);
int edu65xx_via_irq(const edu65xx_via_t *via);
#endif
