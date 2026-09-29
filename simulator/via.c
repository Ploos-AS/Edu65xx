#include "via.h"
void edu65xx_via_reset(edu65xx_via_t *v){v->orb=v->ora=v->ddrb=v->ddra=v->input_b=v->input_a=0u;v->t1_counter=v->t1_latch=0u;v->ifr=v->ier=0u;}
static uint8_t port(uint8_t o,uint8_t d,uint8_t i){return(uint8_t)((o&d)|(i&(uint8_t)~d));}
int edu65xx_via_irq(const edu65xx_via_t *v){return (v->ifr & v->ier & 0x7Fu)!=0u;}
uint8_t edu65xx_via_read(edu65xx_via_t *v,uint8_t r){
 switch(r&15u){
 case 0:return port(v->orb,v->ddrb,v->input_b); case 1:return port(v->ora,v->ddra,v->input_a);
 case 2:return v->ddrb; case 3:return v->ddra;
 case 4:{uint8_t x=(uint8_t)v->t1_counter;v->ifr&=(uint8_t)~EDU65XX_VIA_IFR_T1;return x;}
 case 5:return(uint8_t)(v->t1_counter>>8);
 case 6:return(uint8_t)v->t1_latch; case 7:return(uint8_t)(v->t1_latch>>8);
 case 13:return(uint8_t)(v->ifr|(edu65xx_via_irq(v)?0x80u:0u));
 case 14:return(uint8_t)(v->ier|0x80u);
 default:return 0u;
 }}
void edu65xx_via_write(edu65xx_via_t *v,uint8_t r,uint8_t x){
 switch(r&15u){
 case 0:v->orb=x;break;case 1:v->ora=x;break;case 2:v->ddrb=x;break;case 3:v->ddra=x;break;
 case 4:v->t1_latch=(uint16_t)((v->t1_latch&0xFF00u)|x);break;
 case 5:v->t1_latch=(uint16_t)(((uint16_t)x<<8)|(v->t1_latch&0x00FFu));v->t1_counter=v->t1_latch;v->ifr&=(uint8_t)~EDU65XX_VIA_IFR_T1;break;
 case 13:v->ifr&=(uint8_t)~(x&0x7Fu);break;
 case 14:if((x&0x80u)!=0u)v->ier|=(uint8_t)(x&0x7Fu);else v->ier&=(uint8_t)~(x&0x7Fu);break;
 default:break;
 }}
void edu65xx_via_tick(edu65xx_via_t *v){if(v->t1_counter>0u){--v->t1_counter;if(v->t1_counter==0u)v->ifr|=EDU65XX_VIA_IFR_T1;}}
uint8_t edu65xx_via_porta_pins(const edu65xx_via_t *v){return(uint8_t)(v->ora&v->ddra);}
uint8_t edu65xx_via_portb_pins(const edu65xx_via_t *v){return(uint8_t)(v->orb&v->ddrb);}
