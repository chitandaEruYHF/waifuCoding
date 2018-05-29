/*---------------------------------------------------------------------------------------------------------*/
/*                                                                                                         */
/* Copyright (c) Nuvoton Technology Corp. All rights reserved.                                             */
/*                                                                                                         */
/*---------------------------------------------------------------------------------------------------------*/
////�����Դ��ļ��޸ġ�
////LB��ԭ�а�����������ʹ��GPA0-5�ܽſ���3*3�Ÿ������������ADC�ܽ�GPA0-7���ã�����GPA7���ӵ�λ�������ڱ�������
////Ҫʹ�������������·ADC�������ԭ�а�����ADC���ų�ͻ������ADC6һ·����ADC���ã��������޸İ���������������GPA5��
////�޸ĺ�ScanKey6��������ʹ��GPA0-4�ܽſ���3*2����������
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
