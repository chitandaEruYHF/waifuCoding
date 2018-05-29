#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "NUC1xx.h"
#include "Driver\DrvGPIO.h"
#include "Driver\DrvSYS.h"
#include "Driver\DrvSPI.h"
#include "Driver\DrvTIMER.h"



#define KEYTMR_OF  100



uint8_t key=0;

unsigned int mode=0;
unsigned int modechangeflag=0;



unsigned int mode2_freq=100;	 //KHz
unsigned int mode2_phasediff=8;	 //multiplied by 11.25degree


/*
unsigned char stringdegree_00[]="0°     ";
unsigned char stringdegree_01[]="11.25° ";
unsigned char stringdegree_02[]="22.5°  ";
unsigned char stringdegree_03[]="33.75° ";
unsigned char stringdegree_04[]="45°    ";
unsigned char stringdegree_05[]="56.25° ";
unsigned char stringdegree_06[]="67.5°  ";
unsigned char stringdegree_07[]="78.75° ";
unsigned char stringdegree_08[]="90°    ";
*/

unsigned char stringdegree_00[]="11.25°";
unsigned char stringdegree_01[]="11.25°";
unsigned char stringdegree_02[]="11.25°";
unsigned char stringdegree_03[]="11.25°";
unsigned char stringdegree_04[]="11.25°";
unsigned char stringdegree_05[]="11.25°";
unsigned char stringdegree_06[]="11.25°";
unsigned char stringdegree_07[]="11.25°";
unsigned char stringdegree_08[]="11.25°";



struct struct_act
{
	unsigned char num;
	unsigned char *str[20];
	unsigned char x[20],y[20],inverse[20];
} a0,a1,a2,a3,a4,a5;

struct struct_act *act[8];


unsigned char a0_s0[]="TEAM NUM.021", a0_s1[]="WELCOME", a0_s2[]="PRESS ANY KEY";

//adc_test


//人机界面设置
unsigned char a1_s0[]="CHOOSE OP MODE",a1_s1[]="ADC VALVE",a1_s2[]="SETUP&RUN AD9850";
unsigned char a2_s0[]="DUO ADC VALVE",a2_s1[]="BACK",a2_s4[]="Dc",a2_s5[]="Ds",a2_s6[]="V",a2_s7[]="V";

//添加ADC的结果显示
unsigned char a2_s2[20],a2_s3[20];

unsigned char a4_s0[]="SETUP&RUN AD9850",a4_s1[]="FREQ",a4_s2[]="KHz",a4_s3[]="delPHASE",a4_s4[]="RUN",a4_s5[]="BACK",a4_s6[]="1",a4_s7[]="0",a4_s8[]="0";
unsigned char *a4_s9;
unsigned char a5_s0[]="SETUP&RUN AD9850",a5_s1[]="FREQ",a5_s2[]="KHz",a5_s3[]="delPHASE",a5_s4[]="RUNNING",a5_s5[]="BACK",a5_s6[]="1",a5_s7[]="0",a5_s8[]="0",a5_s9[]="191.25°";



unsigned int ui_state=0;  //状态号

unsigned int key_ENTER_state=0;
unsigned int key_ENTER_prestate=0;
unsigned int ENTER_key_timer=0;
unsigned int key_ENTER_flag=0; 


unsigned int key_DOWN_state=0;
unsigned int key_DOWN_prestate=0;
unsigned int key_DOWN_timer=0;
unsigned int key_DOWN_flag=0; 


unsigned int key_UP_state=0;
unsigned int key_UP_prestate=0;
unsigned int key_UP_timer=0;
unsigned int key_UP_flag=0; 


unsigned int key_INCREASE_state=0;
unsigned int key_INCREASE_prestate=0;
unsigned int key_INCREASE_timer=0;
unsigned int key_INCREASE_flag=0; 
int degree_counter=8;

unsigned int key_DECREASE_state=0;
unsigned int key_DECREASE_prestate=0;
unsigned int key_DECREASE_timer=0;
unsigned int key_DECREASE_flag=0; 

unsigned int key_state=0;
unsigned int key_prestate=0;unsigned int state=0;
unsigned int key_timer=0;
unsigned int key_flag=0; 
unsigned char istr1[]="1234  ";



