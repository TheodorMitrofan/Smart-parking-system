#ifndef SERVO_H
#define SERVO_H

#include <stdint.h>

/* Servo CONTINUOUS-ROTATION pe OC1A (PB1).
 *   OCR1A = 3000 (1.5ms) -> STOP
 *   OCR1A < 3000         -> rotire intr-un sens
 *   OCR1A > 3000         -> rotire in sens opus
 *
 * SERVO_INCHIS / SERVO_DESCHIS sunt doar etichete logice — driver-ul
 * roteste cronometrat in directia corespunzatoare, apoi opreste motorul. */
#define SERVO_INCHIS   0
#define SERVO_DESCHIS  1

void servo_init();
void servo_set_position(uint16_t target);

#endif
