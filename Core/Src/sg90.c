#include "sg90.h"
#include "gpio.h"
#include "tim.h"

void initSG90(void)
{
	HAL_TIM_PWM_Start(&htim4,TIM_CHANNEL_4); //定时器四，通道四PWM模式，PSC 7200-1 ARR 200-1 设定为20ms
	__HAL_TIM_SetCompare(&htim4, TIM_CHANNEL_4, 15); //初始化设定为90度
}

void sgMiddle(void)
{
	__HAL_TIM_SetCompare(&htim4, TIM_CHANNEL_4, 15); //1.5ms 90度
}

void sgRight(void)
{
	__HAL_TIM_SetCompare(&htim4, TIM_CHANNEL_4, 5);  //0.5ms 0度
}

void sgLeft(void)
{
	__HAL_TIM_SetCompare(&htim4, TIM_CHANNEL_4, 25);  //2.5ms 180度
}
