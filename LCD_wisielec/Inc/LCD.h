#include "stm32f4xx.h"

#define LCD_MODE     		0x0006		// I/D=1 - inkrementacja, S=0 - brak przesuniêcia zawartoœci DDRAM
#define LCD_CLEAR    		0x0001		// czyœci wyœwietlacz i ustawia kursor na pocz¹tku (adres=0)
#define LCD_ON		 		0x000c		// D=1 - w³¹czenie wyœwietlacza, C=0 - wy³¹czenie kursora, B=0 - wy³¹czenie migania kursora
#define LCD_OFF		 		0x0008		// D=0 - wy³¹czenie wyœwietlacza, C=0 - wy³¹czenie kursora, B=0 - wy³¹czenie migania kursora
#define LCD_8bit_data 		0x0038		// DL=1 - 8 linii danych, N=1 - dwu liniowy, F=0 - matryca 5x7 punktów
#define LCD_DISP_SHIFT		0x0018		// Przesuwanie ekranu o jedno pole
#define _BV(bit) (1<<bit)
//przyporz¹dkowanie sygna³om wyœwietlacza LCD odpowiednich portów mikrokontrolera
#define	LCD_DATA_OUT	GPIOE->ODR

// ustawienie sygna³ów sterujacych dla rozpoczêcia zapisu instrukcji
#define LCD_WR_INST	GPIOD->BSRR=0b00000001100000000000000000000000
// ustawienie sygna³ów sterujacych dla rozpoczêcia zapisu danej
#define LCD_WR_DATA	GPIOD->BSRR=0b00000001000000000000000010000000
// ustawienie sygna³ów sterujacych dla rozpoczêcia odczytu flagi zajêtoœci
#define LCD_RD_FLAG	GPIOD->BSRR=0b00000000100000000000000100000000
// ustawienie sygna³ów sterujacych dla rozpoczêcia odczytu danej
#define LCD_RD_DATA	GPIOD->BSRR=0b00000000000000000000000110000000
// ustawienie sygna³u sterujacego EN
#define LCD_EN_SET	GPIOD->BSRR=0b00000000000000000000001000000000
// wyzerowanie sygna³u sterujacego EN
#define LCD_EN_CLEAR	GPIOD->BSRR = 0b00000010000000000000000000000000
// Ustawienie sygnalu sterujacego dla przesuwania napisu w lewo



void check_busy_flag (void);
void send_LCDdata (unsigned char LCDdata);
void send_LCDinstr (unsigned char LCDinstr) ;
void send_LCDinstr_without_check_flag (unsigned char LCDinstr);
void init_LCD(void);
unsigned char address_DDRAM (unsigned char x, unsigned char y);
void set_cursor (unsigned char x, unsigned char y);
void write_string_xy(unsigned char x, unsigned char y, char *str);
void window_1(void);
void send_gcram	(unsigned char v0,unsigned char v1,unsigned char v2,unsigned char v3,
	 			unsigned char v4,unsigned char v5,unsigned char v6,unsigned char v7);
void define_char(void);