void itodegree(unsigned int phasediff, unsigned char **instrde)
{

		switch (phasediff)
		{
			case 0: *instrde = stringdegree_00;break;
			case 1: *instrde = stringdegree_01;break;
			case 2: *instrde = stringdegree_02;break;
			case 3: *instrde = stringdegree_03;break;
			case 4: *instrde = stringdegree_04;break;
			case 5: *instrde = stringdegree_05;break;
			case 6: *instrde = stringdegree_06;break;
			case 7: *instrde = stringdegree_07;break; 
			case 8: *instrde = stringdegree_08;break;

			
			default: break;	 
		}  
}




void itoafreq(unsigned int i, unsigned char* istr1,unsigned char* istr2,unsigned char* istr3)
{
	unsigned int j;
	j=i/100;
	istr1[0]='0'+j;istr1[1]='\0';
	i=i-j*100;
	j=i/10;
	istr2[0]='0'+j;istr2[1]='\0';
	i=i-j*10;
	istr3[0]='0'+i;istr3[1]='\0';
}


unsigned int atoifreq(unsigned char* istr1,unsigned char* istr2,unsigned char* istr3)
{	 
	unsigned int a,b,c,i;	  
	a=istr1[0]-'0';b=istr2[0]-'0';c=istr3[0]-'0';
	i = a*100+b*10+c;
	return i;
}



//按钮按下检测
void ENTER_detect(void)
{

	if (key==3)///////////////////	 ENTER
	{
		key_ENTER_prestate=key_ENTER_state;		
		key_ENTER_state=0;
		if (key_ENTER_prestate==1) 	key_ENTER_flag=1;
	}
	else
	{
		key_ENTER_prestate = key_ENTER_state; 
		key_ENTER_state=1;	
	}
}

void DOWN_detect(void)
{
	if (key == 2) ///////////////////	 DOWN
	{
		key_DOWN_prestate=key_DOWN_state;		
		key_DOWN_state=0;
		if (key_DOWN_prestate==1) key_DOWN_flag=1;
		
	}
	else
	{
		key_DOWN_prestate = key_DOWN_state; 
		key_DOWN_state=1;	
	}

}

void UP_detect(void)
{
	if (key == 1) ///////////////////	 UP
	{
		key_UP_prestate=key_UP_state;		
		key_UP_state=0;
		if (key_UP_prestate==1)  key_UP_flag=1;
	}
	else
	{
		key_UP_prestate = key_UP_state; 
		key_UP_state=1;	
	}
}

void INCREASE_detect(void)
{
	if (key == 4) ///////////////////	 INCREASE	
	{
		key_INCREASE_prestate=key_INCREASE_state;		
		key_INCREASE_state=0;
		if (key_INCREASE_prestate==1) 
		{	key_INCREASE_flag=1;	key_INCREASE_timer =0;	}
		else if (key_INCREASE_prestate==0)
		{
			if 	(++key_INCREASE_timer>=KEYTMR_OF)
			{ key_INCREASE_flag=1; key_INCREASE_timer=0;}  
		}
	}
	else
	{
		key_INCREASE_prestate = key_INCREASE_state; 
		key_INCREASE_state=1;
		key_INCREASE_timer=0;	
	}
}

void DECREASE_detect(void)
{
	if (key == 5) ///////////////////	 DECREASE	
	{
		key_DECREASE_prestate=key_DECREASE_state;		
		key_DECREASE_state=0;
		if (key_DECREASE_prestate==1) 
		{	key_DECREASE_flag=1;	key_DECREASE_timer =0;	}
		else if (key_DECREASE_prestate==0)
		{
			if 	(++key_DECREASE_timer>=KEYTMR_OF)
			{ key_DECREASE_flag=1; key_DECREASE_timer=0;}  
		}
	}
	else
	{
		key_DECREASE_prestate = key_DECREASE_state; 
		key_DECREASE_state=1;
		key_DECREASE_timer=0;	
	}
}


void display_ui_act(unsigned int i)
{		
	unsigned int j=0;
	clr_all_pannal();
	for (j=0;j<act[i]->num;j++) 
	{
		print_lcd(act[i]->x[j],act[i]->y[j],act[i]->str[j],act[i]->inverse[j]);	
	}
	
}



