/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
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
#include "i2c.h"
#include "tim.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "sg90.h"
#include "sr04.h"
#include "motor.h"
#include "oled.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
#define MIDDLE 0
#define LEFT 1
#define RIGHT 2

#define BZ 1
#define XJ 2
#define GS 3

#define LeftWheel_Value_XJ HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_3)
#define RightWheel_Value_XJ HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_4)

#define LeftWheel_Value_GS HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_8)
#define RightWheel_Value_GS HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_9)

#define A25 HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_15)
#define A26 HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_13)
#define A27 HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_14)
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
char dir;

void xunjiMode() //读取循迹模块引脚电平状�??
{
	if(LeftWheel_Value_XJ == GPIO_PIN_RESET && RightWheel_Value_XJ == GPIO_PIN_RESET)
		goForward();
	if(LeftWheel_Value_XJ == GPIO_PIN_SET && RightWheel_Value_XJ == GPIO_PIN_RESET)
		goLeft();
	if(LeftWheel_Value_XJ == GPIO_PIN_RESET && RightWheel_Value_XJ == GPIO_PIN_SET)
		goRight();
	if(LeftWheel_Value_XJ == GPIO_PIN_SET && RightWheel_Value_XJ == GPIO_PIN_SET)
		stop();
}

void gensuiMode() //读取跟随模式引脚电平状�??
{
	if(LeftWheel_Value_GS == GPIO_PIN_RESET && RightWheel_Value_GS == GPIO_PIN_RESET)
		goForward();
	if(LeftWheel_Value_GS == GPIO_PIN_SET && RightWheel_Value_GS == GPIO_PIN_RESET)
		goRight();
	if(LeftWheel_Value_GS == GPIO_PIN_RESET && RightWheel_Value_GS == GPIO_PIN_SET)
		goLeft();
	if(LeftWheel_Value_GS == GPIO_PIN_SET && RightWheel_Value_GS == GPIO_PIN_SET)
		stop();
}

void bizhangMode()
{
	double disMiddle;
	double disLeft;
	double disRight;

	if(dir != MIDDLE){ //让sg90舵机位置处于正中
		sgMiddle();
		dir = MIDDLE;
		HAL_Delay(300);
	}
	disMiddle = get_distance();
	
	if(disMiddle > 35){
		goForward();
	}else if(disMiddle < 10){
		goBack();
		
	}else
	{
		stop();
		sgLeft();
		HAL_Delay(300);
		disLeft = get_distance();
		sgMiddle();
		HAL_Delay(300);
		sgRight();
		dir = RIGHT;
		HAL_Delay(300);
		disRight = get_distance();
		
		if(disLeft < disRight){
			goRight();
			HAL_Delay(150);
			stop();
		}
		if(disRight < disLeft){
			goLeft();
			HAL_Delay(150);
			stop();
		}
	}
	HAL_Delay(50);
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
	int mark = 0;
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
  MX_TIM4_Init();
  MX_TIM2_Init();
  MX_I2C1_Init();
  /* USER CODE BEGIN 2 */
	initSG90();
	HAL_Delay(1000);
	dir = MIDDLE;
	Oled_Init();
	Oled_Screen_Clear();
	Oled_Show_Str(2,2,"-----Ready----");
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
		//与语音模块进行联动，读取语音模块的引脚状�?
		if(A25 == 1 && A26 == 1 && A27 == 0){
			if(mark != XJ){ //循迹模式
				Oled_Screen_Clear();
				Oled_Show_Str(2,2,"-----XunJi----");
			}
			mark = XJ;
			xunjiMode();
		}
		
		if(A25 == 0 && A26 == 1 && A27 == 1){
			if(mark != GS){ //跟随模式
				Oled_Screen_Clear();
				Oled_Show_Str(2,2,"-----GenSui----");
			}
			mark = GS;
			gensuiMode();
		}
		
		if(A25 == 1 && A26 == 0 && A27 == 1){
			if(mark != BZ){ //避障模式
				Oled_Screen_Clear();
				Oled_Show_Str(2,2,"-----BiZhang----");
			}
			mark = BZ;
			bizhangMode();
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
}

/* USER CODE BEGIN 4 */

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
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
