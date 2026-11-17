/*
 * ClockHands.h
 *
 *  Created on: 2026. 10. 26.
 *      Author: kccistc
 */

#ifndef SRC_AP_CLOCKHANDS_CLOCKHANDS_H_
#define SRC_AP_CLOCKHANDS_CLOCKHANDS_H_

#include <stdint.h>

void ClockHands_init(void);
void ClockHands_setTime(uint8_t hour, uint8_t min, uint8_t sec);
void ClockHands_ISR(void);

#endif /* SRC_AP_CLOCKHANDS_CLOCKHANDS_H_ */
