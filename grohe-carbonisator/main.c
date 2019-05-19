#include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <util/delay.h>
#include <math.h>
#include "lcd.h"
#include "sht11_costum/sht11.h"
#define WAIT 5
int counter = 0;
int state = 0;
int tmp;
char buffer[64] = {0};
int leftBtn = 1; int leftBtnLast = 1; int leftFlag = 0;
int rightBtn = 1; int rightBtnLast = 1; int rightFlag = 0;
int plusBtn = 1; int plusBtnLast = 1; int plusFlag = 0;
int minusBtn = 1; int minusBtnLast = 1; int minusFlag = 0;
int menuBtn = 1; int menuBtnLast = 1; int menuFlag = 0;

///////////////////////////////////////////////////////////
void printReady(int val, char* buffer)
{
    int j,k,len,offset = 0;
    char tempBuff[10]={""};
    memset(buffer, 0, 32);

    sprintf(tempBuff,"%d",val);
    len = strlen(tempBuff);

    if (tempBuff[0]=='-' && len<4)
    {
        switch (len)
        {
            case 2:
                buffer[0] = '-';
                buffer[1] = '0';
                buffer[2] = '.';
                buffer[3] = '0';
                buffer[4] = tempBuff[1];
                break;
            case 3:
                buffer[0] = '-';
                buffer[1] = '0';
                buffer[2] = '.';
                buffer[3] = tempBuff[1];
                buffer[4] = tempBuff[2];
                break;
        }

    }
    else
    {
    	if (len > 2)
    	{
			for(j=len-1 ; j>-1 ; j--)
			{
				k = j+1;
				if (j == len-3)
				{
					buffer[k] = '.';
					offset += 1;
					buffer[k-offset] = tempBuff[j];
				}
				else
				{
					buffer[k-offset] = tempBuff[j];
				}
			}
    	}
    	else
    	{
            switch (len)
            {
                case 1:
                    buffer[0] = '0';
                    buffer[1] = '.';
                    buffer[2] = '0';
                    buffer[3] = tempBuff[0];
                    break;
                case 2:
                    buffer[0] = '0';
                    buffer[1] = '.';
                    buffer[2] = tempBuff[0];
                    buffer[3] = tempBuff[1];
                    break;
            }
    	}
    }
}
void setup()
{
	DDRA &= ~(1<<PA0);
	DDRA &= ~(1<<PA1);
	DDRA &= ~(1<<PA2);
	DDRA &= ~(1<<PA3);
	DDRA &= ~(1<<PA4);

	DDRC |= (1<<PC0);
	DDRC |= (1<<PC1);
	DDRC |= (1<<PC2);
	DDRC |= (1<<PC3);
	DDRC |= (1<<PC4);

	PORTC = 0x00;
}
void init_timer()
{
	TCCR1B |= (1 << CS12);
}
void mainMenu()
{
	sht11_start_temp();
	while(!sht11_ready()){;}
	tmp = sht11_result_temp();
	printReady(tmp, buffer);
	lcd_clrscr();
	lcd_home();
	lcd_puts("water cooling");
	lcd_gotoxy(0,1);
	lcd_puts(buffer);
	lcd_puts(" *C");
	_delay_ms(200);
}
void welcome()
{
	lcd_init(LCD_DISP_ON);
	lcd_clrscr();
	lcd_home();
	lcd_puts("water carbonator");
	lcd_gotoxy(0,1);
	lcd_puts("initializing");
	_delay_ms(500);
	mainMenu();
}
void menu1()
{
	lcd_clrscr();
	lcd_home();
	lcd_puts("carbonated water");
	lcd_gotoxy(0,1);
	lcd_puts("2dcl> constant<");
}
void menu2()
{
	lcd_clrscr();
	lcd_home();
	lcd_puts("cooled water");
	lcd_gotoxy(0,1);
	lcd_puts("2dcl> constant<");
}
void menu3()
{
	lcd_clrscr();
	lcd_home();
	lcd_puts("clean & empty");
	lcd_gotoxy(0,1);
	lcd_puts("start>");
}
void subMenu3()
{
	lcd_clrscr();
	lcd_home();
	lcd_puts("remove CO2&H20");
	lcd_gotoxy(0,1);
	lcd_puts("start>");
}
void menu4()
{
	lcd_clrscr();
	lcd_home();
	lcd_puts("fill with H2O");
	lcd_gotoxy(0,1);
	lcd_puts("start>");
}
void menu5()
{
	lcd_clrscr();
	lcd_home();
	lcd_puts("test menu");
	lcd_gotoxy(0,1);
	lcd_puts("O1> O2< O3- O4+");
}
void dcl(int carb)
{
   if (carb == 1)
   {
      PORTC = 0x07;
      _delay_ms(3000);
      PORTC = 0x00;
   }
   else if (carb == 2) // cooled
   {
      PORTC = 0x0b;
      _delay_ms(3000);
      PORTC = 0x00;
   }
   else
   {
      PORTC = 0x00;
   }
}
void constant(int carb)
{
   if (carb == 1)
   {
      PORTC = 0x07;
   }
   else if (carb == 2) // cooled
   {
      PORTC = 0x0b;
   }
}
void resetInput()
{
   leftFlag = 0;
   rightFlag = 0;
   plusFlag = 0;
   minusFlag = 0;
}
void cleanSystem()
{

	lcd_clrscr();
	lcd_home();
	lcd_puts("water opened");
	PORTC = 0x0b;
	_delay_ms(13000);
	PORTC = 0x00;
	lcd_clrscr();
	lcd_home();
	lcd_puts("CO2 opened");
	_delay_ms(500);
	PORTC = 0x07;
	_delay_ms(13000);
	PORTC = 0x00;
}
void fillSystem()
{
	lcd_clrscr();
	lcd_home();
	lcd_puts("water opened");
	PORTC = 0x0b;
	_delay_ms(13000);
	PORTC = 0x00;
	lcd_clrscr();
	lcd_home();
	lcd_puts("CO2 opened");
	_delay_ms(500);
	PORTC = 0x07;
	_delay_ms(4000);
	PORTC = 0x00;
}
///////////////////////////////////////////////////////////
int main()
{
	setup();
	welcome();
	init_timer();
	while (1)
	{
		if (TCNT1 >= 43200)
		{
			counter++;
			TCNT1 = 0;
		}
		/////////////////////////////////////////////////
		_delay_ms(50);
		rightBtn = (PINA & (1<<PA0));
		leftBtn = (PINA & (1<<PA1));
		minusBtn = (PINA & (1<<PA2));
		plusBtn = (PINA & (1<<PA3));
		menuBtn = (PINA & (1<<PA4));

		if (rightBtn != rightBtnLast)
		{
			rightBtnLast = rightBtn;
			if (rightBtn == 0)
			{
				rightFlag = 1;
			}
		}
		if (leftBtn != leftBtnLast)
		{
			leftBtnLast = leftBtn;
			if (leftBtn == 0)
			{
				leftFlag = 1;
			}
		}
		if (minusBtn != minusBtnLast)
		{
			minusBtnLast = minusBtn;
			if (minusBtn == 0)
			{
				minusFlag = 1;
			}
		}
		if (plusBtn != plusBtnLast)
		{
			plusBtnLast = plusBtn;
			if (plusBtn == 0)
			{
				plusFlag = 1;
			}
		}
		if (menuBtn != menuBtnLast)
		{
			menuBtnLast = menuBtn;
			if (menuBtn == 0)
			{
				menuFlag = 1;
			}
		}
		/////////////////////////////////////////////////
		switch (state)
		{
			case 0: //pocetno stanje
				if (counter >= WAIT) // <<< Ovdje ide citanje SHT senzora
				{
					counter = 0;
					mainMenu();
				}
				if ((PINA & (1<<PA0)) == 0 && (PINA & (1<<PA1)) == 0) //test mode
				{
					lcd_clrscr();
					lcd_home();
					lcd_puts("entering");
					lcd_gotoxy(0,1);
					lcd_puts("test mode");
					_delay_ms(800);
					resetInput();
					menu5();
					menuFlag = 0;
					counter = 0;
					state = 5;
				}
				if (menuFlag == 1) //meni tipka
				{
					resetInput();
					menu1();
					menuFlag = 0;
					counter = 0;
					state = 1;
				}
				break;
			case 1: // carbonated water meni
				if (counter >= WAIT)
				{
					state = 0;
				}
				if (rightFlag == 1) //2dcl desno tipka
				{
					rightFlag = 0;
					counter = 0;
					dcl(1);
				}
				if ((PINA & (1<<PA1)) == 0) //constant lijevo tipka
				{
					counter = 0;
					constant(1);
				}
				else
				{
					PORTC = 0x00;
				}
				if (menuFlag == 1) //meni tipka
				{
					resetInput();
					menu2();
					menuFlag = 0;
					counter = 0;
					state = 2;
				}
				break;
			case 2: //cooled water meni
				if (counter >= WAIT)
				{
					state = 0;
				}
				if (rightFlag == 1) //2dcl desno tipka
				{
					rightFlag = 0;
					counter = 0;
					dcl(2);
				}
				if ((PINA & (1<<PA1)) == 0) //constant lijevo tipka
				{
					counter = 0;
					constant(2);
				}
				else
				{
					PORTC = 0x00;
				}
				if (menuFlag == 1) //meni tipka
				{
					menu3();
					resetInput();
					menuFlag = 0;
					counter = 0;
					state = 3;
				}
				break;
			case 3: //clean system
				if (counter >= WAIT)
				{
					state = 0;
				}
				if (rightFlag == 1) //desno tipka
				{
					subMenu3();
					state = 6;
					counter = 0;
					resetInput();
					menuFlag = 0;
				}
				if (menuFlag == 1) //meni tipka
				{
					menu4();
					state = 4;
					resetInput();
					menuFlag = 0;
					counter = 0;
				}
				break;
			case 4: //fill system
				if (counter >= WAIT)
				{
					state = 0;
				}
				if (rightFlag == 1) //desno tipka
				{
					resetInput();
					menuFlag = 0;
					counter = 0;
					fillSystem();
					state = 7;
				}
				if (menuFlag == 1) //meni tipka
				{
					mainMenu();
					resetInput();
					menuFlag = 0;
					counter = 0;
					state = 0;
				}
				break;
		/////////////////////////////////////////
			case 6: //subMenu 3
				if (rightFlag == 1) //desno tipka
				{
					state = 0;
					counter = 0;
					menuFlag = 0;
					resetInput();
					cleanSystem();

					lcd_clrscr();
					lcd_home();
					lcd_puts("system cleaned");
					_delay_ms(2500);

					mainMenu();
				}
				if (menuFlag == 1) //meni tipka
				{
					menu3();
					resetInput();
					menuFlag = 0;
					counter = 0;
					state = 3;
				}
				break;
			case 7: //subMenu4
				state = 0;
				counter = 0;
				menuFlag = 0;
				resetInput();

				lcd_clrscr();
				lcd_home();
				lcd_puts("system is filled");

				_delay_ms(2500);

				mainMenu();
				break;
		/////////////////////////////////////////
			case 5:
				if ((PINA & (1<<PA0)) == 0) //desno
				{PORTC |= (1<<PC0);}
				else
				{PORTC &= ~(1<<PC0);}

				if ((PINA & (1<<PA1)) == 0) //lijevo
				{PORTC |= (1<<PC1);}
				else
				{PORTC &= ~(1<<PC1);}

				if ((PINA & (1<<PA2)) == 0) //-
				{PORTC |= (1<<PC2);}
				else
				{PORTC &= ~(1<<PC2);}

				if ((PINA & (1<<PA3)) == 0) //+
				{PORTC |= (1<<PC3);}
				else
				{PORTC &= ~(1<<PC3);}

				if (menuFlag == 1) //meni tipka
				{
					mainMenu();
					state = 0;
					resetInput();
					menuFlag = 0;
					counter = 0;
				}
				break;
			default:
				state = 0;
				mainMenu();
				_delay_ms(500);
				break;
		}
	}
   return 0;
}
