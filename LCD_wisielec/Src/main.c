
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  ** This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * COPYRIGHT(c) 2018 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f4xx_hal.h"

/* USER CODE BEGIN Includes */
#include "LCD.h"

/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/
TIM_HandleTypeDef htim10;

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/

volatile unsigned char kl_ENTER = 0;	//przycisk ENTER (KEY0)
volatile unsigned char kl_UP = 0;		//przycisk UP (KEY1)
volatile unsigned char kl_DOWN = 0;		//przycisk DOWN (KEY2)
volatile unsigned char kl_CANCEL = 0;	//przycisk CANCELv(KEY3)
volatile int licznik = 0; 	//zmienna regulujaca czestotliwosc zapalania sie i wylaczania modyfikowanej diody

union {		//do obs³ugi przycisków -> w przerwaniu ISR
 struct {
  unsigned char ENTER:1;	//LSB
  unsigned char UP:1;
  unsigned char DOWN:1;
  unsigned char CANCEL:1;
  unsigned char ENTER_PREV:1;
  unsigned char UP_PREV:1;
  unsigned char DOWN_PREV:1;
  unsigned char CANCEL_PREV:1; //MSB
 } flags;
  char byte;
} key;

union {		//do obs³ugi przycisków -> w przerwaniu ISR
 struct {
  unsigned char ENTER:1;	   //LSB
  unsigned char UP:1;
  unsigned char DOWN:1;
  unsigned char CANCEL:1;
  unsigned char ENTER_PREV:1;
  unsigned char UP_PREV:1;
  unsigned char DOWN_PREV:1;
  unsigned char CANCEL_PREV:1;
  unsigned char ENTER_PREV1:1;
  unsigned char UP_PREV1:1;
  unsigned char DOWN_PREV1:1;
  unsigned char CANCEL_PREV1:1;
  unsigned char ENTER_PREV2:1;
  unsigned char UP_PREV2:1;
  unsigned char DOWN_PREV2:1;
  unsigned char CANCEL_PREV2:1; //MSB
  } flags;
  int word;
} falling_slope;

signed char cyfry[6] = {0, 0, 0, 0, 0, 0};	//wartoœci tablicy modyfikowane s¹ przyciskami
volatile signed char kopia[6] = {0, 0, 0, 0, 0, 0};   //kopia wartosci, uzyta pozniej w przyadku przycisku CANCEL
volatile signed char enter = 0;		//zmienna przechowujaca aktualna ilosc klikniec przycisku ENTER % 5
volatile unsigned char miganie = 0;	//zmienna bezposrednio wskazujaca na zapalenie lub nie modyfikowanego znaku

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIM10_Init(void);

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/

/* USER CODE END PFP */

/* USER CODE BEGIN 0 */



