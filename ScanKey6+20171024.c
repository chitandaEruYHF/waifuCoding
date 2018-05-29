/*---------------------------------------------------------------------------------------------------------*/
/*                                                                                                         */
/* Copyright (c) Nuvoton Technology Corp. All rights reserved.                                             */
/*                                                                                                         */
/*---------------------------------------------------------------------------------------------------------*/
////根据自带文件修改。
////LB板原有按键驱动程序使用GPA0-5管脚控制3*3九个按键，与板上ADC管脚GPA0-7复用（其中GPA7连接电位器）。在本例中需
////要使用五个按键和两路ADC，与板上原有按键、ADC安排冲突（仅有ADC6一路独立ADC可用），所以修改按键驱动程序剥离出GPA5。
////修改后ScanKey6驱动程序使用GPA0-4管脚控制3*2六个按键。
#include <stdio.h>
#include "Driver\DrvGPIO.h"

void delay(void)
{
	int j;
	for(j=0;j<10;j++);
}

void OpenKeyPad(void)
{
	uint8_t i;
	/* Initial key pad */
	for(i=0;i<5;i++)
	DrvGPIO_Open(E_GPA, i, E_IO_QUASI);
}

void CloseKeyPad(void)
{
	uint8_t i;

	for(i=0;i<5;i++)
	DrvGPIO_Close(E_GPA, i);
}

uint8_t Scankey(void)
{
	uint8_t act[4]={0x3b, 0x3d, 0x3e};    
	uint8_t i,temp,pin;

	for(i=0;i<3;i++)
	{
		temp=act[i];
		for(pin=0;pin<5;pin++)
		{
			if((temp&0x01)==0x01)
				DrvGPIO_SetBit(E_GPA,pin);
			else
				DrvGPIO_ClrBit(E_GPA,pin);
			temp>>=1;
		}
		delay();
		if(DrvGPIO_GetBit(E_GPA,3)==0)
			return(i+1);
		if(DrvGPIO_GetBit(E_GPA,4)==0)
			return(i+4);
	}
		return 0;
}
