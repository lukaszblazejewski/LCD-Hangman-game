#include "stm32f4xx.h"

#define LCD_MODE     		0x06		// inkrementacja, brak przesuni�cia zawarto�ci DDRAM
#define LCD_CLEAR    		0x01		// czy�ci wy�wietlacz i ustawia kursor na pocz�tku (adres=0)
#define LCD_ON		 		0x0c		// w��czenie wy�wietlacza, wy��czenie kursora, wy��czenie migania kursora
#define LCD_OFF		 		0x08		// wy��czenie wy�wietlacza, wy��czenie kursora, wy��czenie migania kursora
#define LCD_8bit_data 		0x38		// 8 linii danych, dwu liniowy, matryca 5x7 punkt�w
#define LCD_DISP_SHIFT		0x18		// Przesuwanie ekranu o jedno pole
#define _BV(bit) (1<<bit)
//przyporz�dkowanie sygna�om wy�wietlacza LCD odpowiednich port�w mikrokontrolera
#define	LCD_DATA_OUT	GPIOE->ODR
#define	LCD_CONTROL		GPIOD->ODR

// ustawienie sygna��w sterujacych dla rozpocz�cia zapisu instrukcji
#define LCD_WR_INST	GPIOD-> BSRR=0b00000001100000000000000000000000
// ustawienie sygna��w sterujacych dla rozpocz�cia zapisu danej
#define LCD_WR_DATA	GPIOD->BSRR=0b00000001000000000000000010000000
// ustawienie sygna��w sterujacych dla rozpocz�cia odczytu flagi zaj�to�ci
#define LCD_RD_FLAG	GPIOD->BSRR=0b00000000100000000000000100000000
// ustawienie sygna��w sterujacych dla rozpocz�cia odczytu danej
#define LCD_RD_DATA	GPIOD->BSRR=0b00000000000000000000000110000000
// ustawienie sygna�u sterujacego EN
#define LCD_EN_SET	GPIOD->BSRR=0b00000000000000000000001000000000
// wyzerowanie sygna�u sterujacego EN
#define LCD_EN_CLEAR	GPIOD->BSRR = 0b00000010000000000000000000000000
// Ustawienie sygnalu sterujacego dla przesuwania napisu w lewo


void check_busy_flag (void)
{
	uint16_t i = 0;
	char flag = 0x80;
		  LCD_RD_FLAG;
		  GPIOE -> MODER = 0x00000000;
			 for(i = 0; i < 15; i++)
				 asm("nop");
		  while (flag != 0){
			LCD_EN_SET;				// ustawienie linii EN
			 for(i = 0; i < 40; i++)
				 asm("nop");
			flag = ((GPIOD -> IDR) & _BV(11));
			LCD_EN_CLEAR;			// zerowanie linii EN
			HAL_Delay(1);
		};
		  GPIOE -> MODER = 0b00000000010101010101010100000000;
};

void send_LCDdata (unsigned char LCDdata) {	//wys�anie bajtu do LCD
	uint16_t i = 0;
	check_busy_flag();		// sprawdzenie flagi zaj�to�ci
	LCD_WR_DATA;			// ustawienie sygna��w steruj�cych dla rozpocz�cia zapisu danej
							// po 40us (minimalny dopuszczalny czas)
	 for(i = 0; i < 15; i++)
		 asm("nop");
	LCD_EN_SET;				// ustawienie linii EN
	LCD_DATA_OUT = (LCDdata << 4);	//przes�anie bajtu danych do LCD_DATA_OUT
	 for(i = 0; i < 40; i++)
		 asm("nop");
	LCD_EN_CLEAR;			// zerowanie linii EN
	HAL_Delay(1);
};

void send_LCDinstr (unsigned char LCDinstr) {	//wys�anie bajtu do LCD
	uint16_t i = 0;
	check_busy_flag();		// sprawdzenie flagi zaj�to�ci
	LCD_WR_INST;			// ustawienie sygna��w steruj�cych dla rozpocz�cia zapisu instrukcji
							// po 40us (minimalny dopuszczalny czas)
	 for(i = 0; i < 15; i++)
		 asm("nop");
	LCD_EN_SET;				// ustawienie linii EN
	LCD_DATA_OUT = (LCDinstr << 4);//przes�anie instrukcji do LCD_DATA_OUT
	 for(i = 0; i < 40; i++)
		 asm("nop");
	LCD_EN_CLEAR;			// zerowanie linii EN
	HAL_Delay(1);
}

