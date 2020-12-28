/***********************************************************/
/*  Author       : Sugumaran_A                             */
/*  File_Name    : USERINPUT_TIMER.c                       */         
/*  Description  : User input timer                        */
/*  Date         : 28/12/2020                              */
/***********************************************************/

#include "USERINPUT_TIMER.h" 

#define _XTAL_FREQ 20000000
__CONFIG(FOSC_HS & WDTE_OFF & PWRTE_OFF & BOREN_ON & LVP_OFF);

int inp_num = 0;

void my_delay(void);

//For LCD
#define RS RC0
#define RW RC1
#define EN RC2

void lcd_cmd(unsigned char cmd)
{
	RS = 0;
	RW = 0;
	EN = 1;
	PORTD = cmd;
	__delay_ms(50);
	EN = 0;
}

void lcd_data(unsigned char data)
{
	RS = 1;
	RW = 0;
	EN = 1;
	PORTD = data;
	__delay_ms(50);
	EN = 0;
}

void print_lcd(unsigned char *string)
{
	while(*string)
	{
		lcd_data(*string);
		__delay_ms(10);
		string++;
	}
}


void interrupt ISR(void)
{
	if(INTF == 1)
 	{
		lcd_cmd(0x01); //clear display
		lcd_cmd(0x80); //cursor Begin at first line
		print_lcd("USER INPUTMODE\0");
		RB1=RB2=RB3=0;
		while(INTF == 1)
		{
			while(RB3!=1)
			{	
				if(RB1 == 1){
					__delay_ms(160);
					inp_num++;
				}
				if(RB2 == 1){
					__delay_ms(160);
					inp_num = inp_num - 1;
				}
				lcd_cmd(0x40);//2 line
				lcd_cmd(0xC0); //cursor Begin at first line
				
				int i,j;
				if(inp_num <0 || inp_num > 99)
					inp_num = 0;
				
				i = ((inp_num/10)%10);
				lcd_data(i+48);
				__delay_ms(50);
				
				j = inp_num%10;
				lcd_data(j+48);
				__delay_ms(50);
				
			}
			if(RB3 == 1)
			{
				__delay_ms(50);
				INTF = 0;
				my_delay();
				RE0 = 1;
			}
		
		}
	}
}

void my_delay()
{
	int i,j,num1,num2;
	num1=num2= 0;
	int tot_num = inp_num;
	lcd_cmd(0x01); //clear display	
	
	for ( i=1; i <= inp_num; i++ )
	{
		lcd_cmd(0x80); //cursor Begin at first line
		print_lcd("TIMER STARTED\0");
		lcd_cmd(0xC0); //cursor Begin at second line
		num1 = ((tot_num/10)%10);
		lcd_data(num1+48);
		__delay_ms(10);
		num2 = tot_num%10;
		lcd_data(num2+48);
		__delay_ms(10);
		for( j=1; j <= 30; j++)
		{	
			RC3 = 1;
			__delay_ms(1000);
			RC3 = 0;
			__delay_ms(1000);
		}
		tot_num = tot_num - 1;
		if(tot_num == 0)
		{
			lcd_cmd(0x01); //clear display
			lcd_cmd(0x80); //cursor Begin at FIRST line
			lcd_cmd(0x0c); //Display ON/OFF ,cursor blink off
			print_lcd("TIMER ENDED\0");
		}
	}
	
}

void main()
{
	TRISB = 0X0F;// INPUT BUTTON
	TRISC = 0X00;// FOR LCD
	TRISD = 0X00;// FOR LCD
	TRISE = 0;// To which we need to turn ON/OFF we can connect that in this PIN
	
	OPTION_REG = 0x00;	
	INTEDG = 1;//rising edge or falling edge
	INTCON = 0x00;
	GIE = 1;
	INTE = 1;
	PORTC = 0;
	PORTD = 0;
	PORTE = 0;
	lcd_cmd(0x38); //2lines and 5x7 matrix 
	lcd_cmd(0x01); //clear display
	lcd_cmd(0x02); //return home
	lcd_cmd(0x0d); //Display ON/OFF ,cursor blink off
	lcd_cmd(0x06); //Increment cursor to Right
	lcd_cmd(0x80); //cursor Begin at first line
	
	while(1);
}
