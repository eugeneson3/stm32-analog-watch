/*
 * stepmotor.h
 *
 *  Created on: 2026. 10. 24.
 *      Author: kccistc
 */

#ifndef SRC_DRIVER_STEPMOTOR_STEPMOTOR_H_
#define SRC_DRIVER_STEPMOTOR_STEPMOTOR_H_

#include "stm32f4xx_hal.h"

#define STEP_COUNT				8		/* half-step 시퀀스 길이 */
#define STEPMOTOR_STEPS_PER_REV	4096	/* 28BYJ-48 half-step 기준 1바퀴 스텝 수 */

typedef struct {
	GPIO_TypeDef* port[4];	/* IN1 ~ IN4 */
	uint16_t pin[4];
}stepMotor_pin_t;

typedef struct {
	const stepMotor_pin_t *pin;
	uint16_t 		stepCounter;
}stepMotor_t;

typedef enum {
	STEPMOTOR_ORIENTATION_CW,
	STEPMOTOR_ORIENTATION_CCW
}stepMotor_orientation_t;

void StepMotor_init(stepMotor_t *m, const stepMotor_pin_t *pin);
void StepMotor_stepPhase(stepMotor_t *m, stepMotor_orientation_t orientation);
void StepMotor_release(stepMotor_t *m);

#endif /* SRC_DRIVER_STEPMOTOR_STEPMOTOR_H_ */
