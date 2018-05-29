#include "NUC1xx.h"
#include "Driver\DrvSYS.h"
#include "Driver\DrvSPI.h"
#include "Driver\DrvGPIO.h"

extern  SPI_T * SPI_PORT[4]={SPI0, SPI3, SPI2, SPI3};

extern  char Ascii[];

//αָ��
#define LCD_ROM_IN_H    DrvGPIO_SetBit(E_GPA,0)
#define LCD_ROM_IN_L    DrvGPIO_ClrBit(E_GPA,0)
#define LCD_ROM_OUT_H    DrvGPIO_SetBit(E_GPA,1)
#define LCD_ROM_OUT_L    DrvGPIO_ClrBit(E_GPA,1)
#define LCD_ROM_SCK_H    DrvGPIO_SetBit(E_GPA,2)
#define LCD_ROM_SCK_L    DrvGPIO_ClrBit(E_GPA,2)
#define LCD_ROM_CS_H    DrvGPIO_SetBit(E_GPA,3)
#define LCD_ROM_CS_L    DrvGPIO_ClrBit(E_GPA,3)


void SysTimerDelay(uint32_t us)
{
    SysTick->LOAD = us * 22; /* Assume the internal 22MHz RC used */
    SysTick->VAL   =  (0x00);
    SysTick->CTRL = (1 << SysTick_CTRL_CLKSOURCE_Pos) | (1<<SysTick_CTRL_ENABLE_Pos);

    /* Waiting for down-count to zero */
    while((SysTick->CTRL & (1 << 16)) == 0);
}

//*******************************
//this function to wrtie command to panel
//*******************************
void transfer_command_lcd(unsigned char data)
{
	DrvGPIO_ClrBit(E_GPA,4);											// rs=0;ָ��Ĵ���
   
   	SPI_PORT[eDRVSPI_PORT2]->SSR.SSR=1;									//ENABLE SLAVE SELECT
	SPI_PORT[eDRVSPI_PORT2]->TX[0] = data;
	SPI_PORT[eDRVSPI_PORT2]->CNTRL.GO_BUSY = 1;
    while ( SPI_PORT[eDRVSPI_PORT2]->CNTRL.GO_BUSY == 1 );
	SPI_PORT[eDRVSPI_PORT2]->SSR.SSR=0;	
}

//*******************************
//this function to wrtie data to panel
//*******************************
void transfer_data_lcd(unsigned char data)
{
	DrvGPIO_SetBit(E_GPA,4);										    // rs=1; ���ݼĴ���
   
   	SPI_PORT[eDRVSPI_PORT2]->SSR.SSR=1;									//ENABLE SLAVE SELECT
	SPI_PORT[eDRVSPI_PORT2]->TX[0] = 0x100 | data;
	SPI_PORT[eDRVSPI_PORT2]->CNTRL.GO_BUSY = 1;
    while ( SPI_PORT[eDRVSPI_PORT2]->CNTRL.GO_BUSY == 1 );
	SPI_PORT[eDRVSPI_PORT2]->SSR.SSR=0;	
}

void LCD_Port_Init(void)
{
     	DrvGPIO_Open(E_GPA, 0, E_IO_OUTPUT);			// LCD_ROM_IN
     	DrvGPIO_Open(E_GPA, 1, E_IO_INPUT);			    // LCD_ROM_OUT
     	DrvGPIO_Open(E_GPA, 2, E_IO_OUTPUT);			// LCD_ROM_SCK
     	DrvGPIO_Open(E_GPA, 3, E_IO_OUTPUT);			// LCD_ROM_CS
     	DrvGPIO_Open(E_GPA, 4, E_IO_OUTPUT);			// LCD_RS(CD)
     	DrvGPIO_Open(E_GPA, 5, E_IO_OUTPUT);			// LCD_RESET
}