void init_act(void)
{

	itoafreq(mode2_freq, a4_s6,a4_s7,a4_s8);
	itodegree(mode2_phasediff,&a4_s9); 




	a0.num=3;
	a0.str[0]=a0_s0; a0.x[0]=0;  a0.y[0]=0;  a0.inverse[0]=0; 
	a0.str[1]=a0_s1; a0.x[1]=1;  a0.y[1]=0;  a0.inverse[1]=0;
	a0.str[2]=a0_s2; a0.x[2]=2;  a0.y[2]=0;  a0.inverse[2]=0;						  ///////act0
	act[0]=&a0;


	a1.num=3;
	a1.str[0]=a1_s0;	a1.x[0]=0;	a1.y[0]=0;	a1.inverse[0]=0;
	a1.str[1]=a1_s1;	a1.x[1]=1;	a1.y[1]=0;	a1.inverse[1]=0;
	a1.str[2]=a1_s2;	a1.x[2]=2;	a1.y[2]=0;	a1.inverse[0]=0;
//a1.str[3]=a1_s3;	a1.x[3]=3;	a1.y[3]=0;	a1.inverse[3]=0;					  ///////act1
	act[1]=&a1;


	a2.num=8;	
	//字符串型ADC结果print
	//sprintf(a2_s2,"%lf",Dc_test);
	//sprintf(a2_s3,"%lf",Ds_test);
	
	a2.str[0]=a2_s0;	a2.x[0]=0;	a2.y[0]=0;	a2.inverse[0]=0;	 
	a2.str[1]=a2_s4;	a2.x[1]=1;	a2.y[1]=0;	a2.inverse[1]=0;
	a2.str[2]=a2_s5;	a2.x[2]=2;	a2.y[2]=0;	a2.inverse[2]=0;
	a2.str[3]=a2_s2;	a2.x[3]=1;	a2.y[3]=4;	a2.inverse[3]=0;
	a2.str[4]=a2_s3;	a2.x[4]=2;	a2.y[4]=4;	a2.inverse[4]=0;	
	a2.str[5]=a2_s1;	a2.x[5]=3;	a2.y[5]=12;	a2.inverse[5]=0; 						/////act=2
	a2.str[6]=a2_s6;	a2.x[6]=1;	a2.y[6]=13;	a2.inverse[6]=0;
	a2.str[7]=a2_s7;	a2.x[7]=2;	a2.y[7]=13;	a2.inverse[7]=0;
	//sprintf(a2_s2,"%lf",Dc_test);
	
	//sprintf(a2_s3,"%lf",Ds_test);
	act[2]=&a2;

	

	a4.num=10;	
	a4.str[0]=a4_s0;	a4.x[0]=0;	a4.y[0]=0;	a4.inverse[0]=0;
	a4.str[1]=a4_s1;	a4.x[1]=1;	a4.y[1]=0;	a4.inverse[1]=0;	
	a4.str[2]=a4_s2;	a4.x[2]=1;	a4.y[2]=13;	a4.inverse[2]=0;
	a4.str[3]=a4_s3;	a4.x[3]=2;	a4.y[3]=0;	a4.inverse[3]=0;
	a4.str[4]=a4_s4;	a4.x[4]=3;	a4.y[4]=0;	a4.inverse[4]=0;
	a4.str[5]=a4_s5;	a4.x[5]=3;	a4.y[5]=12;	a4.inverse[5]=0;
	a4.str[6]=a4_s6;	a4.x[6]=1;	a4.y[6]=10;	a4.inverse[6]=0;
	a4.str[7]=a4_s7;	a4.x[7]=1;	a4.y[7]=11;	a4.inverse[7]=0;
	a4.str[8]=a4_s8;	a4.x[8]=1;	a4.y[8]=12;	a4.inverse[8]=0;
	a4.str[9]=a4_s9;	a4.x[9]=2;	a4.y[9]=10;	a4.inverse[9]=0;	/////act=4
	act[4]=&a4;



	a5.num=10;	
	a5.str[0]=a5_s0;	a5.x[0]=0;	a5.y[0]=0;	a5.inverse[0]=0;
	a5.str[1]=a5_s1;	a5.x[1]=1;	a5.y[1]=0;	a5.inverse[1]=0;	
	a5.str[2]=a5_s2;	a5.x[2]=1;	a5.y[2]=13;	a5.inverse[0]=0;
	a5.str[3]=a5_s3;	a5.x[3]=2;	a5.y[3]=0;	a5.inverse[3]=0;
	a5.str[4]=a5_s4;	a5.x[4]=3;	a5.y[4]=0;	a5.inverse[4]=0;
	a5.str[5]=a5_s5;	a5.x[5]=3;	a5.y[5]=12;	a5.inverse[5]=0;
	a5.str[6]=a5_s6;	a5.x[6]=1;	a5.y[6]=10;	a5.inverse[6]=0;
	a5.str[7]=a5_s7;	a5.x[7]=1;	a5.y[7]=11;	a5.inverse[7]=0;
	a5.str[8]=a5_s8;	a5.x[8]=1;	a5.y[8]=12;	a5.inverse[8]=0;
	a5.str[9]=a5_s9;	a5.x[9]=2;	a5.y[9]=10;	a5.inverse[9]=0;		/////act=5
	act[5]=&a5;

	

	display_ui_act(0);
}