void send_LCDinstr_without_check_flag (unsigned char LCDinstr) {	//wyslanie bajtu do LCD
	uint16_t i = 0;
	LCD_WR_INST;						// ustawienie sygna��w steruj�cych dla rozpocz�cia zapisu instrukcji
	 for(i = 0; i < 15; i++)
		 asm("nop");
	LCD_EN_SET;							// ustawienie linii EN
	LCD_DATA_OUT = (LCDinstr << 4);		//przes�anie instrukcji do LCD_DATA_OUT
	 for(i = 0; i < 40; i++)
		 asm("nop");
	LCD_EN_CLEAR;						// zerowanie linii EN
	HAL_Delay(1);
};

void init_LCD(void)
{
	//inicjalizacja wy�wietlacza LCD
	//maksymalna warto�� op�nienia generowanego przez funkcj� _delay_ms wynosi 6.5535s z rozdzielczo�ci� 0.1 ms, a do czasu (2^16-1)*4/fclk[kHz]=16.38375 ms z rozdzielczo�ci� 2.5e-4 ms
		HAL_Delay(15); 	//odmierzenie 15ms po w��czeniu zasilania
		send_LCDinstr_without_check_flag(LCD_8bit_data); //ustawia 8 bitow� magistral� danych
		HAL_Delay(5); 	//odmierzenie 5 ms po pierwszym wywo�aniu instrukcji LCD_8bit_data
	 	send_LCDinstr_without_check_flag(LCD_8bit_data); //ustawia 8 bitow� magistral� danych
	 	HAL_Delay(1);
		send_LCDinstr_without_check_flag(LCD_8bit_data); //ustawia 8 bitow� magistral� danych
	 	HAL_Delay(1);
		send_LCDinstr(LCD_8bit_data); //ustawia 8 bitow� magistral� danych
		send_LCDinstr(LCD_OFF);	   
		send_LCDinstr(LCD_CLEAR); 
		send_LCDinstr(LCD_MODE);
		send_LCDinstr(LCD_ON);	   
	};

unsigned char address_DDRAM (unsigned char x, unsigned char y){
unsigned char xy;
switch(x)
	{
			case 1: 	xy = y - 1; 		break;
			case 2:		xy = 0x40 + y - 1;	break;
			case 3:		xy = 0x14 + y - 1;	break;
			case 4:		xy = 0x54 + y - 1;	break;
			default:	xy = y - 1;
	};
return xy;
};
void set_cursor (unsigned char x, unsigned char y)
{
	send_LCDinstr(address_DDRAM(x, y)|0x80);
}

void write_string_xy(unsigned char x, unsigned char y, char *str)	//pisz tekst na LCD wskazywany wska�nikiem *text
{
	set_cursor(x, y);
	while (*str) { //sprawdzenie czy nie odczytano ostatniego znaku z wprowdzanego ci�gu znak�w
		send_LCDdata(*str++);  // umie�� wskazany znak tekstu na LCD
	};
};

void send_gcram	(unsigned char v0, unsigned char v1, unsigned char v2, unsigned char v3,
	 			unsigned char v4, unsigned char v5, unsigned char v6, unsigned char v7)
{
		 send_LCDdata(v0);
		 send_LCDdata(v1);
		 send_LCDdata(v2);
		 send_LCDdata(v3);
		 send_LCDdata(v4);
		 send_LCDdata(v5);
		 send_LCDdata(v6);
		 send_LCDdata(v7);
};
void define_char(void) { 		// Funkcja definiuje elementy budujace wisielca
	unsigned char i;
	for (i = 0; i < 8; i++)
	{

		send_LCDinstr (i * 8 + 0x40);	  //Ustawianie adresu dla pami�ci CGRAM
		switch(i)
		{
			case 0:
			{
				send_gcram(0, 0, 0, 0, 16, 12, 6, 1);  //podstawa szubienicy w dol
				break;
			};
			case 1:
			{
				send_gcram(1, 6, 12, 16, 0, 0, 0, 0);  //podstawa szubienicy w gore
				break;
			};
			case 2:
			{
				send_gcram(0, 0, 0, 0, 0, 0, 0, 31);  //__
				break;
			};
			case 3:
			{
				send_gcram(16, 16, 16, 16, 16, 16, 16, 16);  // |
				break;
			};
			case 4:
			{
				send_gcram(0, 0, 0, 0, 0, 14, 27, 17);  //gorna polowa glowy
				break;
			};
			case 5:
			{
				send_gcram(17, 27, 14, 0, 0, 0, 0, 0);  //dolna polowa glowy
				break;
			};
			case 6:
			{
				send_gcram(31, 0, 0, 0, 0, 0, 0, 0);  //_ u g�ry
				break;
			};
			case 7:
			{
				send_gcram(14, 17, 17, 17, 31, 17, 17, 2);
				break;
			};
		};
	};
};

void window_1(void) {
	send_LCDinstr(LCD_CLEAR); 					//czysc wyswietlacz LCD
	write_string_xy(1, 1, "ILE ZNAKOW MA MIEC");
	write_string_xy(2, 5, "HASLO?");
};
