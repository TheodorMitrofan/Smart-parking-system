#include "config.h"
#include "rc522.h"
#include "spi.h"
#include <avr/io.h>
#include <util/delay.h>
#include <string.h>

void rc522_write(uint8_t reg, uint8_t val) {
    PORTB &= ~(1 << SS_PIN);
    spi_transfer((reg << 1) & 0x7E);
    spi_transfer(val);
    PORTB |= (1 << SS_PIN);
}

uint8_t rc522_read(uint8_t reg) {
    PORTB &= ~(1 << SS_PIN);
    spi_transfer(((reg << 1) & 0x7E) | 0x80);
    uint8_t val = spi_transfer(0);
    PORTB |= (1 << SS_PIN);
    return val;
}

static void rc522_set_bits(uint8_t reg, uint8_t mask) {
    rc522_write(reg, rc522_read(reg) | mask);
}

static void rc522_clear_bits(uint8_t reg, uint8_t mask) {
    rc522_write(reg, rc522_read(reg) & (~mask));
}

void rc522_antenna_off() {
    rc522_clear_bits(TxControlReg, 0x03);
}

void rc522_antenna_on() {
    rc522_set_bits(TxControlReg, 0x03);
}

void rc522_init() {
    PORTD |= (1 << RST_PIN);
    _delay_ms(50);
    rc522_write(CommandReg, PCD_RESETPHASE);
    _delay_ms(50);
    rc522_write(TModeReg, 0x8D);
    rc522_write(TPrescalerReg, 0x3E);
    rc522_write(TReloadRegL, 30);
    rc522_write(TReloadRegH, 0);
    rc522_write(TxAutoReg, 0x40);
    rc522_write(ModeReg, 0x3D);
    rc522_antenna_off();
    _delay_ms(10);
    rc522_antenna_on();
    _delay_ms(10);
}

void rc522_reset_field() {
    rc522_antenna_off();
    _delay_ms(100);
    rc522_antenna_on();
    _delay_ms(50);
}

static uint8_t rc522_to_card(uint8_t cmd, uint8_t* sendData, uint8_t sendLen,
                             uint8_t* backData, uint8_t* backLen) {
    uint8_t status = 1;
    uint8_t waitIRq = 0x30;

    rc522_write(ComIEnReg, 0x77 | 0x80);
    rc522_clear_bits(ComIrqReg, 0x80);
    rc522_set_bits(FIFOLevelReg, 0x80);
    rc522_write(CommandReg, PCD_IDLE);

    for (uint8_t i = 0; i < sendLen; i++) rc522_write(FIFODataReg, sendData[i]);

    rc522_write(CommandReg, cmd);
    if (cmd == PCD_TRANSCEIVE) rc522_set_bits(BitFramingReg, 0x80);

    uint16_t i = 2000;
    uint8_t n;
    do { n = rc522_read(ComIrqReg); i--; }
    while ((i != 0) && !(n & 0x01) && !(n & waitIRq));

    rc522_clear_bits(BitFramingReg, 0x80);

    if (i != 0 && !(rc522_read(ErrorReg) & 0x1B)) {
        status = 0;
        if (cmd == PCD_TRANSCEIVE) {
            n = rc522_read(FIFOLevelReg);
            if (n > 16) n = 16;
            *backLen = n;
            for (uint8_t j = 0; j < n; j++) backData[j] = rc522_read(FIFODataReg);
        }
    }
    return status;
}

uint8_t rc522_request(uint8_t* tagType) {
    uint8_t backLen;
    rc522_write(BitFramingReg, 0x07);
    uint8_t cmd[1] = { PICC_REQIDL };
    return rc522_to_card(PCD_TRANSCEIVE, cmd, 1, tagType, &backLen);
}

uint8_t rc522_anticoll(uint8_t* uid) {
    uint8_t backLen;
    rc522_write(BitFramingReg, 0x00);
    uint8_t cmd[2] = { PICC_ANTICOLL, 0x20 };
    uint8_t status = rc522_to_card(PCD_TRANSCEIVE, cmd, 2, uid, &backLen);
    if (status == 0) {
        uint8_t check = 0;
        for (uint8_t i = 0; i < 4; i++) check ^= uid[i];
        if (check != uid[4]) status = 1;
    }
    return status;
}

static uint8_t uid_is_valid(uint8_t* uid) {
    uint8_t all_zero = 1, all_ff = 1;
    for (uint8_t i = 0; i < 4; i++) {
        if (uid[i] != 0x00) all_zero = 0;
        if (uid[i] != 0xFF) all_ff = 0;
    }
    return !all_zero && !all_ff;
}

uint8_t rc522_read_card(uint8_t* uid_out) {
    uint8_t uid1[5], uid2[5];
    uint8_t tagType[2];

    if (rc522_request(tagType) != 0) return 1;
    if (rc522_anticoll(uid1) != 0) return 1;
    if (!uid_is_valid(uid1)) return 1;

    _delay_ms(50);

    if (rc522_request(tagType) != 0) return 1;
    if (rc522_anticoll(uid2) != 0) return 1;
    if (memcmp(uid1, uid2, 4) != 0) return 1;

    memcpy(uid_out, uid1, 5);
    return 0;
}