void ui_proc0(void)
{
	if(key_ENTER_flag||key_UP_flag||key_DOWN_flag||key_INCREASE_flag||key_DECREASE_flag)
	{
		key_ENTER_flag=0;key_UP_flag=0;key_DOWN_flag=0;key_INCREASE_flag=0;key_DECREASE_flag=0;
		act[1]->inverse[1]=1; display_ui_act(1);
		ui_state=101;
	}
}

void ui_proc101(void)
{
	if(key_UP_flag)
	{
		act[1]->inverse[1]=0; print_lcd(act[1]->x[1],act[1]->y[1],act[1]->str[1],act[1]->inverse[1]);
		act[1]->inverse[2]=1; print_lcd(act[1]->x[2],act[1]->y[2],act[1]->str[2],act[1]->inverse[2]);
		ui_state=102;
	}
	else if (key_DOWN_flag)
	{
		act[1]->inverse[1]=0; print_lcd(act[1]->x[1],act[1]->y[1],act[1]->str[1],act[1]->inverse[1]);
		act[1]->inverse[2]=1; print_lcd(act[1]->x[2],act[1]->y[2],act[1]->str[2],act[1]->inverse[2]);
		ui_state=102;
	}
	else if (key_ENTER_flag)
	{
		act[1]->inverse[1]=0;
		act[2]->inverse[5]=1; display_ui_act(2);
		ui_state=201;
	}

	if(key_ENTER_flag||key_UP_flag||key_DOWN_flag||key_INCREASE_flag||key_DECREASE_flag)
	{
		key_ENTER_flag=0;key_UP_flag=0;key_DOWN_flag=0;key_INCREASE_flag=0;key_DECREASE_flag=0;
	}
}

void ui_proc102(void)
{
	if(key_UP_flag)
	{
		act[1]->inverse[2]=0; print_lcd(act[1]->x[2],act[1]->y[2],act[1]->str[2],act[1]->inverse[2]);
		act[1]->inverse[1]=1; print_lcd(act[1]->x[1],act[1]->y[1],act[1]->str[1],act[1]->inverse[1]);
		ui_state=101;
	}
	else if (key_DOWN_flag)
	{
		act[1]->inverse[2]=0; print_lcd(act[1]->x[2],act[1]->y[2],act[1]->str[2],act[1]->inverse[2]);
		act[1]->inverse[1]=1; print_lcd(act[1]->x[1],act[1]->y[1],act[1]->str[1],act[1]->inverse[1]);
		ui_state=101;
	}
	else if (key_ENTER_flag)
	{
		act[1]->inverse[2]=0;
		itodegree(mode2_phasediff,&a4_s9);a4.str[9]=a4_s9;
		degree_counter=mode2_phasediff;
		/*act[4]->inverse[6]=1;*/ 
		act[4]->inverse[4]=1;
		display_ui_act(4);
		/*ui_state=406;*/
		ui_state=404;
	}

	if(key_ENTER_flag||key_UP_flag||key_DOWN_flag||key_INCREASE_flag||key_DECREASE_flag)
	{
		key_ENTER_flag=0;key_UP_flag=0;key_DOWN_flag=0;key_INCREASE_flag=0;key_DECREASE_flag=0;
	}
}


