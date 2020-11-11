#include <reg51.h>

#define display_port P2 //Data pins connected to port 2 on microcontroller

sbit rs = P3^2;  //RS pin connected to pin 2 of port 3
sbit rw = P3^3;  //RW pin connected to pin 3 of port 3
sbit e  = P3^4;  //E pin connected to pin 4 of port 3

sbit DHT11 = P1^3;  //pin for DHT

int I_humidity,D_humidity,I_temperature,D_temperature,Checksum;

void msdelay(unsigned int time)  // Function for creating delay in milliseconds.
{
    unsigned i,j;
    for(i=0;i<time;i++)    
			for(j=0;j<1275;j++);
}

void Request()     // MUC SEND REQUEST
{
	DHT11 = 0;
	msdelay(18);
	DHT11=1;
}

void Response()    //  RECIEVE RESPONSE FROM DHT 11
{
	while(DHT11 == 1);
	while(DHT11 == 0);
	while(DHT11 == 1);
}

int Receive_data()        //Receive_data
{
	int i,dataD =0;
	for(i=0;i<8;i++)
	{
		while(DHT11==0);
		msdelay(40);
		
		if(DHT11 == 1)
			dataD =((dataD<<1)|0x01);
		else
			dataD = (dataD<<1);
		while(DHT11==1);
	}
	return dataD;
}


void lcd_cmd(unsigned char command)  //Function to send command instruction to LCD
{
    display_port = command;
    rs= 0;
    rw=0;
    e=1;
    msdelay(20);
    e=0;
}

void lcd_data(unsigned char disp_data)  //Function to send display_data to LCD
{
    display_port = disp_data;
    rs= 1;
    rw=0;
    e=1;
    msdelay(20);
    e=0;
}

 void lcd_init()    //Function to prepare the LCD  and get it ready
{
    lcd_cmd(0x38);  // for_using 2 lines and 5X7 matrix of LCD
    msdelay(10);
    lcd_cmd(0x0F);  // turn display ON, cursor blinking
    msdelay(10);
    lcd_cmd(0x01);  //clear screen
    msdelay(10);
    lcd_cmd(0x80);  // bring cursor to position 1 of line 1
    msdelay(10);
}

void lcd_str(unsigned char *s)
{
	unsigned int w;
	for(w =0;s[w]!='\0';w++)
	{
		lcd_data(s[w]);
		msdelay(10);
	}
}
void main()
{
	  unsigned int a,b,c,d;
	
    lcd_init();
	
	  lcd_str("welcome");
	  msdelay(1000);
	
	  lcd_cmd(0x01);   //clear screen
	  lcd_cmd(0x80);	 // bring cursor to position 1 of line 1
	
	  lcd_str("Humidity");
	  msdelay(1000);
	
	  lcd_cmd(0x01);  //clear screen
	  lcd_cmd(0x80);  // bring cursor to position 1 of line 1
	
	  lcd_str("Temperature");
	  msdelay(1000);
	
	  lcd_cmd(0x01);  //clear screen
	
	  while(1)
		{
			Request();  // sending starting pulse
			Response(); // receive response
			
			I_humidity = Receive_data();    //8_bit in I_humidity
			D_humidity = Receive_data();    //8_bit in D_humidity
			I_temperature = Receive_data(); //8_bit in I_temperature
			D_temperature = Receive_data(); //8_bit in D_temperature
			Checksum = Receive_data();      //8_bit in Checksum
			
			if((I_humidity + D_humidity + I_temperature + D_temperature)!= Checksum)
			{
				lcd_str("ERROR");
			}
			else
			{
				a = (I_humidity/10)+48;
				b = (I_humidity%10)+48;
				
				lcd_cmd(0x80);   // bring cursor to position 1 of line 1
		    lcd_data(a);
				
				lcd_cmd(0x83);	// bring cursor to position 4 of line 1
				lcd_data(b);
				
				msdelay(1000);
				
				lcd_cmd(0x01); //clear screen
				
				c = (I_temperature/10)+48;
				d = (I_temperature%10)+48;
				
				lcd_cmd(0x80);   // bring cursor to position 1 of line 1
		    lcd_data(c); 
				
				lcd_cmd(0x83);	// bring cursor to position 4 of line 1
				lcd_data(d);
				
				msdelay(1000);
				
				lcd_cmd(0x01);
				
				lcd_cmd(0x80);   // bring cursor to position 2 of line 1
				
			}
		}
	}