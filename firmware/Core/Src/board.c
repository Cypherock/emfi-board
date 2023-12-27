/*
 * board.c
 *
 *  Created on: Nov 5, 2023
 *      Author: akuan
 */
#include "board.h"

#include "pid.h"
ADC_HandleTypeDef hadc1;
DMA_HandleTypeDef hdma_adc1;

TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim6;

PID_TypeDef TPID;
system_info_t sysInfo;
uint8_t performShoot = 1;

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void)
{
	 RCC_OscInitTypeDef RCC_OscInitStruct = {0};
	  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

	  /** Configure the main internal regulator output voltage
	  */
	  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
	  {
	    Error_Handler();
	  }

	  /** Initializes the RCC Oscillators according to the specified parameters
	  * in the RCC_OscInitTypeDef structure.
	  */
	  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI48|RCC_OSCILLATORTYPE_MSI;
	  RCC_OscInitStruct.HSI48State = RCC_HSI48_ON;
	  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
	  RCC_OscInitStruct.MSICalibrationValue = 0;
	  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_6;
	  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_MSI;
	  RCC_OscInitStruct.PLL.PLLM = 1;
	  RCC_OscInitStruct.PLL.PLLN = 40;
	  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
	  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
	  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
	  {
	    Error_Handler();
	  }

	  /** Initializes the CPU, AHB and APB buses clocks
	  */
	  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
	                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
	  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
	  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

	  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
	  {
	    Error_Handler();
	  }
}

/**
 * @brief ADC1 Initialization Function
 * @param None
 * @retval None
 */
void MX_ADC1_Init(void)
{

	/* USER CODE BEGIN ADC1_Init 0 */

	/* USER CODE END ADC1_Init 0 */

	ADC_MultiModeTypeDef multimode = {0};
	ADC_ChannelConfTypeDef sConfig = {0};

	/* USER CODE BEGIN ADC1_Init 1 */

	/* USER CODE END ADC1_Init 1 */

	/** Common config
	 */
	hadc1.Instance = ADC1;
	hadc1.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV16;
	hadc1.Init.Resolution = ADC_RESOLUTION_12B;
	hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
	hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE;
	hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
	hadc1.Init.LowPowerAutoWait = DISABLE;
	hadc1.Init.ContinuousConvMode = ENABLE;
	hadc1.Init.NbrOfConversion = 1;
	hadc1.Init.DiscontinuousConvMode = DISABLE;
	hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
	hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
	hadc1.Init.DMAContinuousRequests = ENABLE;
	hadc1.Init.Overrun = ADC_OVR_DATA_PRESERVED;
	hadc1.Init.OversamplingMode = DISABLE;
	if (HAL_ADC_Init(&hadc1) != HAL_OK)
	{
		Error_Handler();
	}

	/** Configure the ADC multi-mode
	 */
	multimode.Mode = ADC_MODE_INDEPENDENT;
	if (HAL_ADCEx_MultiModeConfigChannel(&hadc1, &multimode) != HAL_OK)
	{
		Error_Handler();
	}

	/** Configure Regular Channel
	 */
	sConfig.Channel = ADC_CHANNEL_7;
	sConfig.Rank = ADC_REGULAR_RANK_1;
	sConfig.SamplingTime = ADC_SAMPLETIME_2CYCLES_5;
	sConfig.SingleDiff = ADC_SINGLE_ENDED;
	sConfig.OffsetNumber = ADC_OFFSET_NONE;
	sConfig.Offset = 0;
	if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
	{
		Error_Handler();
	}
	/* USER CODE BEGIN ADC1_Init 2 */
	HAL_ADCEx_Calibration_Start(&hadc1, ADC_SINGLE_ENDED);
	/* USER CODE END ADC1_Init 2 */

}

/**
 * @brief TIM2 Initialization Function
 * @param None
 * @retval None
 */
