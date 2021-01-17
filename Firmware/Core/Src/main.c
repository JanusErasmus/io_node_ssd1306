/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "i2c.h"
#include "rtc.h"
#include "spi.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Utils/cli.h"
#include "stm32f1xx_it.h"
#include "ssd1306.h"
#include "dsb18b20.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void tx_usart2(uint8_t *data, int len)
{
	HAL_GPIO_WritePin(USART2_DE_GPIO_Port, USART2_DE_Pin, GPIO_PIN_SET);
	for (int k = 0; k < len; ++k)
	{
		if(HAL_UART_Transmit(&huart2, data, len, 100) != HAL_OK)
		{
			printf("Could not transmit USART2\n");
			break;
		}
	}
	HAL_GPIO_WritePin(USART2_DE_GPIO_Port, USART2_DE_Pin, GPIO_PIN_RESET);
}

static void make_output()
{
//    GPIO_InitTypeDef GPIO_InitStruct = {0};
//    GPIO_InitStruct.Pin = IN_7_Pin;
//    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
//    GPIO_InitStruct.Pull = GPIO_NOPULL;
//    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
//    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    // Streamlined configuration for PB07 as output OD
    // Set MODE7 = 0x03 (50 MHz Output) , CNF7 = 0x01 (open drain)
    GPIOB->CRL |= 0x03 << 28 ;
}

static void make_input()
{
//    GPIO_InitTypeDef GPIO_InitStruct = {0};
//    GPIO_InitStruct.Pin = IN_7_Pin;
//    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
//    GPIO_InitStruct.Pull = GPIO_NOPULL;
//    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
//    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    //Streamlined configuration for PB07 as input
    // Set MODE7 = 0x00 (Input) , CNF7 = 0x01 (floating input)
    GPIOB->CRL &= ~(0x03 << 28) ;
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */
  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART1_UART_Init();
  MX_ADC1_Init();
  MX_USART2_UART_Init();
  MX_RTC_Init();
  // MX_SPI1_Init();
  MX_I2C1_Init();
  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  printf("IO Node\n");
  printf("SYSCLOCK: %lu Hz\n", HAL_RCC_GetSysClockFreq());
  printf("HCLOCK  : %lu Hz\n", HAL_RCC_GetHCLKFreq());
  printf("APB1    : %lu Hz\n", HAL_RCC_GetPCLK1Freq());
  printf("APB2    : %lu Hz\n", HAL_RCC_GetPCLK2Freq());


  DS18B20_Init(IN_7_GPIO_Port, IN_7_Pin, make_input, make_output);

  setbuf(stdout, NULL);
  cli_init("io$ ");

//=========================== Init Display ====================================
  ssd1306_Init();
  ssd1306_SetCursor(0, 0);
  ssd1306_WriteString("Die \"Cute\" skerm", Font_7x10, White);
  ssd1306_SetCursor(0, 12);
  ssd1306_WriteString("Besig...", Font_11x18, White);
  ssd1306_UpdateScreen();
//=============================================================================
  USART1->CR1 |= USART_CR1_RXNEIE;
  USART1->CR3 |= USART_CR3_EIE;

  USART2->CR1 |= USART_CR1_RXNEIE;
  USART2->CR3 |= USART_CR3_EIE;


  char data[512];
  uint32_t tick =  HAL_GetTick() + 2000;
  float shown = 25;
  float ds_temp = 25;
  int sample_tick = 0;
  int display_tick = 50;
  float prev_temp = 0;
  float temp_2;
  while (1)
  {
	  cli_run();
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	  int rx = usart2_pop((uint8_t*)data, 512);
	  if(rx)
	  {
		  for (int k = 0; k < rx; ++k)
		  {
			  printf("RX %02X\n", data[k]);
		  }
	  }

	  if(tick < HAL_GetTick())
	  {
	      tick = HAL_GetTick() + 100;


	      if(sample_tick-- <= 0)
	      {
	          sample_tick = 60;
//	           cpp_report(1);

//=========================== Display temperature =============================
	          float voltages[8] = {0};
	          adc_sample(voltages);
	          float temp = (voltages[0] * 100.0) - 273.0;
              temp_2 = (voltages[1] * 100.0) - 273.0;
	          ds_temp = DS18B20_sample(true);
	          if(((prev_temp - 0.5) > temp) || (temp > (prev_temp + 0.5)))
	          {
	              prev_temp = temp;
	          }
	      }

	      if(((prev_temp - 0.1) > shown) || (shown > (prev_temp + 0.1)))
	      {
	          if(prev_temp > shown)
	              shown += 0.1;

              if(prev_temp < shown)
                  shown -= 0.1;
	      }

	      if(display_tick > 0)
	      {
	          ssd1306_Fill(Black);
	          sprintf(data, "LM: %0.1f", shown);
	          ssd1306_SetCursor(0, 0);
	          ssd1306_WriteString(data, Font_7x10, White);
              sprintf(data, "DS: %0.1f", ds_temp);
              ssd1306_SetCursor(0, 11);
              ssd1306_WriteString(data, Font_7x10, White);
              sprintf(data, "    %0.1f", ds_temp - shown);
              ssd1306_SetCursor(0, 22);
              ssd1306_WriteString(data, Font_7x10, White);
              sprintf(data, "LM2 %0.1f", temp_2);
              ssd1306_SetCursor(0, 53);
              ssd1306_WriteString(data, Font_7x10, White);
	          ssd1306_UpdateScreen();
	      }

//	      if(display_tick-- == 0)
//	      {
//	          ssd1306_Fill(Black);
//	          ssd1306_UpdateScreen();
//	      }
//
//	      uint8_t inputs;
//	      gpio_sample_in(&inputs);
//	      if((inputs & 0x01) == 0)
//	      {
//	          display_tick = 50;
//	      }
///============================================================================
	  }
  }

  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_RTC|RCC_PERIPHCLK_ADC;
  PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_HSE_DIV128;
  PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV6;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

#ifdef __GNUC__
/* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
 set to 'Yes') calls __io_putchar() */
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */

/**
 * @brief Retargets the C library printf function to the USART.
 * @param None
 * @retval None
 */
PUTCHAR_PROTOTYPE
{
  if(ch == '\n')
  {
    uint8_t cr = '\r';
    HAL_UART_Transmit(&huart1, &cr, 1, 0xFFFF);
  }

//  if(ch == '\r')
//  {
//    uint8_t cr = '\n';
//    HAL_UART_Transmit(&huart1, &cr, 1, 0xFFFF);
//  }


 HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, 0xFFFF);

return ch;
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