/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  *
  * @retval None
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
	int znaki_hasla = 1;
	char flaga = 0;
	char haslo[10] = {0};
	char kopia_hasla[10] = {0};
	signed char znak = 0;
	char pozycja = 1;
	int numer_znaku = 0;
	unsigned char zle = 0;
	int i = 0;
	int sprawdzone_znaki = 1;
	char znaleziony_znak = 0;
	key.byte = 0xFF;   				//inicjalizacja flag stanami nieaktywnymi
	falling_slope.word = 0x0000; 	//inicjalizacja flag stanami nieaktywnymi
  /* USER CODE END 1 */

  /* MCU Configuration----------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_TIM10_Init();
  /* USER CODE BEGIN 2 */
  HAL_TIM_Base_Start_IT(&htim10);
  init_LCD();
  define_char();

  window_1();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {

  /* USER CODE END WHILE */

  /* USER CODE BEGIN 3 */

		if(flaga == 0)		// flaga kontroluje aktualnie wyswietlane okno na wyswietlaczu
		{			// wprowadzenie iloœci liter w haœle
			set_cursor(2, 15);
			 if(miganie == 1)
			 {
				if(znaki_hasla != 10)
					 send_LCDdata(znaki_hasla + 48);
				if(znaki_hasla == 10)
				{
					send_LCDdata(49);
					send_LCDdata(48);
				}
			 }
			 else
			 {
				if(znaki_hasla != 10)
					send_LCDdata(32);
				if(znaki_hasla == 10)
				{
					send_LCDdata(32);
					send_LCDdata(32);
				};
			 }
			if(kl_UP == 1){
				znaki_hasla++;
				kl_UP = 0;
			};
			if(kl_DOWN == 1){
				znaki_hasla--;
				kl_DOWN = 0;
			};
			if(znaki_hasla <= 0)
				znaki_hasla = 1;
			if(kl_ENTER == 1)		// zatwierdzenie liczby liter w haœle
			{
				set_cursor(2, 15);
				send_LCDdata(znaki_hasla+48);
				flaga = 1;
				kl_ENTER = 0;
			};
			if(kl_CANCEL == 1)
				kl_CANCEL = 0;
		};
		if (flaga == 1)			// rozpoczêcie odgadywania has³a
		{
			write_string_xy(3, 1, "WPROWADZ HASLO:");
			set_cursor(4, (pozycja));
			if(kl_UP == 1){
				znak++;
				kl_UP = 0;
			};
			if(kl_DOWN == 1){
				znak--;
				kl_DOWN = 0;
			};
			if(znak < 0)
				znak = 25;
			if(znak >= 26)
				znak = 0;
			 if(miganie == 1)
				send_LCDdata(znak + 97);
			 else
				send_LCDdata(32);
			if(kl_ENTER == 1)
			{
				set_cursor(4, (pozycja));
				send_LCDdata(znak + 97);
				haslo[numer_znaku] = znak + 97;
				kopia_hasla[numer_znaku] = znak + 97;
				numer_znaku++;
				pozycja++;
				znak = 0;
				kl_ENTER = 0;
			};
			if(kl_CANCEL == 1)
			{
				set_cursor(4,(pozycja));
				send_LCDdata(32);
				haslo[numer_znaku] = 0;
				kopia_hasla[numer_znaku] = 0;
				numer_znaku--;
				pozycja--;
				znak = haslo[numer_znaku] - 97;
				kl_CANCEL = 0;
			};
			if(numer_znaku < 0)
				numer_znaku = 0;
			if(pozycja<1)
				pozycja = 1;
			if((pozycja - 1) >= znaki_hasla)
			{
				send_LCDinstr(LCD_CLEAR);
				flaga++;
				znak = 0;
				pozycja = 1;
				numer_znaku = 0;
				write_string_xy(3, 1, "LITERA:");
				set_cursor(4, 1);
				send_LCDdata(haslo[0]);
				for(i = 1; i < znaki_hasla; i++)
				{
					send_LCDdata(2);
				};
			};
		};
		if(flaga ==2 )
		{
			set_cursor(3, 8);
			if(kl_UP == 1){
				znak++;
				kl_UP = 0;
			};
			if(kl_DOWN == 1){
				znak--;
				kl_DOWN = 0;
			};
			if(znak < 0)
				znak = 25;
			if(znak >= 26)
				znak = 0;
			 if(miganie == 1)
				send_LCDdata(znak + 97);
			 else
				send_LCDdata(32);
			if(kl_ENTER == 1)
			{
				for (i = 1; i < znaki_hasla; i++)
				{
					if((znak + 97) == haslo[i])
					{
						set_cursor(4, i + 1);
						send_LCDdata(znak + 97);
						sprawdzone_znaki++;
						znaleziony_znak = 1;
						haslo[i] = 0;
					};
				};
				if(sprawdzone_znaki == (znaki_hasla))
				{
					znak = 0;
					zle = 0;
					flaga = 4;
					send_LCDinstr(LCD_CLEAR);
					write_string_xy(1, 3, "HASLO POPRAWNE");
					write_string_xy(2, 5, "GRATULACJE!");
					write_string_xy(3, 5, "NOWE HASLO:");
					write_string_xy(4, 7, "[ENTER]");
					sprawdzone_znaki = 1;
				};
					if(znaleziony_znak == 0)
					{
						if(zle == 0)
						{
							set_cursor(1, 1);
							send_LCDdata(0);
						};
						if(zle == 1)
						{
							set_cursor(2, 1);
							send_LCDdata(1);
						};
						if((zle > 1) && (zle < 20))
						{
							set_cursor(1, zle);
							send_LCDdata(2);
						};
						if((zle >= 21) && (zle < 23))
						{
							set_cursor((zle - 19), 20);
							send_LCDdata(3);
						};
						if((zle >= 23) && (zle < 27))
						{
							set_cursor(3, (42 - zle));
							send_LCDdata(2);
						};
						if(zle == 27)
						{
							set_cursor(3, 15);
							send_LCDdata(4);
							set_cursor(4, 15);
							send_LCDdata(5);
						};
						if(zle == 28)
						{
							set_cursor(3, 14);
							send_LCDdata(2);
							set_cursor(4, 14);
							send_LCDdata(6);
						};
						if(zle == 29)
						{
							set_cursor(3, 13);
							send_LCDdata(0);
						};
						if(zle == 30)
						{
							set_cursor(4, 13);
							send_LCDdata(1);
						};
						if(zle == 31)
						{
							set_cursor(3, 12);
							send_LCDdata(2);
							set_cursor(4, 12);
							send_LCDdata(6);
						};
						if(zle == 32)
						{
							set_cursor(3, 11);
							send_LCDdata(0);
						};
						if(zle == 33)
						{
							set_cursor(4, 11);
							send_LCDdata(1);
							HAL_Delay(5000);
							send_LCDinstr(LCD_CLEAR);
							write_string_xy(1, 5, "PRZEGRYWASZ");
							flaga++;
						};
						zle++;
				};
				znaleziony_znak = 0;
				kl_ENTER = 0;
			};
			if(kl_CANCEL == 1)
				kl_CANCEL = 0;
		};
		if(flaga == 3)
		{
			write_string_xy(2, 2, "NOWE");
			write_string_xy(3, 1, "HASLO:");
			write_string_xy(4, 1, "[CANCEL]");
			write_string_xy(2, 12, "KOLEJNA");
			write_string_xy(3, 13, "PROBA:");
			write_string_xy(4, 12, "[ENTER]");
			if(kl_ENTER == 1)
			{
				flaga = 2;
				send_LCDinstr(LCD_CLEAR);
				pozycja = 1;
				znak = 0;
				zle = 0;
				kl_ENTER = 0;
				sprawdzone_znaki = 1;
				for(i = 0; i < 8; i++)
					haslo[i] = kopia_hasla[i];
				write_string_xy(3, 1, "LITERA:");
				set_cursor(4, 1);
				send_LCDdata(haslo[0]);
				for(i = 1; i < znaki_hasla; i++)
					send_LCDdata(2);
			};
			if(kl_CANCEL == 1)
			{
				flaga = 0;
				pozycja = 4;
				znak = 0;
				zle = 0;
				send_LCDinstr(LCD_CLEAR);
				window_1();
				kl_CANCEL = 0;
			};
		};
		if (flaga == 4)
		{
			if(kl_ENTER)
			{
				send_LCDinstr(LCD_CLEAR);
				window_1();
				flaga = 0;
				kl_ENTER = 0;
			};
			if(kl_CANCEL == 1)
				kl_CANCEL = 0;
			if(kl_UP == 1)
				kl_UP = 0;
			if(kl_DOWN == 1)
				kl_DOWN = 0;
		};
  };

  /* USER CODE END 3 */

};

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;

    /**Configure the main internal regulator output voltage 
    */
  __HAL_RCC_PWR_CLK_ENABLE();

  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = 16;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Configure the Systick interrupt time 
    */
  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

    /**Configure the Systick 
    */
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