void MX_TIM2_Init(void)
{

	/* USER CODE BEGIN TIM2_Init 0 */

	/* USER CODE END TIM2_Init 0 */

	TIM_ClockConfigTypeDef sClockSourceConfig = {0};
	TIM_MasterConfigTypeDef sMasterConfig = {0};
	TIM_OC_InitTypeDef sConfigOC = {0};

	/* USER CODE BEGIN TIM2_Init 1 */

	/* USER CODE END TIM2_Init 1 */
	htim2.Instance = TIM2;
	htim2.Init.Prescaler = 1;
	htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim2.Init.Period = 7999;
	htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
	if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
	{
		Error_Handler();
	}
	sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
	if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
	{
		Error_Handler();
	}
	if (HAL_TIM_PWM_Init(&htim2) != HAL_OK)
	{
		Error_Handler();
	}
	sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
	{
		Error_Handler();
	}
	sConfigOC.OCMode = TIM_OCMODE_PWM1;
	sConfigOC.Pulse = 0;
	sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
	sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
	if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
	{
		Error_Handler();
	}
	if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
	{
		Error_Handler();
	}
	/* USER CODE BEGIN TIM2_Init 2 */

	/* USER CODE END TIM2_Init 2 */
	HAL_TIM_MspPostInit(&htim2);

}

/**
 * @brief TIM6 Initialization Function
 * @param None
 * @retval None
 */
void MX_TIM6_Init(void)
{

	/* USER CODE BEGIN TIM6_Init 0 */

	/* USER CODE END TIM6_Init 0 */

	TIM_MasterConfigTypeDef sMasterConfig = {0};

	/* USER CODE BEGIN TIM6_Init 1 */

	/* USER CODE END TIM6_Init 1 */
	htim6.Instance = TIM6;
	htim6.Init.Prescaler = 0;
	htim6.Init.CounterMode = TIM_COUNTERMODE_DOWN;
	htim6.Init.Period = 47999;
	htim6.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
	if (HAL_TIM_Base_Init(&htim6) != HAL_OK)
	{
		Error_Handler();
	}
	sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	if (HAL_TIMEx_MasterConfigSynchronization(&htim6, &sMasterConfig) != HAL_OK)
	{
		Error_Handler();
	}
	/* USER CODE BEGIN TIM6_Init 2 */

	/* USER CODE END TIM6_Init 2 */

}

uint8_t nopCount = 2;
void TIM6_Start(void)
{
	//TIM6->CCR1 = 40;
	//HAL_TIM_Base_Start(&htim6);

	GPIOA->BRR = GPIO_PIN_1;
//	for (int i = 0; i < nopCount; i++)
//	{
//		__NOP();
//	}
//	__NOP();
	__NOP();
	GPIOA->BSRR = GPIO_PIN_1;
	//HAL_TIM_Base_Stop(&htim6);
}
/**
 * Enable DMA controller clock
 */
void MX_DMA_Init(void)
{

	/* DMA controller clock enable */
	__HAL_RCC_DMA1_CLK_ENABLE();

	/* DMA interrupt init */
	/* DMA1_Channel1_IRQn interrupt configuration */
	HAL_NVIC_SetPriority(DMA1_Channel1_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(DMA1_Channel1_IRQn);

}

#define NV_DATA_SECTOR_ADDR			((uint32_t*)0x08010000)

/**
  * Read word from Non-Volatile memory
  */
void BSP_NonVolatileRead(uint32_t* dstAddr, uint32_t length)
{
    uint32_t* sourceAddr = NV_DATA_SECTOR_ADDR;

    for (int i=0; i < length; i++) {
        *dstAddr = ((uint32_t)(*sourceAddr));
        dstAddr++;
        sourceAddr++;
    }
}

HAL_StatusTypeDef BSP_FlashSectorErase(uint32_t page_address, uint32_t noOfpages)
{
    FLASH_EraseInitTypeDef ptEraseTypeInfo;
    uint32_t sectorError;
    HAL_StatusTypeDef status = HAL_OK;

    ptEraseTypeInfo.TypeErase = FLASH_TYPEERASE_PAGES;
    ptEraseTypeInfo.NbPages = noOfpages;
    ptEraseTypeInfo.Page = page_address;
    HAL_FLASH_Unlock();
    if (HAL_FLASHEx_Erase(&ptEraseTypeInfo, &sectorError) != HAL_OK)
        status = HAL_ERROR;
    HAL_FLASH_Lock();

    return status;
}

HAL_StatusTypeDef BSP_FlashSectorWrite(__IO uint32_t *dstAddr, uint32_t* srcAddr, uint32_t noOfWords)
{
	HAL_StatusTypeDef status = HAL_OK;

    HAL_FLASH_Unlock();
    for (int i = 0; i < noOfWords; i++) {
        if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_FAST, (uint32_t)(dstAddr), (uint64_t)(*srcAddr)) == HAL_OK) {
            if (*srcAddr != *dstAddr) {
                status = HAL_ERROR;
                break;
            }
            srcAddr++;
            dstAddr++;
        }
        else
            status = HAL_ERROR;
    }
    HAL_FLASH_Lock();

    return status;
}


