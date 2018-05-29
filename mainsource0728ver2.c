/////////////////////////////////////////////////
//�ж�Timer0Ƶ��200HZ,��Ƶ��1MHZ��SPIʱ��Ƶ��XXMHZ��ADC����Ƶ��XXMHZ
////////////////////////////////////////////////



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "NUC1xx.h"
#include "Driver\DrvGPIO.h"
#include "Driver\DrvSYS.h"
#include "Driver\DrvSPI.h"
#include "Driver\DrvADC.h"
#include "Driver\DrvTIMER.h"


#include "LCD_Driver20171019.c"
#include "ScanKey6+20171024.c"
#include "AD9850FUNC0903.c"
#include "UIFUNC0728.c"


//////////////////////////////
// ��������  αָ���     //
//////////////////////////////

//ad9850��freqency
#define ad9850_freq 100

// 1s�����ʱ�����ֵ��200��5ms
#define V_T1s	200
//#define V_T1s	100

//αָ��  
#define C12LED_H    DrvGPIO_SetBit(E_GPC,12)	//led off ����LED��
#define C12LED_L    DrvGPIO_ClrBit(E_GPC,12)   //led on
#define PD11_H    DrvGPIO_SetBit(E_GPE,12)	   //�жϲ��Թܽ�
#define PD11_L    DrvGPIO_ClrBit(E_GPE,12)	   //�жϲ��Թܽ�


//////////////////////////////
//			��������        //
//////////////////////////////

//��λ�� unit:11.25��
int angle=0;

//Timer0_Callback�жϳ���ļ�����
unsigned int cnt=0;

//adc_test
double Dc_test;
double Ds_test;
//extern unsigned char buffer_dc[20];
//extern unsigned char buffer_ds[20];



// 1s�����ʱ������
unsigned int clock1s=0;
// 1s�����ʱ�������־
unsigned int clock1s_flag=0;
unsigned int times10s=0;
unsigned int led=0;

unsigned char key_display[]="0";
								

void itoa(int i, unsigned char* istr)
{
	unsigned int j;
	j=i/1000;
	istr[0]='0'+j;
	i=i-j*1000;
	j=i/100;
	istr[1]='0'+j;
	i=i-j*100;
	j=i/10;
	istr[2]='0'+j;
	i=i-j*10;
	istr[3]='0'+i;
	istr[4]='\0';
}


//  I/O�˿ڳ�ʼ��
void Port_Init(void)
{
		ad9850_Port_Init();
		Initial_pannel();
		OpenKeyPad();



		DrvGPIO_Open(E_GPC, 12, E_IO_OUTPUT);			  //LED
		DrvGPIO_Open(E_GPE, 12, E_IO_OUTPUT);			  //�жϲ���


}

 
//Timer0_A0 interrupt service routine
//5ms�Զ�����һ��
 void Timer0_Callback (int freq, int phase)
{	
		//�����о�����
	double Dc,Ds;
	int del_angle;
	int abs_angle;
	double K=0.5;
	double B1=0.5;
	double B2=0.5;
	double Lc=4;
	double Ls=4;
	

	
	//����adc���ݵ���ȡ�����
	double ADCTemp1=0;
	double ADCTemp2=0;
	double Vcth=Lc*K*B1;
	double Vsth=Ls*K*B2;
	
	
	//���ȼ�����+1
	if (cnt==4294967295)
	{
		cnt=0;
		cnt=cnt+1;
	}
	else
	{	
		cnt=cnt+1;
	}
	
	
	PD11_H;	 //�ж�ʱ�����
	key=Scankey();
	ENTER_detect();
	DOWN_detect();
	UP_detect();
	INCREASE_detect();
	DECREASE_detect();
	
				
////////////////////////////mode=1 ADC����ת��//////////////////////
	DrvADC_StartConvert();
	
	//�ȴ�ADCת�����
	 while(!DrvADC_IsConversionDone())                 
  {
  }

	ADCTemp1=DrvADC_GetConversionData(5);
	ADCTemp2=DrvADC_GetConversionData(6);

	DrvADC_StopConvert();
	
	//ת��Ϊʮ���Ƶĵ�ѹֵ
	Dc=ADCTemp1*3.3/4096;
	Ds=ADCTemp2*3.3/4096;
	
	//��ʾadc���
	Dc_test=Dc;
	Ds_test=Ds;	
	//sprintf_s(buffer_dc,"%f",Dc);
	//sprintf_s(buffer_ds,"%f",Ds);
	//sprintf(a2_s2,"%lf",Dc_test);
	//sprintf(a2_s3,"%lf",Ds_test);
	
	
/////////////////mode=2 ���໷�ȶ��ж�/////////////////////////////
	if ((Dc>Vcth)&&(Ds>Vsth))
	{
		del_angle=-1;
	}
	
	if ((Dc>Vcth)&&(Ds<=Vsth))
	{
		del_angle=1;
	}
	
	if ((Dc<=Vcth)&&(Ds>Vsth))
	{
		del_angle=1;
	}
	
  if ((Dc<=Vcth)&&(Ds<=Vsth))
	{
		del_angle=-1;
	}
	
//////////////////mode=2 ���໷����////////////////////////////////
	//������λ�ʹ֮�ƽ�����

	angle=angle+del_angle;
	
	if (angle>=0)
	{
		abs_angle=angle;
	}
	else
	{
		abs_angle=32+angle;
	}
	
	//����ad9850������·�µı����ź�
	setup_AD9850(ad9850_freq,ad9850_freq,abs_angle+8,abs_angle);
	
	

	
////// 1������ʱ������ /////////////////////////////////
	if (++clock1s>=V_T1s)
	{
		clock1s_flag = 1; //��1�뵽ʱ�������־��1
		clock1s = 0;
	}

		PD11_L;	//�ж�ʱ�����
}