void Initial_lcd(void)
{
	
	SYSCLK->APBCLK.SPI2_EN  =1;			 //enable spi function 
	SYS->IPRSTC2.SPI2_RST   =1;			 //reset spi function
	SYS->IPRSTC2.SPI2_RST   =0;

	/* set GPIO to SPI mode*/
	SYS->GPDMFP.SPI2_SS0 	=1;
	SYS->GPDMFP.SPI2_CLK_SPI0_SS1 	=1;
	//SYS->GPDMFP.SPI2_MISO0 	=1;
	SYS->GPDMFP.SPI2_MOSI0_SPI0_MOSI1 	=1;

	SPI_PORT[eDRVSPI_PORT2]->CNTRL.CLKP = 1;							//CLKP HIGH IDLE
	SPI_PORT[eDRVSPI_PORT2]->CNTRL.TX_BIT_LEN = 8;						//TX LEGTH 8   /////////////////////////////////////////////////////
	SPI_PORT[eDRVSPI_PORT2]->CNTRL.TX_NEG = 1;							//SET TX_NEG
	SPI_PORT[eDRVSPI_PORT2]->DIVIDER.DIVIDER=0X00;					    //SET DIV

							
	DrvGPIO_ClrBit(E_GPA,5);										   	// rst=1
	SysTimerDelay(100000);
	DrvGPIO_SetBit(E_GPA,5);
	SysTimerDelay(100000);

	transfer_command_lcd(0xe2);		//��λ
	SysTimerDelay(5000);
	transfer_command_lcd(0x2c);		//���Ĳ���1
	SysTimerDelay(50000);								 
	transfer_command_lcd(0x2e);		  //���Ĳ���2
	SysTimerDelay(50000);
	transfer_command_lcd(0x2f);			//��ѹ����3
	SysTimerDelay(5000);
	transfer_command_lcd(0x23);		   //�����Աȶȣ������÷�Χ0x20-0x27
	transfer_command_lcd(0x81);		   //΢���Աȶ�
	transfer_command_lcd(0x28);		   //΢���Աȶȵ�ֵ�������÷�Χ0x00-0x3f
	transfer_command_lcd(0xa2);		   // 1/9ƫѹ�ȣ�bias��
	transfer_command_lcd(0xc8);		 //��ɨ��˳�򣬴��ϵ���
	transfer_command_lcd(0xa0);		 //��ɨ��˳�򣬴�����
	transfer_command_lcd(0x40);		  //��ʼ�У���һ�п�ʼ
	transfer_command_lcd(0xaf);		  //����ʾ
}

void clear_screen()
{
	unsigned int i,j;
	for (i=0;i<9;i++)
	{
		  transfer_command_lcd(0xb0+i);
		  transfer_command_lcd(0x10);
		  transfer_command_lcd(0x00);
		  for (j=0;j<132;j++)
		  {
		  	transfer_data_lcd(0x00);
		  }
	}
}


//*******************************
//this function to initial start address
//*******************************

void lcd_address(uint8_t page,uint8_t column)
{
	column=column-0x01;
	transfer_command_lcd(0xb0+page-1); //����ҳ��ַ��ÿ8 ��Ϊһҳ��ȫ����64 �У����ֳ�8 ҳ����1-8���
	transfer_command_lcd(0x10+(column>>4&0x0f)); //�����е�ַ�ĸ�4 λ
	transfer_command_lcd(column&0x0f); //�����е�ַ�ĵ�4 λ
}

//��ʾ128x64 ����ͼ��
void display_128x64(unsigned char *dp)
{
	uint8_t i,j;
	for(j=0;j<8;j++)
	{
		lcd_address(j+1,1);
		for (i=0;i<128;i++)
		{
			transfer_data_lcd(*dp); //д���ݵ�LCD,ÿд��һ��8 λ�����ݺ��е�ַ�Զ���1
			dp++;
		}
	}
}

//��ʾ16x16 ����ͼ�񡢺��֡���Ƨ�ֻ�16x16 ���������ͼ��
void display_graphic_16x16(unsigned char page,unsigned char column,unsigned char *dp)
{
	int i,j;
	for(j=0;j<2;j++)
	{
		lcd_address(page+j,column);
		for (i=0;i<16;i++)
		{
			transfer_data_lcd(*dp); //д���ݵ�LCD,ÿд��һ��8 λ�����ݺ��е�ַ�Զ���1
			dp++;
		}
	}
}

//��ʾ8x16 ����ͼ��ASCII, ��8x16 ����������ַ�������ͼ��
void display_graphic_8x16(unsigned char page,unsigned char column,unsigned char *dp)
{
	uint8_t i,j;
	for(j=0;j<2;j++)
	{
		lcd_address(page+j,column);
		for (i=0;i<8;i++)
		{
			transfer_data_lcd(*dp); //д���ݵ�LCD,ÿд��һ��8 λ�����ݺ��е�ַ�Զ���1
			dp++;
		}
	}
}

//��ʾ5X8 ����ͼ��ASCII, ��5x8 ����������ַ�������ͼ��
void display_graphic_5x8(unsigned char page,unsigned char column,unsigned char *dp)
{
	uint8_t i;
	lcd_address(page,column);
	for (i=0;i<6;i++)
	{
		transfer_data_lcd(*dp);
		dp++;
	}
}

//��ָ�����Ѷ�ֿ�IC
void send_command_to_ROM( unsigned char datu )
{
	unsigned char i;
	for(i=0;i<8;i++ )
	{
		LCD_ROM_SCK_L;
		SysTimerDelay(10);
		if(datu&0x80) 
			LCD_ROM_IN_H;
		else 
			LCD_ROM_IN_L;
		datu = datu<<1;
		LCD_ROM_SCK_H;
		SysTimerDelay(10);
	}
}

