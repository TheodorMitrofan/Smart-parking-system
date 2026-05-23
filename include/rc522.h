#ifndef RC522_H
#define RC522_H

#include <stdint.h>

#define CommandReg     0x01
#define ComIEnReg      0x02
#define ComIrqReg      0x04
#define ErrorReg       0x06
#define FIFODataReg    0x09
#define FIFOLevelReg   0x0A
#define BitFramingReg  0x0D
#define ModeReg        0x11
#define TxControlReg   0x14
#define TxAutoReg      0x15
#define TModeReg       0x2A
#define TPrescalerReg  0x2B
#define TReloadRegH    0x2C
#define TReloadRegL    0x2D

#define PCD_IDLE       0x00
#define PCD_TRANSCEIVE 0x0C
#define PCD_RESETPHASE 0x0F
#define PICC_REQIDL    0x26
#define PICC_ANTICOLL  0x93

void rc522_init();
void rc522_write(uint8_t reg, uint8_t val);
uint8_t rc522_read(uint8_t reg);
void rc522_antenna_on();
void rc522_antenna_off();
void rc522_reset_field();
uint8_t rc522_request(uint8_t* tagType);
uint8_t rc522_anticoll(uint8_t* uid);
uint8_t rc522_read_card(uint8_t* uid_out);

#endif
