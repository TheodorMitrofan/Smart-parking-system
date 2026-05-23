#ifndef SONAR_H
#define SONAR_H

#include <stdint.h>

void sonar_init();
uint16_t sonar_read_distance_cm();

#endif