void ui_proc201(void)
{
	if (key_ENTER_flag)
	{
		act[2]->inverse[5]=0;
		act[1]->inverse[1]=1; 
		display_ui_act(1);
		mode=0;
		ui_state=0;
	}
	else 
	{
		while(!key_ENTER_flag)
		{
			print_lcd(act[2]->x[3],act[2]->y[3],act[2]->str[3],act[2]->inverse[3]);
			print_lcd(act[2]->x[4],act[2]->y[4],act[2]->str[4],act[2]->inverse[4]);
			ui_state=101;
		}
	}

	if(key_ENTER_flag||key_UP_flag||key_DOWN_flag||key_INCREASE_flag||key_DECREASE_flag)
	{
		key_ENTER_flag=0;key_UP_flag=0;key_DOWN_flag=0;key_INCREASE_flag=0;key_DECREASE_flag=0;
	}
}


void in_de (unsigned int w, unsigned char* actstring )		//w=1 : increase; w=2 : decrease;
{
	if (w==1)
	{
		(*actstring)++;
		if (*actstring>'9') *actstring='0';
	}

	if (w==2)
	{
		(*actstring)--;
		if (*actstring<'0') *actstring='9';
	}

}





void ui_proc404(void)
{
	/*
	if(key_UP_flag)
	{
		act[4]->inverse[4]=0; print_lcd(act[4]->x[4],act[4]->y[4],act[4]->str[4],act[4]->inverse[4]);
		act[4]->inverse[9]=1; print_lcd(act[4]->x[9],act[4]->y[9],act[4]->str[9],act[4]->inverse[9]);
		ui_state=409;
	}
	*/
	
	if(key_UP_flag)
	{
		act[4]->inverse[4]=0; print_lcd(act[4]->x[4],act[4]->y[4],act[4]->str[4],act[4]->inverse[4]);
		act[4]->inverse[5]=1; print_lcd(act[4]->x[5],act[4]->y[5],act[4]->str[5],act[4]->inverse[5]);
		ui_state=405;
	}
	
	else if (key_DOWN_flag)
	{
		act[4]->inverse[4]=0; print_lcd(act[4]->x[4],act[4]->y[4],act[4]->str[4],act[4]->inverse[4]);
		act[4]->inverse[5]=1; print_lcd(act[4]->x[5],act[4]->y[5],act[4]->str[5],act[4]->inverse[5]);
		ui_state=405;
	}
	else if (key_ENTER_flag)
	{
		act[4]->inverse[4]=0;
		act[5]->inverse[5]=1;
		act[5]->str[6]=act[4]->str[6];act[5]->str[7]=act[4]->str[7];act[5]->str[8]=act[4]->str[8];
		mode2_freq = atoifreq(act[4]->str[6],act[4]->str[7],act[4]->str[8]);
		mode2_phasediff = degree_counter;	
		act[5]->str[9]=act[4]->str[9];
		mode=2;modechangeflag=2;
		display_ui_act(5);
		ui_state=505;
		
	}

	if(key_ENTER_flag||key_UP_flag||key_DOWN_flag||key_INCREASE_flag||key_DECREASE_flag)
	{
		key_ENTER_flag=0;key_UP_flag=0;key_DOWN_flag=0;key_INCREASE_flag=0;key_DECREASE_flag=0;
	}
}


void ui_proc405(void)
{
	if(key_UP_flag)
	{
		act[4]->inverse[5]=0; print_lcd(act[4]->x[5],act[4]->y[5],act[4]->str[5],act[4]->inverse[5]);
		act[4]->inverse[4]=1; print_lcd(act[4]->x[4],act[4]->y[4],act[4]->str[4],act[4]->inverse[4]);
		ui_state=404;
	}
	
	/*
	else if (key_DOWN_flag)
	{
		act[4]->inverse[5]=0; print_lcd(act[4]->x[5],act[4]->y[5],act[4]->str[5],act[4]->inverse[5]);
		act[4]->inverse[6]=1; print_lcd(act[4]->x[6],act[4]->y[6],act[4]->str[6],act[4]->inverse[6]);
		ui_state=406;
	}
	*/
	
	else if (key_DOWN_flag)
	{
		act[4]->inverse[5]=0; print_lcd(act[4]->x[5],act[4]->y[5],act[4]->str[5],act[4]->inverse[5]);
		act[4]->inverse[4]=1; print_lcd(act[4]->x[4],act[4]->y[4],act[4]->str[4],act[4]->inverse[4]);
		ui_state=404;
	}
	
	else if (key_ENTER_flag)
	{
		act[4]->inverse[5]=0;
		act[1]->inverse[2]=1; display_ui_act(1);
		itoafreq(mode2_freq,a4_s6,a4_s7,a4_s8);
		itodegree(mode2_phasediff,&a4_s9);
		mode=0;
		ui_state=102;
	}

	if(key_ENTER_flag||key_UP_flag||key_DOWN_flag||key_INCREASE_flag||key_DECREASE_flag)
	{
		key_ENTER_flag=0;key_UP_flag=0;key_DOWN_flag=0;key_INCREASE_flag=0;key_DECREASE_flag=0;
	}
}