HAL_StatusTypeDef BSP_NVSysWrite(uint32_t* srcAddr, uint32_t noOfWords)
{
	HAL_StatusTypeDef status = HAL_OK;
	status = BSP_FlashSectorErase(3, 1);
	if (status != HAL_OK)
		return status;
	status = BSP_FlashSectorWrite(NV_DATA_SECTOR_ADDR, srcAddr, noOfWords);
	return status;
}

void BSP_NVSysRead(uint32_t* dstAddr, uint32_t length)
{
	uint32_t* sourceAddr = NV_DATA_SECTOR_ADDR;

	for (int i=0; i < length; i++){
		*dstAddr = ((uint32_t)(*sourceAddr));
		dstAddr++;
		sourceAddr++;
	}
}

/**
 * @brief GPIO Initialization Function
 * @param None
 * @retval None
 */
void MX_GPIO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	/* GPIO Ports Clock Enable */
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(GPIOB, LED1_PIN_Pin|LED2_PIN_Pin|LED3_PIN_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_SET);

	/*Configure GPIO pin : PA3 */
	GPIO_InitStruct.Pin = GPIO_PIN_3;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	/*Configure GPIO pins : LED1_PIN_Pin LED2_PIN_Pin LED3_PIN_Pin */
	GPIO_InitStruct.Pin = LED1_PIN_Pin|LED2_PIN_Pin|LED3_PIN_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	/*Configure GPIO pins : GPIO_PIN_1 */
	GPIO_InitStruct.Pin = GPIO_PIN_1;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_SET);
}

/* USER CODE BEGIN 4 */
uint16_t BSP_DMA_Count(void)
{
	uint16_t count =  __HAL_DMA_GET_COUNTER(&hdma_adc1);
	return count;
}

void BSP_TIM_PWM_CH1_Start(void)
{
	HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);
}

void BSP_TIM_PWM_CH2_Start(void)
{
	HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2);
}

void BSP_Start_ADC_IT(void)
{

	HAL_ADC_Start_IT(&hadc1);
}

HAL_StatusTypeDef BSP_ReadVoltage(uint16_t *pData, uint16_t Size)
{
	HAL_StatusTypeDef status = HAL_OK;
	status = HAL_ADC_Start_DMA(&hadc1, (uint32_t*)pData, Size);
	if (status != HAL_OK)
		return HAL_ERROR;
	return HAL_OK;
}

void HAL_TickCallBack(void)
{


}
uint8_t isPWM_Generation_Start = 0;
uint8_t isPWM_Stoped = 0;
uint32_t adc_set_point_value = 400;


void update_dutyCycle()
{
	int diffValue =  adc_set_point_value - sysInfo.adcDataReadBuffer;

	uint32_t percentValue = ((float)diffValue / adc_set_point_value) *100;
	if (percentValue < 0)
	{
		HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_1);
	}
	else if ((percentValue < 40) && percentValue)
	{
		TIM2->CCR1  = (39) *percentValue;
	}
	else if (percentValue == 0)
	{
		TIM2->CCR1 = (39);
	}
	else
	{
		TIM2->CCR1 = 1599;
	}

}
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
	// Read & Update The ADC Result
	sysInfo.adcDataReadBuffer = HAL_ADC_GetValue(&hadc1);
	sysInfo.adcDataReadBuffer += (sysInfo.PIDOut / 100);
	//PID_Compute(&TPID);
	if (isPWM_Generation_Start)
	{
		if (sysInfo.adcDataReadBuffer > adc_set_point_value)
		{
			if (isPWM_Stoped == 0)
			{
				HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_1);
				HAL_GPIO_WritePin(GPIOB,LED3_PIN_Pin, GPIO_PIN_SET);
				isPWM_Stoped = 1;
			}
		}
		else
		{
			if (isPWM_Stoped)
			{
				//sysInfo.CH1_ARR = ((3*pwmValue->pwmWidth) - 1);
				//sysInfo.CH1_DC = (((sysInfo.CH1_ARR + 1) /100) *(pwmValue->pwmPeriod));
				TIM2->CCR1 = 399;
				TIM2->ARR = 3999;
				HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);
				isPWM_Stoped = 0;
			}

			HAL_GPIO_WritePin(GPIOB,LED3_PIN_Pin, GPIO_PIN_RESET);

			//update_dutyCycle();


		}
	}
}


