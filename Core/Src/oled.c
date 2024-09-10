#include "oled.h"
#include "i2c.h"
#include "oledfont.h"

//0x78 OLED显示屏地址
//0x00 写入命令
//0x40 写入写入数据
void Oled_Write_Cmd(uint8_t dataCmd)
{
	
	HAL_I2C_Mem_Write(&hi2c1, 0x78, 0x00, I2C_MEMADD_SIZE_8BIT,
										&dataCmd, 1, 0xff);
}

void Oled_Write_Data(uint8_t dataData)
{
	HAL_I2C_Mem_Write(&hi2c1, 0x78, 0x40, I2C_MEMADD_SIZE_8BIT,
										&dataData, 1, 0xff);
}

void Oled_Init(void){
	Oled_Write_Cmd(0xAE);//--display off
	Oled_Write_Cmd(0x00);//---set low column address
	Oled_Write_Cmd(0x10);//---set high column address
	Oled_Write_Cmd(0x40);//--set start line address  
	Oled_Write_Cmd(0xB0);//--set page address
	Oled_Write_Cmd(0x81); // contract control
	Oled_Write_Cmd(0xFF);//--128   
	Oled_Write_Cmd(0xA1);//set segment remap 
	Oled_Write_Cmd(0xA6);//--normal / reverse
	Oled_Write_Cmd(0xA8);//--set multiplex ratio(1 to 64)
	Oled_Write_Cmd(0x3F);//--1/32 duty
	Oled_Write_Cmd(0xC8);//Com scan direction
	Oled_Write_Cmd(0xD3);//-set display offset
	Oled_Write_Cmd(0x00);//
	
	Oled_Write_Cmd(0xD5);//set osc division
	Oled_Write_Cmd(0x80);//
	
	Oled_Write_Cmd(0xD8);//set area color mode off
	Oled_Write_Cmd(0x05);//
	
	Oled_Write_Cmd(0xD9);//Set Pre-Charge Period
	Oled_Write_Cmd(0xF1);//
	
	Oled_Write_Cmd(0xDA);//set com pin configuartion
	Oled_Write_Cmd(0x12);//
	
	Oled_Write_Cmd(0xDB);//set Vcomh
	Oled_Write_Cmd(0x30);//
	
	Oled_Write_Cmd(0x8D);//set charge pump enable
	Oled_Write_Cmd(0x14);//
	
	Oled_Write_Cmd(0xAF);//--turn on oled panel		
}

void Oled_Screen_Clear(void){
	char i,n;
	Oled_Write_Cmd (0x20);                    //set memory addressing mode开启选址模式设定
	Oled_Write_Cmd (0x02);                    //page addressing mode页面地址寻址模式

	for(i=0;i<8;i++){
		Oled_Write_Cmd(0xb0+i);               //第0页开始，共8页
		Oled_Write_Cmd(0x00);                 //设置列低地址到开头
		Oled_Write_Cmd(0x10);                 //设置列高地址到开头
		for(n=0;n<128;n++)Oled_Write_Data(0x00); 		//清空所有列	
	}	
}

//字体为8*16,需要2个page
void Oled_Show_Char(char row,char col,char oledChar){ //row*2-2 //row行 col列,从1开始
	unsigned int  i;
	Oled_Write_Cmd(0xb0+(row*2-2));                           //page 0
	Oled_Write_Cmd(0x00+(col&0x0f));                          //low 加上col低位
	Oled_Write_Cmd(0x10+(col>>4));                            //high 加上col高位
	for(i=((oledChar-32)*16);i<((oledChar-32)*16+8);i++){ //减掉""的ASCII码找到在字体库中的索引
		//如果是1的话，因为是8*16，所以在一个8位一个单位的字体数组中，要偏移16个字节，先写8列，就是8个字节
		Oled_Write_Data(F8X16[i]);                            //
	}

	Oled_Write_Cmd(0xb0+(row*2-1));                           //page 1
	Oled_Write_Cmd(0x00+(col&0x0f));                          //low
	Oled_Write_Cmd(0x10+(col>>4));                            //high
	for(i=((oledChar-32)*16+8);i<((oledChar-32)*16+8+8);i++){ //偏移8个字节，再写8位
		Oled_Write_Data(F8X16[i]);                            //
	}		
}


void Oled_Show_Str(char row,char col,char *str){
	while(*str!=0){
		Oled_Show_Char(row,col,*str);
		str++;
		col += 8; //字体是8*16，列偏移8个单位	
	}		
}
