/*
 * simulation_line_follower.c
 *
 * Created: 12-04-2020 12:06:28
 * Author : Aadesh
 */ 

#include <avr/io.h>
#include <lcd.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdlib.h>
#define F_CPU 1000000UL

static volatile int pulse = 0;
static volatile int i = 0;
unsigned char speed1=0,speed2=0;


void IR_sen()
{
	DDRA = 0x00;
	DDRB = 0xFF;
	DDRD = 0xFF;
	int CS, LS, RS;
	Initialise();
	void init_pwm(unsigned char speed1,unsigned char speed2)
	{
		
		DDRB |= (1 << PB3);
		TCCR0 = 0x6D;
		OCR0 = speed1;
		DDRD |= (1 << PD7);
		TCCR2 = 0x6D;
		OCR2 = speed2;
	}
	void ultrasonic()
	{
		int16_t count_a = 0;
		char show_a[16];
		Initialise();
		DDRD = 0b11111011;
		_delay_ms(50);

		Initialise();

		GICR |= 1<<INT0;
		MCUCR |= 1<<ISC00;
		Initialise();
		
		sei();
		
		while(1)
		{
			PORTD |= 1<<PIND0;
			_delay_us(15);

			PORTD &= ~(1<<PIND0);
			count_a = pulse/58;
			Send_A_String("hey!");
			GoToMrLCDLocation(2,2);
			//Send_A_String("Distance Sensor");
			//GoToMrLCDLocation(1,2);
			Send_A_String("Distance=");
			itoa(count_a,show_a,10);
			Send_A_String(show_a);
			Send_A_String(" ");
			GoToMrLCDLocation(13,2);
			Send_A_String("cm");
			GoToMrLCDLocation(1,1);
		}
	}
	

	
	
	while(1)
	{
		CS = PINA & 0b00000001;
		LS = PINA & 0b00000010;
		RS = PINA & 0b00000100; 
		//STARIGHT
		if((CS == 0b00000001) && (LS == 0b00000010) && (RS == 0b00000100))
		{
			PORTB = 0b00000001;
			PORTD = 0b01000000;
			init_pwm(1000,1000); 
		}
		
		//SOFT LEFT
		if((CS == 0b00000001) && (LS == 0b00000010) && (RS == 0b00000000))
		{
			PORTB = 0b00000001;
			PORTD = 0b00000000;
			init_pwm(1000,0); 
		}
		
		//SHARP LEFT
		if((CS == 0b00000000) && (LS == 0b00000010) && (RS == 0b00000000))
		{
			PORTB = 0b00000001;
			PORTD = 0b00000000;
			init_pwm(1000,0); 
		}
		
		//SHARP RIGHT
		if((CS == 0b00000000) && (LS == 0b00000000) && (RS == 0b00000100))
		{
			PORTB = 0b00000000;
			PORTD = 0b01000000;
			init_pwm(0,1000); 
		}
		
		//SOFT RIGHT
		if((CS == 0b00100001) && (LS == 0b00000000) && (RS == 0b00000100))
		{
			PORTB = 0b00000000;
			PORTD = 0b01000000;
			init_pwm(0,1000); 
		}
		
		//STOP
		if((CS == 0b00000000) && (LS == 0b00000000) && (RS == 0b00000000))
		{
			PORTB = 0b00000000;
			PORTD = 0b00000000;
			init_pwm(0,0);
		}
	}
}






int main(void)
{
   IR_sen();
   return 0;  
    
}

ISR(INT0_vect)
{
	if(i == 1)
	{
		TCCR1B = 0;
		pulse = TCNT1;
		TCNT1 = 0;
		i = 0;
	}

	if(i==0)
	{
		TCCR1B |= 1<<CS10;
		i = 1;
	}
}
