/*
 * board.h
 *
 *  Created on: Nov 5, 2023
 *      Author: akuan
 */

#ifndef INC_BOARD_H_
#define INC_BOARD_H_


#include "stdbool.h"
#include "main.h"

typedef enum
{
	ACTIVATE_VOLT_GENERATER = 1,
	GET_RAW_VALUE,
	SET_PWM_VALUE,
	PERFORM_SOFTWARE_SHOOT,
	GET_SYSTEM_TIME,
	GET_ADC_CONTROL_SETPOINT,
	SET_ADC_CONTROL_SETPOINT,
	GET_PWM_VALUE,
	READ_PID_VALUE,
	SET_PID_VALUE,
	SET_PID_LIMIT_VALUE,
	READ_PID_LIMIT_VALUE,
	SET_ADC_SET_POINT,

} sys_cmd_t;
typedef struct
{
	uint32_t kp;
	uint32_t ki;
	uint32_t kd;
	uint32_t control_ticks;
	uint32_t temp;
} pid_config_t;

typedef struct
{
	uint32_t p_limit;
	uint32_t i_limit;
	uint32_t d_limit;
	uint32_t output_limit;
	uint32_t setpoint;
	uint32_t last_control;
	uint32_t temp;
} pid_limit_t;

typedef struct
{
	pid_config_t pidConfig;
	pid_limit_t pidLimit;
	bool high_voltage_enabled;
	uint32_t softwareShootDuration;
	double Volt;
	double PIDOut;
	double VoltSetpoint;
	uint32_t adcDataReadBuffer;
	int32_t CH1_DC;
	int32_t CH1_ARR;
} system_info_t;

extern system_info_t sysInfo;
#define LED_ONE_ON  HAL_GPIO_WritePin(GPIOB,LED1_PIN_Pin, GPIO_PIN_SET)
#define LED_ONE_OFF  HAL_GPIO_WritePin(GPIOB,LED1_PIN_Pin, GPIO_PIN_RESET)
#define LED_TWO_ON  HAL_GPIO_WritePin(GPIOB,LED2_PIN_Pin, GPIO_PIN_SET)
#define LED_TWO_OFF  HAL_GPIO_WritePin(GPIOB,LED2_PIN_Pin, GPIO_PIN_RESET)
#define LED_THREE_ON  HAL_GPIO_WritePin(GPIOB,LED3_PIN_Pin, GPIO_PIN_SET)
#define LED_THREE_OFF  HAL_GPIO_WritePin(GPIOB,LED3_PIN_Pin, GPIO_PIN_RESET)

void SystemClock_Config(void);
void MX_GPIO_Init(void);
void MX_DMA_Init(void);
void MX_ADC1_Init(void);
void MX_TIM2_Init(void);
void MX_TIM6_Init(void);

void TIM6_Start(void);

void BSP_Start_ADC_IT(void);
void BSP_TIM_PWM_CH1_Start(void);
void BSP_TIM_PWM_CH2_Start(void);

HAL_StatusTypeDef BSP_ReadVoltage(uint16_t *pData, uint16_t Size);

void BSP_Config_PID(void);
void parse_cmd(uint8_t cmd, uint8_t * data, uint8_t size);
void HAL_TickCallBack(void);

#endif /* INC_BOARD_H_ */
