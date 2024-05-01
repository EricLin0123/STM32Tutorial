/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2024 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "dma.h"
#include "i2c.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include "screen.h"
#include "ws2812.h"
#include "imu.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define isblink 1
#define DMAdemo 0
#define isDMA 0
#define isADC 0
#define isDMA_ADC
#define isPWM 0
#define isWS2812 0
#define isIMU 0
#define isUARTdemo 0
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
int _write(int32_t file, uint8_t *ptr, int32_t len)
{
  for (int i = 0; i < len; i++)
  {
    ITM_SendChar(*ptr++);
  }
  return len;
}
void increment(uint8_t *a);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
const uint8_t a = 0x4C;
/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void)
{

  /* USER CODE BEGIN 1 */
  char buffer[20];
#if (isDMA)
  uint8_t DataA[] = {0x01, 0x02, 0x03, 0x04};
  uint8_t DataB[] = {0, 0, 0, 0};
#endif

#if (isADC || isDMA_ADC)
  uint32_t adcValue;
#endif

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */
#if (isADC || isDMA_ADC)
  HAL_ADC_MspInit(&hadc1);
#endif
  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_ADC1_Init();
  MX_I2C1_Init();
  MX_TIM1_Init();
  MX_TIM2_Init();
  MX_SPI1_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */
  OLED_Init();
  OLED_Clear();
  printf("Hello from printf!\n");
  OLED_ShowString(0, 0, "Hello from i2c!", 8);
  HAL_UART_Transmit(&huart2, (uint8_t *)"Hello from UART!\n", 17, HAL_MAX_DELAY);
#if (DMAdemo)
  const uint8_t b = 0x00;
  OLED_ShowString(0, 0, "Hello, World!", 8);
  sprintf(buffer, "%08x", &ADC1->DR);
  OLED_ShowString(0, 1, buffer, 8);
  sprintf(buffer, "%x", a);
  OLED_ShowString(0, 2, buffer, 8);
  sprintf(buffer, "%08x", &a);
  OLED_ShowString(0, 3, buffer, 8);
  sprintf(buffer, "%x", b);
  OLED_ShowString(0, 4, buffer, 8);
  sprintf(buffer, "%08x", &b);
  OLED_ShowString(0, 5, buffer, 8);
#endif

#if (isPWM)
  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
#endif
#if (isIMU)
  while (ADXL345_Init() != HAL_OK)
    ;
#endif
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
#if (isblink)
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET);
    HAL_Delay(500);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET);
    HAL_Delay(500);
#endif

#if (isDMA)
    OLED_ShowString(0, 0, "Data A", 8);
    sprintf(buffer, "ADDR: %02x", &DataA);
    OLED_ShowString(0, 1, buffer, 8);
    sprintf(buffer, "%02x %02x %02x %02x", DataA[0], DataA[1], DataA[2], DataA[3]);
    OLED_ShowString(0, 2, buffer, 8);
    // start DMA transfer
    HAL_DMA_Start(&hdma_memtomem_dma1_channel2, (uint32_t)DataA, (uint32_t)DataB, 4);
    // wait for DMA transfer complete
    while (HAL_DMA_PollForTransfer(&hdma_memtomem_dma1_channel2, HAL_DMA_FULL_TRANSFER, HAL_MAX_DELAY) != HAL_OK)
      ;
    // after DMA transfer
    OLED_ShowString(0, 3, "Data B", 8);
    sprintf(buffer, "ADDR: %02x", &DataB);
    OLED_ShowString(0, 4, buffer, 8);
    sprintf(buffer, "%02x %02x %02x %02x", DataB[0], DataB[1], DataB[2], DataB[3]);
    OLED_ShowString(0, 5, buffer, 8);
    increment(DataA);
#endif

#if (isADC)
    HAL_ADC_Start(&hadc1);
    while (HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY))
      ;
    adcValue = HAL_ADC_GetValue(&hadc1);
    OLED_ShowString(0, 0, "ADC Value", 8);
    sprintf(buffer, "%d", adcValue);
    OLED_ShowString(0, 1, buffer, 8);
#endif

/* DMA ADC begin */
#if (isDMA_ADC)
    HAL_ADC_Start_DMA(&hadc1, &adcValue, 1);
    OLED_ShowString(0, 0, "ADC Value", 8);
    adcValue = (adcValue & 0x0000FFFF);
    sprintf(buffer, "%04d", adcValue);
    OLED_ShowString(0, 1, buffer, 8);
#endif

#if (isPWM)
    HAL_ADC_Start_DMA(&hadc1, (uint32_t *)&TIM1->CCR1, 1);
    HAL_ADC_Start_DMA(&hadc1, (uint32_t *)&TIM1->CCR2, 1);
#endif

#if (isWS2812)
    // ws2812_example();
    ws2812_test();
#endif

#if (isIMU)
    ADXL345_Test();
#endif

#if (isUARTdemo)
    if (HAL_UART_Receive(&huart2, (uint8_t *)buffer, 1, HAL_MAX_DELAY) == HAL_OK)
    {
      HAL_UART_Transmit(&huart2, (uint8_t *)buffer, 1, HAL_MAX_DELAY);
      HAL_UART_Transmit(&huart2, (uint8_t *)"\r\n\r", 1, HAL_MAX_DELAY);
    }
#endif
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
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
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
  PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV8;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }

  /** Enables the Clock Security System
   */
  HAL_RCC_EnableCSS();
}

/* USER CODE BEGIN 4 */
void increment(uint8_t *a)
{
  for (int i = 0; a[i] != NULL; i++)
  {
    a[i]++;
  }
}
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc1)
{
  // HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_12);
  UNUSED(hadc1);
}
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  if (GPIO_Pin == GPIO_PIN_11)
    // OLED_ShowString(0, 2, "EXTI Callback", 8);
    HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_12);
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
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef USE_FULL_ASSERT
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
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