//�Ӿ���Ѷ�ֿ�IC ��ȡ���ֻ��ַ����ݣ�1 ���ֽڣ�
static unsigned char get_data_from_ROM( )
{
	unsigned char i;
	unsigned char ret_data=0;
	for(i=0;i<8;i++)
	{
		LCD_ROM_OUT_H;
		LCD_ROM_SCK_L;
		//delay_us(1);
		ret_data=ret_data<<1;
		if( DrvGPIO_GetBit(E_GPA, 1) == 1 )
			ret_data=ret_data+1;
		else
			ret_data=ret_data+0;
		LCD_ROM_SCK_H;
		//delay_us(1);
	}
	return(ret_data);
}

//��ָ����ַ��������д��Һ����ָ����page,column)������
void get_and_write_16x16(unsigned long fontaddr,unsigned char page,unsigned char column,unsigned char inverse)
{
	unsigned char i,j,disp_data;
	LCD_ROM_CS_L;
	send_command_to_ROM(0x03);
	send_command_to_ROM((fontaddr&0xff0000)>>16); //��ַ�ĸ�8 λ,��24 λ
	send_command_to_ROM((fontaddr&0xff00)>>8); //��ַ����8 λ,��24 λ
	send_command_to_ROM(fontaddr&0xff); //��ַ�ĵ�8 λ,��24 λ
	for(j=0;j<2;j++)
	{
		lcd_address(page+j,column);
		for(i=0; i<16; i++ )
		{
			disp_data=get_data_from_ROM();
			if (inverse==0) transfer_data_lcd(disp_data);  //д���ݵ�LCD,ÿд��1 �ֽڵ����ݺ��е�ַ�Զ���1
			else transfer_data_lcd(~disp_data); 
		}
	}
	LCD_ROM_CS_H;
}

//��ָ����ַ��������д��Һ����ָ����page,column)������
void get_and_write_8x16(unsigned long fontaddr,unsigned char page,unsigned char column,unsigned char inverse)
{
	unsigned char i,j,disp_data;
	LCD_ROM_CS_L;
	send_command_to_ROM(0x03);
	send_command_to_ROM((fontaddr&0xff0000)>>16); //��ַ�ĸ�8 λ,��24 λ
	send_command_to_ROM((fontaddr&0xff00)>>8); //��ַ����8 λ,��24 λ
	send_command_to_ROM(fontaddr&0xff); //��ַ�ĵ�8 λ,��24 λ
	for(j=0;j<2;j++)
	{
		lcd_address(page+j,column);
		for(i=0; i<8; i++ )
		{
			disp_data=get_data_from_ROM();
			if (inverse==0) transfer_data_lcd(disp_data); //д���ݵ�LCD,ÿд��1 �ֽڵ����ݺ��е�ַ�Զ���1
			else transfer_data_lcd(~disp_data);
 		}
	}
	LCD_ROM_CS_H;
}



//****************************************************************
unsigned long fontaddr=0;
void display_GB2312_string(unsigned char page,unsigned char column,unsigned char *text,unsigned char inverse)
{
	unsigned char i= 0;
	while((text[i]>0x00))
	{
		if(((text[i]>=0xb0) &&(text[i]<=0xf7))&&(text[i+1]>=0xa1))
		{
			//������壨GB2312�������ھ���Ѷ�ֿ�IC �еĵ�ַ�����¹�ʽ�����㣺
			//Address = ((MSB - 0xB0) * 94 + (LSB - 0xA1)+ 846)*32+ BaseAdd;BaseAdd=0
			//���ڵ���8 λ��Ƭ���г˷�������⣬���Է�����ȡ��ַ
			fontaddr = (text[i]- 0xb0)*94;
			fontaddr += (text[i+1]-0xa1)+846;
			fontaddr = (unsigned long)(fontaddr*32);
			get_and_write_16x16(fontaddr,page,column,inverse); //��ָ����ַ��������д��Һ����ָ����page,column)������
			i+=2;
			column+=16;
		}
		else if(((text[i]>=0xa1) &&(text[i]<=0xa3))&&(text[i+1]>=0xa1))
		{
			//������壨GB2312��15x16 ����ַ��ھ���Ѷ�ֿ�IC �еĵ�ַ�����¹�ʽ�����㣺
			//Address = ((MSB - 0xa1) * 94 + (LSB - 0xA1))*32+ BaseAdd;BaseAdd=0
			//���ڵ���8 λ��Ƭ���г˷�������⣬���Է�����ȡ��ַ
			fontaddr = (text[i]- 0xa1)*94;
			fontaddr += (text[i+1]-0xa1);
			fontaddr = (unsigned long)(fontaddr*32);
			get_and_write_16x16(fontaddr,page,column,inverse); //��ָ����ַ��������д��Һ����ָ����page,column)������
			i+=2;
			column+=16;
		}
		else if((text[i]>=0x20) &&(text[i]<=0x7e))
		{
			fontaddr = (text[i]- 0x20);
			fontaddr = (unsigned long)(fontaddr*16);
			fontaddr = (unsigned long)(fontaddr+0x3cf80);
			get_and_write_8x16(fontaddr,page,column,inverse); //��ָ����ַ��������д��Һ����ָ����page,column)������
			i+=1;
			column+=8;
		}
		else
			i++;
	}
}