void ui_proc406(void)
{
	if(key_UP_flag)
	{
		act[4]->inverse[6]=0; print_lcd(act[4]->x[6],act[4]->y[6],act[4]->str[6],act[4]->inverse[6]);
		act[4]->inverse[5]=1; print_lcd(act[4]->x[5],act[4]->y[5],act[4]->str[5],act[4]->inverse[5]);
		ui_state=405;
	}
	else if (key_DOWN_flag)
	{
		act[4]->inverse[6]=0; print_lcd(act[4]->x[6],act[4]->y[6],act[4]->str[6],act[4]->inverse[6]);
		act[4]->inverse[7]=1; print_lcd(act[4]->x[7],act[4]->y[7],act[4]->str[7],act[4]->inverse[7]);
		ui_state=407;
	}
	else if (key_INCREASE_flag)
	{
	    in_de(1,act[4]->str[6]);
		print_lcd(act[4]->x[6],act[4]->y[6],act[4]->str[6],act[4]->inverse[6]);
		ui_state=406;
	}
	else if (key_DECREASE_flag)
	{
		in_de(2,act[4]->str[6]);		
		print_lcd(act[4]->x[6],act[4]->y[6],act[4]->str[6],act[4]->inverse[6]);
		ui_state=406;
	}
	
	if(key_ENTER_flag||key_UP_flag||key_DOWN_flag||key_INCREASE_flag||key_DECREASE_flag)
	{
		key_ENTER_flag=0;key_UP_flag=0;key_DOWN_flag=0;key_INCREASE_flag=0;key_DECREASE_flag=0;
	}
}

void ui_proc407(void)
{
	if(key_UP_flag)
	{
		act[4]->inverse[7]=0; print_lcd(act[4]->x[7],act[4]->y[7],act[4]->str[7],act[4]->inverse[7]);
		act[4]->inverse[6]=1; print_lcd(act[4]->x[6],act[4]->y[6],act[4]->str[6],act[4]->inverse[6]);
		ui_state=406;
	}
	else if (key_DOWN_flag)
	{
		act[4]->inverse[7]=0; print_lcd(act[4]->x[7],act[4]->y[7],act[4]->str[7],act[4]->inverse[7]);
		act[4]->inverse[8]=1; print_lcd(act[4]->x[8],act[4]->y[8],act[4]->str[8],act[4]->inverse[8]);
		ui_state=408;
	}
	else if (key_INCREASE_flag)
	{
	    in_de(1,act[4]->str[7]);
		print_lcd(act[4]->x[7],act[4]->y[7],act[4]->str[7],act[4]->inverse[7]);
		ui_state=407;
	}
	else if (key_DECREASE_flag)
	{
		in_de(2,act[4]->str[7]);		
		print_lcd(act[4]->x[7],act[4]->y[7],act[4]->str[7],act[4]->inverse[7]);
		ui_state=407;
	}

	if(key_ENTER_flag||key_UP_flag||key_DOWN_flag||key_INCREASE_flag||key_DECREASE_flag)
	{
		key_ENTER_flag=0;key_UP_flag=0;key_DOWN_flag=0;key_INCREASE_flag=0;key_DECREASE_flag=0;
	}
}


