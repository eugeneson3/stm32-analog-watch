/*
 * stepmotor.c
 *
 *  Created on: 2026. 10. 24.
 *      Author: kccistc
 */

#include "stepmotor.h"

static const uint8_t step_table[STEP_COUNT] = {0x01,0x03,0x02,0x06,0x04,0x0c,0x08,0x09}; // IN4,IN3,IN2,IN1

static void StepMotor_clkEnable(GPIO_TypeDef *port)
{
	if      (port == GPIOA) __HAL_RCC_GPIOA_CLK_ENABLE();
	else if (port == GPIOB) __HAL_RCC_GPIOB_CLK_ENABLE();
	else if (port == GPIOC) __HAL_RCC_GPIOC_CLK_ENABLE();
	else if (port == GPIOD) __HAL_RCC_GPIOD_CLK_ENABLE();
}

static void StepMotor_writePattern(stepMotor_t *m, uint8_t step)
{
	for (int i = 0; i < 4; i++){
		GPIO_PinState s = (step & (1<<i)) ? GPIO_PIN_SET : GPIO_PIN_RESET;
		HAL_GPIO_WritePin(m->pin->port[i], m->pin->pin[i], s);
	}
}

/* 핀을 출력으로 설정한다 (CubeMX에 핀 설정이 없어도 동작하도록 여기서 직접 초기화) */
void StepMotor_init(stepMotor_t *m, const stepMotor_pin_t *pin)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	m->pin = pin;
	m->stepCounter = 0;

	for (int i = 0; i < 4; i++){
		StepMotor_clkEnable(pin->port[i]);
		HAL_GPIO_WritePin(pin->port[i], pin->pin[i], GPIO_PIN_RESET);

		GPIO_InitStruct.Pin = pin->pin[i];
		GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
		HAL_GPIO_Init(pin->port[i], &GPIO_InitStruct);
	}
}

void StepMotor_stepPhase(stepMotor_t *m, stepMotor_orientation_t orientation)
{
	if (orientation == STEPMOTOR_ORIENTATION_CW){
		/* + STEP_COUNT first: 0 - 1 would be -1 and index step_table out of range */
		m->stepCounter = (m->stepCounter + STEP_COUNT - 1) % STEP_COUNT;
	}
	else{
		m->stepCounter = (m->stepCounter + 1) % STEP_COUNT;
	}
	StepMotor_writePattern(m, step_table[m->stepCounter]);
}

/* 코일 전류 차단 (정지 중 발열/전류 감소) */
void StepMotor_release(stepMotor_t *m)
{
	StepMotor_writePattern(m, 0x00);
}