void BSP_Config_PID(void)
{

	PID(&TPID, &sysInfo.Volt, &sysInfo.PIDOut, &sysInfo.VoltSetpoint, 2, 5, 1, _PID_P_ON_E, _PID_CD_DIRECT);

	PID_SetMode(&TPID, _PID_MODE_AUTOMATIC);
	PID_SetSampleTime(&TPID, 500);
	PID_SetOutputLimits(&TPID, -100, 100);
}

#include "usbd_cdc_if.h"
uint8_t SendBuffer[128] = { 0 };
uint8_t commandResponce = 0;
typedef struct
{
	uint16_t pwmWidth;
	uint16_t pwmPeriod;

}pwm_setting_t;
pwm_setting_t pwmSetting = { 800, 1};
uint8_t dataRead[128] = { 0 };
void parse_cmd(uint8_t cmd, uint8_t * data, uint8_t size)
{
	switch(cmd)
	{
		case ACTIVATE_VOLT_GENERATER:
		{
			commandResponce = cmd;
			CDC_Transmit_FS((uint8_t *)&commandResponce, sizeof(uint8_t));
			uint8_t * value = (uint8_t *)data;
			isPWM_Generation_Start = *value;
			isPWM_Stoped = 1;
			//isPWM_Generation_Start = 1;
			break;
		}

		case GET_RAW_VALUE:
		{
			uint16_t readAdc = sysInfo.adcDataReadBuffer;
			CDC_Transmit_FS((uint8_t *)&readAdc, sizeof(uint16_t));
			break;
		}

		case SET_PWM_VALUE:
		{

			CDC_Transmit_FS((uint8_t *)&cmd, 1);
			pwm_setting_t * pwmValue = (pwm_setting_t *)data;

			sysInfo.CH1_ARR = ((3*pwmValue->pwmWidth) - 1);
			sysInfo.CH1_DC = (((sysInfo.CH1_ARR + 1) /100) *(pwmValue->pwmPeriod));
			//BSP_TIM_PWM_CH1_Start();
			break;
		}


		case PERFORM_SOFTWARE_SHOOT:
			CDC_Transmit_FS((uint8_t *)&cmd, 1);
			TIM6_Start();
			nopCount++;
			performShoot = 1;
			break;
		case GET_SYSTEM_TIME:
		{
			uint64_t currentTime = HAL_GetTick();
			memcpy(SendBuffer,(uint8_t *)&currentTime,sizeof(uint64_t));
			CDC_Transmit_FS((uint8_t *)&SendBuffer, sizeof(uint64_t));
		}
			break;
		case GET_ADC_CONTROL_SETPOINT:
			CDC_Transmit_FS((uint8_t *)&sysInfo.pidLimit.setpoint, sizeof(uint32_t));
			break;
		case SET_ADC_CONTROL_SETPOINT:
			break;
		case GET_PWM_VALUE:

			CDC_Transmit_FS((uint8_t *)&pwmSetting.pwmWidth, sizeof(uint32_t));
			pwmSetting.pwmPeriod = 5;
			CDC_Transmit_FS((uint8_t *)&pwmSetting.pwmPeriod, sizeof(uint32_t));
			break;
		case READ_PID_VALUE:
			memcpy(SendBuffer,(uint8_t *)&sysInfo.pidConfig,sizeof(pid_config_t));
			CDC_Transmit_FS((uint8_t *)&SendBuffer, sizeof(pid_config_t));
			break;
		case SET_PID_VALUE:
			break;
		case SET_PID_LIMIT_VALUE:
			break;
		case READ_PID_LIMIT_VALUE:
			memcpy(SendBuffer,(uint8_t *)&sysInfo.pidLimit,sizeof(pid_limit_t));
			CDC_Transmit_FS((uint8_t *)&SendBuffer, sizeof(pid_limit_t));
			break;
		case SET_ADC_SET_POINT:
		{
			uint16_t * value = (uint16_t *)data;
			adc_set_point_value = *value;
			break;
		}
	}
}
/* USER CODE END 4 */