/* TIM10 init function */
static void MX_TIM10_Init(void)
{

  htim10.Instance = TIM10;
  htim10.Init.Prescaler = 255;
  htim10.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim10.Init.Period = 999;
  htim10.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  if (HAL_TIM_Base_Init(&htim10) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}

/** Configure pins as 
        * Analog 
        * Input 
        * Output
        * EVENT_OUT
        * EXTI
*/
static void MX_GPIO_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStruct;

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOE, D0_Pin|D1_Pin|D2_Pin|D3_Pin 
                          |D4_Pin|D5_Pin|D6_Pin|D7_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOD, RW_Pin|EN_Pin|RS_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : D0_Pin D1_Pin D2_Pin D3_Pin 
                           D4_Pin D5_Pin D6_Pin D7_Pin */
  GPIO_InitStruct.Pin = D0_Pin|D1_Pin|D2_Pin|D3_Pin 
                          |D4_Pin|D5_Pin|D6_Pin|D7_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /*Configure GPIO pins : KEY3_Pin KEY0_Pin KEY1_Pin KEY2_Pin */
  GPIO_InitStruct.Pin = KEY3_Pin|KEY0_Pin|KEY1_Pin|KEY2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : RW_Pin EN_Pin RS_Pin */
  GPIO_InitStruct.Pin = RW_Pin|EN_Pin|RS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim->Instance == TIM10)				// Zabezpieczenie przed wp³ywem zak³óceñ na stan wejœc z przycisków
	{
		key.byte=(key.byte << 4);// przesuwaj¹c w lew¹ stronê bity z do³u uzupe³niane s¹ zerami
		//ustawienie jedynek na pozycjach od 0 do 3 w przypadku odczytania zer z odpowiednich wejœæ KEYx
			key.byte |= ((HAL_GPIO_ReadPin(KEY0_GPIO_Port, KEY0_Pin)==GPIO_PIN_SET)|((HAL_GPIO_ReadPin(KEY1_GPIO_Port, KEY1_Pin)==GPIO_PIN_SET) << 1)|((HAL_GPIO_ReadPin(KEY2_GPIO_Port, KEY2_Pin)==GPIO_PIN_SET) << 2)|((HAL_GPIO_ReadPin(KEY3_GPIO_Port, KEY3_Pin)==GPIO_PIN_SET) << 3));
			falling_slope.word = (falling_slope.word << 4);

			if(key.flags.ENTER)			 //czy wciœniêty jest przycisk ENTER?
			{
				if(!key.flags.ENTER_PREV)
					falling_slope.flags.ENTER = 1; //Czy wykryto zbocze opadaj¹ce
				if(falling_slope.flags.ENTER_PREV2)
				{
					kl_ENTER = 1; //Czy od chwili wykrycia zbocza opadaj¹cego up³ynê³y 3 przerwania z wciœniêtym przyciskiem
					enter++;
				}
			}
			else
			{
				falling_slope.word &= 0xEEEE;  //wyzerowanie flag dla przycisku ENTER potwierdzaj¹cych wykrycie zbocza opadaj¹cego
			};


			if(key.flags.UP)
			{  //czy wciœniêty jest przycisk UP?
				if(!key.flags.UP_PREV)
					falling_slope.flags.UP = 1; //Czy wykryto zbocze opadaj¹ce
				if(falling_slope.flags.UP_PREV2)
					kl_UP = 1; //Czy od chwili wykrycia zbocza opadaj¹cego up³ynê³y 3 przerwania z wciœniêtym przyciskiem
			}
			else
			{
				falling_slope.word &= 0xDDDD;  //wyzerowanie flag dla przycisku UP potwierdzaj¹cych wykrycie zbocza opadaj¹cego
			};

			if(key.flags.DOWN)
			{  //czy wciœniêty jest przycisk DOWN?
				if(!key.flags.DOWN_PREV)
					falling_slope.flags.DOWN = 1; //Czy wykryto zbocze opadaj¹ce
				if(falling_slope.flags.DOWN_PREV2)
					kl_DOWN = 1; //Czy od chwili wykrycia zbocza opadaj¹cego up³ynê³y 3 przerwania z wciœniêtym przyciskiem
			}
			else
			{
				falling_slope.word &= 0xBBBB;  //wyzerowanie flag dla przycisku DOWN potwierdzaj¹cych wykrycie zbocza opadaj¹cego
			};

			if(key.flags.CANCEL)
			{  //czy wciœniêty jest przycisk CANCEL?
				if(!key.flags.CANCEL_PREV)
					falling_slope.flags.CANCEL = 1;  //Czy wykryto zbocze opadaj¹ce
				if(falling_slope.flags.CANCEL_PREV2)
					kl_CANCEL = 1;  //Czy od chwili wykrycia zbocza opadaj¹cego up³ynê³y 3 przerwania z wciœniêtym przyciskiem
			}
			else
			{
				falling_slope.word &= 0x7777;  //wyzerowanie flag dla przycisku CANCEL potwierdzaj¹cych wykrycie zbocza opadaj¹cego
			};
			if(licznik == 62)	//czy minê³a sekunda?
			{
				licznik = 0;		//reset licznika
				miganie = !miganie;		//zmiana stanu modyfikowanego znaku
			}

			licznik++;
	};
};
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  file: The file name as string.
  * @param  line: The line in file as a number.
  * @retval None
  */
void _Error_Handler(char *file, int line)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  while(1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