//TIMER0 initialize -
// desired value: 5ms
void Timer0_Init(void)
{
	DrvTIMER_Init();//��ʼ��timer
	DrvTIMER_Open(E_TMR0,200,E_PERIODIC_MODE);//���ö�ʱ��timer0,��ʱ��tickÿ��200�� ,5ms
	DrvTIMER_SetTimerEvent(E_TMR0,1,(TIMER_CALLBACK) Timer0_Callback,0); //��װ��ʱ�����¼���timer0
	
}

void Init_Devices(void)
{	  
	SYSCLK->APBCLK.WDT_EN =0;//Disable WDT clock source

	DrvSYS_SetOscCtrl(E_SYS_XTL12M, 1);	 // Enable the external 12MHz oscillator oscillation
	DrvSYS_SelectHCLKSource(0);	 // HCLK clock source. 0: external 12MHz; 4:internal 22MHz RC oscillator
	DrvSYS_SetClockDivider(E_SYS_HCLK_DIV, 11); // HCLK clock frequency = 12M/(11+1)=1M

	DrvADC_Open(ADC_SINGLE_END, ADC_SINGLE_CYCLE_OP, 102, INTERNAL_HCLK, 0);  //ʹ��ADCchannel 5&6��ADCClock = 1M/(0+1)  =1M

	Port_Init();             //��ʼ��I/O��
  Timer0_Init();           //��ʼ����ʱ��0
	
	DrvTIMER_Start(E_TMR0);
	
}

//��ʼ��ADC
/*void Init_adc()
{
    E_ADC_INPUT_MODE InputMode = ADC_SINGLE_END;
    
    DrvADC_SetADCOperationMode(ADC_SINGLE_CYCLE_OP);
    DrvADC_SetADCInputMode(InputMode);
    DrvGPIO_DisableDigitalInputBit(E_GPA, 5);//u8ChannelCount is 0-15
    DrvGPIO_DisableDigitalInputBit(E_GPA, 6);
    DrvGPIO_InitFunction(E_FUNC_ADC5);
    DrvGPIO_InitFunction(E_FUNC_ADC6);//{ E_FUNC_ADC0, E_FUNC_ADC1, E_FUNC_ADC2, E_FUNC_ADC3,E_FUNC_ADC4, E_FUNC_ADC5, E_FUNC_ADC6, E_FUNC_ADC7 };
    DrvADC_SetADCChannel(0x60);
}
*/
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int main(void)
{

	Init_Devices( );
	//Init_adc();
	DrvSYS_Delay(2000);
	times10s =2000;
	Initial_pannel();
	init_act();
	ad9850_reset();
	DrvTIMER_EnableInt(E_TMR0);
	
	setup_AD9850(ad9850_freq,ad9850_freq,0,8);

// ��ѭ���������У���Timer0_A0�жϷ������δ��ִ�еĿ���ʱ�������������³����в���ѭ��
	while(1)
	 {
	 	ui_state_proc(ui_state);
	

		 
		if (clock1s_flag==1)   // ���1�붨ʱ�Ƿ�
		 {
			clock1s_flag=0;


			if(led==0)
			{
				C12LED_L;
				led=1;

			}
			else 
			{
				C12LED_H;
				led=0;
			}
		 }
	  }		 
}


