#include "NUC1xx.h"
#include "Driver\DrvSYS.h"
#include "Driver\DrvSPI.h"
#include "Driver\DrvGPIO.h"

extern  SPI_T * SPI_PORT[4]={SPI0, SPI3, SPI2, SPI3};

extern  char Ascii[];

//伪指令
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
	DrvGPIO_ClrBit(E_GPA,4);											// rs=0;指令寄存器
   
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
	DrvGPIO_SetBit(E_GPA,4);										    // rs=1; 数据寄存器
   
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

	transfer_command_lcd(0xe2);		//软复位
	SysTimerDelay(5000);
	transfer_command_lcd(0x2c);		//升涯步骤1
	SysTimerDelay(50000);								 
	transfer_command_lcd(0x2e);		  //升涯步骤2
	SysTimerDelay(50000);
	transfer_command_lcd(0x2f);			//升压步骤3
	SysTimerDelay(5000);
	transfer_command_lcd(0x23);		   //粗条对比度，可设置范围0x20-0x27
	transfer_command_lcd(0x81);		   //微调对比度
	transfer_command_lcd(0x28);		   //微调对比度的值，可设置范围0x00-0x3f
	transfer_command_lcd(0xa2);		   // 1/9偏压比（bias）
	transfer_command_lcd(0xc8);		 //行扫描顺序，从上到下
	transfer_command_lcd(0xa0);		 //列扫描顺序，从左到右
	transfer_command_lcd(0x40);		  //起始行，第一行开始
	transfer_command_lcd(0xaf);		  //开显示
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
	transfer_command_lcd(0xb0+page-1); //设置页地址，每8 行为一页，全屏共64 行，被分成8 页，第1-8编号
	transfer_command_lcd(0x10+(column>>4&0x0f)); //设置列地址的高4 位
	transfer_command_lcd(column&0x0f); //设置列地址的低4 位
}

//显示128x64 点阵图像
void display_128x64(unsigned char *dp)
{
	uint8_t i,j;
	for(j=0;j<8;j++)
	{
		lcd_address(j+1,1);
		for (i=0;i<128;i++)
		{
			transfer_data_lcd(*dp); //写数据到LCD,每写完一个8 位的数据后列地址自动加1
			dp++;
		}
	}
}

//显示16x16 点阵图像、汉字、生僻字或16x16 点阵的其他图标
void display_graphic_16x16(unsigned char page,unsigned char column,unsigned char *dp)
{
	int i,j;
	for(j=0;j<2;j++)
	{
		lcd_address(page+j,column);
		for (i=0;i<16;i++)
		{
			transfer_data_lcd(*dp); //写数据到LCD,每写完一个8 位的数据后列地址自动加1
			dp++;
		}
	}
}

//显示8x16 点阵图像、ASCII, 或8x16 点阵的自造字符、其他图标
void display_graphic_8x16(unsigned char page,unsigned char column,unsigned char *dp)
{
	uint8_t i,j;
	for(j=0;j<2;j++)
	{
		lcd_address(page+j,column);
		for (i=0;i<8;i++)
		{
			transfer_data_lcd(*dp); //写数据到LCD,每写完一个8 位的数据后列地址自动加1
			dp++;
		}
	}
}

//显示5X8 点阵图像、ASCII, 或5x8 点阵的自造字符、其他图标
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

//送指令到晶联讯字库IC
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

//从晶联讯字库IC 中取汉字或字符数据（1 个字节）
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

//从指定地址读出数据写到液晶屏指定（page,column)座标中
void get_and_write_16x16(unsigned long fontaddr,unsigned char page,unsigned char column,unsigned char inverse)
{
	unsigned char i,j,disp_data;
	LCD_ROM_CS_L;
	send_command_to_ROM(0x03);
	send_command_to_ROM((fontaddr&0xff0000)>>16); //地址的高8 位,共24 位
	send_command_to_ROM((fontaddr&0xff00)>>8); //地址的中8 位,共24 位
	send_command_to_ROM(fontaddr&0xff); //地址的低8 位,共24 位
	for(j=0;j<2;j++)
	{
		lcd_address(page+j,column);
		for(i=0; i<16; i++ )
		{
			disp_data=get_data_from_ROM();
			if (inverse==0) transfer_data_lcd(disp_data);  //写数据到LCD,每写完1 字节的数据后列地址自动加1
			else transfer_data_lcd(~disp_data); 
		}
	}
	LCD_ROM_CS_H;
}

//从指定地址读出数据写到液晶屏指定（page,column)座标中
void get_and_write_8x16(unsigned long fontaddr,unsigned char page,unsigned char column,unsigned char inverse)
{
	unsigned char i,j,disp_data;
	LCD_ROM_CS_L;
	send_command_to_ROM(0x03);
	send_command_to_ROM((fontaddr&0xff0000)>>16); //地址的高8 位,共24 位
	send_command_to_ROM((fontaddr&0xff00)>>8); //地址的中8 位,共24 位
	send_command_to_ROM(fontaddr&0xff); //地址的低8 位,共24 位
	for(j=0;j<2;j++)
	{
		lcd_address(page+j,column);
		for(i=0; i<8; i++ )
		{
			disp_data=get_data_from_ROM();
			if (inverse==0) transfer_data_lcd(disp_data); //写数据到LCD,每写完1 字节的数据后列地址自动加1
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
			//国标简体（GB2312）汉字在晶联讯字库IC 中的地址由以下公式来计算：
			//Address = ((MSB - 0xB0) * 94 + (LSB - 0xA1)+ 846)*32+ BaseAdd;BaseAdd=0
			//由于担心8 位单片机有乘法溢出问题，所以分三部取地址
			fontaddr = (text[i]- 0xb0)*94;
			fontaddr += (text[i+1]-0xa1)+846;
			fontaddr = (unsigned long)(fontaddr*32);
			get_and_write_16x16(fontaddr,page,column,inverse); //从指定地址读出数据写到液晶屏指定（page,column)座标中
			i+=2;
			column+=16;
		}
		else if(((text[i]>=0xa1) &&(text[i]<=0xa3))&&(text[i+1]>=0xa1))
		{
			//国标简体（GB2312）15x16 点的字符在晶联讯字库IC 中的地址由以下公式来计算：
			//Address = ((MSB - 0xa1) * 94 + (LSB - 0xA1))*32+ BaseAdd;BaseAdd=0
			//由于担心8 位单片机有乘法溢出问题，所以分三部取地址
			fontaddr = (text[i]- 0xa1)*94;
			fontaddr += (text[i+1]-0xa1);
			fontaddr = (unsigned long)(fontaddr*32);
			get_and_write_16x16(fontaddr,page,column,inverse); //从指定地址读出数据写到液晶屏指定（page,column)座标中
			i+=2;
			column+=16;
		}
		else if((text[i]>=0x20) &&(text[i]<=0x7e))
		{
			fontaddr = (text[i]- 0x20);
			fontaddr = (unsigned long)(fontaddr*16);
			fontaddr = (unsigned long)(fontaddr+0x3cf80);
			get_and_write_8x16(fontaddr,page,column,inverse); //从指定地址读出数据写到液晶屏指定（page,column)座标中
			i+=1;
			column+=8;
		}
		else
			i++;
	}
}



