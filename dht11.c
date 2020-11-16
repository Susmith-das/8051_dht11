#include <reg51.h>
#include <intrins.h>

#define lcd P2 //Data pins connected to port 2 on microcontroller

sbit DHT = P1^3;    //pin for DHT
sbit rs  = P3^2;   //RS pin connected to pin 2 of port 3
sbit e   = P3^3; //E pin connected to pin 4 of port 3

unsigned short int I_humidity,D_humidity,I_temperature,D_temperature,Checksum;

void delay(unsigned short int count)// Function for creating delay in milliseconds.
{
	unsigned short int i;

	while(count)
	{
		i=115;
		while(i>0)
			i--;
		count--;
	}
} 

void us_delay()// Function for creating delay of 40microseconds.
{
	_nop_();_nop_();_nop_();_nop_();_nop_();
	_nop_();_nop_();_nop_();_nop_();_nop_();
	_nop_();_nop_();_nop_();_nop_();_nop_();
	_nop_();_nop_();_nop_();_nop_();_nop_();
	_nop_();_nop_();_nop_();_nop_();_nop_();
	_nop_();_nop_();_nop_();_nop_();_nop_();
	_nop_();_nop_();_nop_();_nop_();_nop_();
	_nop_();_nop_();_nop_();_nop_();_nop_();	
} 

void Request()     //  SEND REQUEST
{
	DHT=0;
	delay(20);
	DHT=1;
}

void Response()    //  RECIEVE RESPONSE FROM DHT 11
{
	while(DHT == 1);
	while(DHT == 0);
	while(DHT == 1);
}

unsigned short int Receive_data()        //Receive_data
{
  unsigned short int k,dataD =0;
	
	for(k=0;k<8;k++)
	{
		while(DHT==0);
		us_delay();

		if(DHT == 1)
			dataD =((dataD<<1)|0x01);
		else
			dataD = (dataD<<1);
		
		while(DHT==1);
	}
	return dataD;
}


void lcd_cmd(unsigned char x)  //Function to send command instruction to LCD
{
    lcd = x;
    rs= 0;
    e=1;
    delay(20);
    e=0;
}

void lcd_data(unsigned char y)  //Function to send display_data to LCD
{
    lcd = y;
    rs= 1;
    e=1;
    delay(20);
    e=0;
}

 void lcd_init()    //Function to prepare the LCD  and get it ready
{
    lcd_cmd(0x38);  // for_using 2 lines and 5X7 matrix of LCD
    delay(10);
    lcd_cmd(0x0F);  // turn display ON, cursor blinking
    delay(10);
    lcd_cmd(0x01);  //clear screen
    delay(10);
    lcd_cmd(0x80);  // bring cursor to position 1 of line 1
    delay(10);
}

void lcd_str(unsigned char *s)
{
	unsigned short int w;
	for(w =0;s[w]!='\0';w++)
	{
		lcd_data(s[w]);
		delay(10);
	}
}
void main()
{
	  unsigned short int a,b,c,d;
    	  DHT=1;	
    	  lcd_init();
	
	  lcd_cmd(0x84);
	
	  lcd_str("welcome");
	  delay(2000);
	
	  lcd_cmd(0x01);   //clear screen
	  lcd_cmd(0x80);  // bring cursor to position 1 of line 1
	
	  while(1)
	  {
			Request();  // sending starting pulse
			Response(); // receive response
		
			I_humidity = Receive_data();        //8_bit in I_humidity
			D_humidity = Receive_data();       //8_bit in D_humidity
			I_temperature = Receive_data();   //8_bit in I_temperature
			D_temperature = Receive_data();  //8_bit in D_temperature
			Checksum = Receive_data();      //8_bit in Checksum
			
			lcd_cmd(0x01);  //clear screen	
			lcd_cmd(0x80);   // bring cursor to position 1 of line 1
			
			if((I_humidity + D_humidity + I_temperature + D_temperature)!= Checksum)
			{
				lcd_str("ERROR");
			}
			else
			{
				a = (I_humidity/10)+48;
				b = (I_humidity%10)+48;
				c = (I_temperature/10)+48;
				d = (I_temperature%10)+48;
				
				lcd_cmd(0x80);   // bring cursor to position 1 of line 1
				
				lcd_str("HUMIDITY: ");
		   		lcd_data(a);	
				lcd_data(b);
				
				delay(1000);
				
				lcd_cmd(0xC0);  // bring cursor to position 1 of line 2
				lcd_str("TEMPERATURE: ");
		    		lcd_data(c);
				lcd_data(d);
				
				delay(1000);			
			}
		}
	}
