#include "sr04.h"
#include "gpio.h"
#include "tim.h"

void TIM2_Delay_us(uint16_t n_us)
{
    
    __HAL_TIM_ENABLE(&htim2); //PSC 72-1 72MHZ时钟，1秒执行1000000次，计数一次1us
    __HAL_TIM_SetCounter(&htim2, 0);
    while(__HAL_TIM_GetCounter(&htim2) < ((1 * n_us)-1) ); //从0开始计数，-1
    
    __HAL_TIM_DISABLE(&htim2);
}

double get_distance(void)
{
		int cnt=0;
		//触发
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, GPIO_PIN_SET);
		TIM2_Delay_us(20);
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, GPIO_PIN_RESET);//发至少20us

		//2. echo
		while(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_11) == GPIO_PIN_RESET); //低电平时while等待
		HAL_TIM_Base_Start(&htim2); //退出while，表示echo引脚高电平，开始计时
		__HAL_TIM_SetCounter(&htim2,0); //计数器设定为0

		while(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_11) == GPIO_PIN_SET);//高电平时while等待
		HAL_TIM_Base_Stop(&htim2); //退出while，表示echo引脚转为低电平

		
		cnt = __HAL_TIM_GetCounter(&htim2); //获取计数值

		return (cnt*340/2*0.000001*100); //计算距离
}
