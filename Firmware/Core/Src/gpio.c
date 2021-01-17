/**
  ******************************************************************************
  * @file    gpio.c
  * @brief   This file provides code for the configuration
  *          of all used GPIO pins.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "gpio.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/*----------------------------------------------------------------------------*/
/* Configure GPIO                                                             */
/*----------------------------------------------------------------------------*/
/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

/** Configure pins as
        * Analog
        * Input
        * Output
        * EVENT_OUT
        * EXTI
*/
void MX_GPIO_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, OUT_5_Pin|OUT_6_Pin|OUT_7_Pin|OUT_0_Pin
                          |OUT_1_Pin|OUT_2_Pin|OUT_3_Pin|OUT_4_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, USART2_DE_Pin|NRF_CE_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(IN_7_GPIO_Port, IN_7_Pin, GPIO_PIN_SET);

  /*Configure GPIO pins : PCPin PCPin PCPin PCPin
                           PCPin PCPin PCPin PCPin */
  GPIO_InitStruct.Pin = OUT_5_Pin|OUT_6_Pin|OUT_7_Pin|OUT_0_Pin
                          |OUT_1_Pin|OUT_2_Pin|OUT_3_Pin|OUT_4_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : PCPin PCPin PCPin PCPin
                           PCPin PCPin */
  GPIO_InitStruct.Pin = IN_1_Pin|IN_2_Pin|IN_3_Pin|IN_4_Pin
                          |IN_5_Pin|IN_0_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : PAPin PAPin */
  GPIO_InitStruct.Pin = USART2_DE_Pin|NRF_CE_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : PtPin */
  GPIO_InitStruct.Pin = NRF_IRQ_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(NRF_IRQ_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : PtPin */
  GPIO_InitStruct.Pin = SPI1_CS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(SPI1_CS_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : PBPin PBPin */
  GPIO_InitStruct.Pin = IN_6_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = IN_7_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

}

/* USER CODE BEGIN 2 */
HAL_StatusTypeDef gpio_sample_in(uint8_t *inputs)
{
	inputs[0] = HAL_GPIO_ReadPin(IN_0_GPIO_Port, IN_0_Pin);
	inputs[1] = HAL_GPIO_ReadPin(IN_1_GPIO_Port, IN_1_Pin);
	inputs[2] = HAL_GPIO_ReadPin(IN_2_GPIO_Port, IN_2_Pin);
	inputs[3] = HAL_GPIO_ReadPin(IN_3_GPIO_Port, IN_3_Pin);
	inputs[4] = HAL_GPIO_ReadPin(IN_4_GPIO_Port, IN_4_Pin);
	inputs[5] = HAL_GPIO_ReadPin(IN_5_GPIO_Port, IN_5_Pin);
	inputs[6] = HAL_GPIO_ReadPin(IN_6_GPIO_Port, IN_6_Pin);
	inputs[7] = HAL_GPIO_ReadPin(IN_7_GPIO_Port, IN_7_Pin);

	return HAL_OK;
}


HAL_StatusTypeDef gpio_set_out(uint8_t *outputs)
{
	HAL_GPIO_WritePin(OUT_0_GPIO_Port, OUT_0_Pin, outputs[0]);
	HAL_GPIO_WritePin(OUT_1_GPIO_Port, OUT_1_Pin, outputs[1]);
	HAL_GPIO_WritePin(OUT_2_GPIO_Port, OUT_2_Pin, outputs[2]);
	HAL_GPIO_WritePin(OUT_3_GPIO_Port, OUT_3_Pin, outputs[3]);
	HAL_GPIO_WritePin(OUT_4_GPIO_Port, OUT_4_Pin, outputs[4]);
	HAL_GPIO_WritePin(OUT_5_GPIO_Port, OUT_5_Pin, outputs[5]);
	HAL_GPIO_WritePin(OUT_6_GPIO_Port, OUT_6_Pin, outputs[6]);
	HAL_GPIO_WritePin(OUT_7_GPIO_Port, OUT_7_Pin, outputs[7]);

	return HAL_OK;
}
/* USER CODE END 2 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