void ui_proc408(void)
{
	if(key_UP_flag)
	{
		act[4]->inverse[8]=0; print_lcd(act[4]->x[8],act[4]->y[8],act[4]->str[8],act[4]->inverse[8]);
		act[4]->inverse[7]=1; print_lcd(act[4]->x[7],act[4]->y[7],act[4]->str[7],act[4]->inverse[7]);
		ui_state=407;
	}
	else if (key_DOWN_flag)
	{
		act[4]->inverse[8]=0; print_lcd(act[4]->x[8],act[4]->y[8],act[4]->str[8],act[4]->inverse[8]);
		act[4]->inverse[9]=1; print_lcd(act[4]->x[9],act[4]->y[9],act[4]->str[9],act[4]->inverse[9]);
		ui_state=409;
	}
	else if (key_INCREASE_flag)
	{
	    in_de(1,act[4]->str[8]);
		print_lcd(act[4]->x[8],act[4]->y[8],act[4]->str[8],act[4]->inverse[8]);
		ui_state=408;
	}
	else if (key_DECREASE_flag)
	{
		in_de(2,act[4]->str[8]);		
		print_lcd(act[4]->x[8],act[4]->y[8],act[4]->str[8],act[4]->inverse[8]);
		ui_state=408;
	}

	if(key_ENTER_flag||key_UP_flag||key_DOWN_flag||key_INCREASE_flag||key_DECREASE_flag)
	{
		key_ENTER_flag=0;key_UP_flag=0;key_DOWN_flag=0;key_INCREASE_flag=0;key_DECREASE_flag=0;
	}
}

void in_de_degree(int degree_counter)
{
		switch (degree_counter)
		{
			case 0: act[4]->str[9]= stringdegree_00;break;
			case 1: act[4]->str[9]= stringdegree_01;break;
			case 2: act[4]->str[9]= stringdegree_02;break;
			case 3: act[4]->str[9]= stringdegree_03;break;
			case 4: act[4]->str[9]= stringdegree_04;break;
			case 5: act[4]->str[9]= stringdegree_05;break;
			case 6: act[4]->str[9]= stringdegree_06;break;
			case 7: act[4]->str[9]= stringdegree_07;break;
			case 8: act[4]->str[9]= stringdegree_08;break;
			
			default: break;	 
		} 	 
}


void ui_proc409(void)
{
	if(key_UP_flag)
	{
		act[4]->inverse[9]=0; print_lcd(act[4]->x[9],act[4]->y[9],act[4]->str[9],act[4]->inverse[9]);
		act[4]->inverse[8]=1; print_lcd(act[4]->x[8],act[4]->y[8],act[4]->str[8],act[4]->inverse[8]);
		ui_state=408;
	}
	else if (key_DOWN_flag)
	{
		act[4]->inverse[9]=0; print_lcd(act[4]->x[9],act[4]->y[9],act[4]->str[9],act[4]->inverse[9]);
		act[4]->inverse[4]=1; print_lcd(act[4]->x[4],act[4]->y[4],act[4]->str[4],act[4]->inverse[4]);
		ui_state=404;
	}
	else if (key_INCREASE_flag)
	{
		degree_counter++;
		if (degree_counter>8) degree_counter=0;
		in_de_degree(degree_counter);
		print_lcd(act[4]->x[9],act[4]->y[9],act[4]->str[9],act[4]->inverse[9]);
		ui_state=409;
	}
	else if (key_DECREASE_flag)
	{
		degree_counter--;
		if (degree_counter<0) degree_counter=8;
		in_de_degree(degree_counter);
		print_lcd(act[4]->x[9],act[4]->y[9],act[4]->str[9],act[4]->inverse[9]);
		ui_state=409;
	}

	if(key_ENTER_flag||key_UP_flag||key_DOWN_flag||key_INCREASE_flag||key_DECREASE_flag)
	{
		key_ENTER_flag=0;key_UP_flag=0;key_DOWN_flag=0;key_INCREASE_flag=0;key_DECREASE_flag=0;
	}
}


void ui_proc505(void)
{
	if (key_ENTER_flag)
	{
		act[5]->inverse[5]=0;
		act[1]->inverse[2]=1; display_ui_act(1);
		mode=0;
		ui_state=102;
	}

	if(key_ENTER_flag||key_UP_flag||key_DOWN_flag||key_INCREASE_flag||key_DECREASE_flag)
	{
		key_ENTER_flag=0;key_UP_flag=0;key_DOWN_flag=0;key_INCREASE_flag=0;key_DECREASE_flag=0;
	}
}




void ui_state_proc(unsigned int ui_state)
{
	switch (ui_state)
		{
			case 0: ui_proc0(); break;
			case 101: ui_proc101();break;
			case 102: ui_proc102();break;
			case 201: ui_proc201();break;
			case 404: ui_proc404();break;
			case 405: ui_proc405();break;
			case 406: ui_proc406();break;
			case 407: ui_proc407();break;
			case 408: ui_proc408();break;
			case 409: ui_proc409();break;
			case 505: ui_proc505();break;

			default: break;
		}

}
