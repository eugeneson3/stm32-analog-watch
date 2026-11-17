/*
 * ClockHands.c
 *
 *  Created on: 2026. 10. 26.
 *      Author: kccistc
 *
 *  스텝모터 3개로 시/분/초 바늘을 아날로그로 표시한다.
 *  - 전원 인가 시 세 바늘 모두 12시(위쪽) = 0시 0분 0초 위치에 있다고 가정
 *  - 바늘 위치는 0 ~ STEPMOTOR_STEPS_PER_REV-1 스텝(12시 = 0)으로 관리
 *  - TIM9 인터럽트마다 각 바늘을 목표 위치 쪽으로 1스텝씩 이동 (가까운 방향)
 */

#include "ClockHands.h"
#include "../../driver/stepmotor/stepmotor.h"

/* TIM9 tick = 1 MHz (prescaler 100 on 100 MHz). One motor step per update event.
 * 28BYJ-48 half-step must stay slower than ~2 ms/step or it loses steps. */
#define CLOCKHANDS_PERIOD_US	2000u

/* 바늘이 반대로 돌면 CW <-> CCW 를 바꾼다 */
#define CLOCKHANDS_DIR_FWD		STEPMOTOR_ORIENTATION_CCW
#define CLOCKHANDS_DIR_BWD		STEPMOTOR_ORIENTATION_CW

enum {HAND_HOUR, HAND_MIN, HAND_SEC, HAND_NUM};

extern TIM_HandleTypeDef htim9;

static const stepMotor_pin_t stepMotorPin[HAND_NUM] = {
	[HAND_HOUR] = {	/* stepmotor 1 */
		.port = {GPIOA, GPIOA, GPIOA, GPIOB},
		.pin  = {GPIO_PIN_0, GPIO_PIN_1, GPIO_PIN_4, GPIO_PIN_0}
	},
	[HAND_MIN] = {	/* stepmotor 2 */
		.port = {GPIOC, GPIOC, GPIOC, GPIOD},
		.pin  = {GPIO_PIN_10, GPIO_PIN_11, GPIO_PIN_12, GPIO_PIN_2}
	},
	[HAND_SEC] = {	/* stepmotor 3 */
		.port = {GPIOC, GPIOC, GPIOC, GPIOC},
		.pin  = {GPIO_PIN_9, GPIO_PIN_8, GPIO_PIN_6, GPIO_PIN_5}
	},
};

typedef struct {
	stepMotor_t motor;
	uint16_t curPos;			/* 현재 바늘 위치 (step) */
	volatile uint16_t targetPos;	/* 목표 바늘 위치 (step) */
	uint8_t released;
} clockHand_t;

static clockHand_t hands[HAND_NUM];

static void ClockHands_setPeriodUs(uint16_t us)
{
	__HAL_TIM_SET_AUTORELOAD(&htim9, us - 1);
	__HAL_TIM_SET_COUNTER(&htim9, 0);	/* avoid waiting for a full 16-bit wrap */
}

void ClockHands_init(void)
{
	for (int i = 0; i < HAND_NUM; i++) {
		StepMotor_init(&hands[i].motor, &stepMotorPin[i]);
		hands[i].curPos = 0;		/* 시작 시 12시 방향 */
		hands[i].targetPos = 0;
		hands[i].released = 1;
	}
	ClockHands_setPeriodUs(CLOCKHANDS_PERIOD_US);
	HAL_TIM_Base_Start_IT(&htim9);
}

/* 시/분/초 -> 각 바늘의 목표 스텝 위치
 * 시침: 12시간 1바퀴 (분/초만큼 조금씩 진행)
 * 분침: 60분 1바퀴 (초만큼 조금씩 진행)
 * 초침: 60초 1바퀴 */
void ClockHands_setTime(uint8_t hour, uint8_t min, uint8_t sec)
{
	uint32_t hourSec = ((hour % 12) * 3600UL) + (min * 60UL) + sec;
	uint32_t minSec  = (min * 60UL) + sec;

	hands[HAND_HOUR].targetPos = (uint16_t)(hourSec * STEPMOTOR_STEPS_PER_REV / 43200UL);
	hands[HAND_MIN].targetPos  = (uint16_t)(minSec  * STEPMOTOR_STEPS_PER_REV / 3600UL);
	hands[HAND_SEC].targetPos  = (uint16_t)(sec     * STEPMOTOR_STEPS_PER_REV / 60UL);
}

static void ClockHands_moveHand(clockHand_t *h)
{
	uint16_t diff = (h->targetPos + STEPMOTOR_STEPS_PER_REV - h->curPos) % STEPMOTOR_STEPS_PER_REV;

	if (diff == 0) {
		if (!h->released) {
			StepMotor_release(&h->motor);
			h->released = 1;
		}
		return;
	}

	h->released = 0;
	if (diff <= STEPMOTOR_STEPS_PER_REV / 2) {	/* 가까운 방향으로 회전 */
		StepMotor_stepPhase(&h->motor, CLOCKHANDS_DIR_FWD);
		h->curPos = (h->curPos + 1) % STEPMOTOR_STEPS_PER_REV;
	}
	else {
		StepMotor_stepPhase(&h->motor, CLOCKHANDS_DIR_BWD);
		h->curPos = (h->curPos + STEPMOTOR_STEPS_PER_REV - 1) % STEPMOTOR_STEPS_PER_REV;
	}
}

/* 메인 루프의 LCD/I2C, 버튼 debounce 지연에 영향받지 않도록 ISR에서 직접 스텝 */
void ClockHands_ISR(void)
{
	for (int i = 0; i < HAND_NUM; i++) {
		ClockHands_moveHand(&hands[i]);
	}
}
