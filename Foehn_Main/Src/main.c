/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "i2c.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stm32f4xx_it.h"
#include "main.h"
#include <stdio.h>
#include <string.h>
#include "firFilter.h"
#include "MPU6050.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
//volatile uint32_t ms_tick;
bool tick10ms = 0;
bool tick1500ms = 0;
//bool tick50ms = 0;

bool foehnzustand = 0;		// von Tasterinterrupt benötigt
uint8_t spannung = 0;			// wird von ADC Interrupt benötigt
bool abstandOK = 1;	

//double lage[5];						// Variable zum debuggen benötigt
uint8_t mittlererAbstand; // Variable zum debuggen benötigt
double mittlererWinkelY;  // Variable zum debuggen benötigt
double mittlererWinkelX;  // Variable zum debuggen benötigt

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

void foehnabschalten(void);
void foehnStarten(void);
void stickyRelais(void);		// Relais bleibt kleben	
void abstandabfragen(void);	
bool abstandInOrdnung(uint8_t *);

void initialisierung(void);		

void lED_Ansteuerung(void);					// setze AMPEL LED
void lagefunktion(void);						// Funktion beinhaltet alle Funktionen, um die Lage abzufragen
bool abstandsfunktion(uint8_t *);		// Funktion beinhaltet alle Funktionen, um den Abstand abzufragen
void zustandsabfrage(void);					// Funktion beinhaltet alle Funktionen, die regelmäßig abgefragt werden

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
	
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */
	
  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */
__disable_irq(); // Tastendruck noch sperren, bis initialisiert wurde
  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART2_UART_Init();
  MX_TIM11_Init();
  MX_ADC1_Init();
  MX_TIM2_Init();
  MX_TIM13_Init();
  MX_I2C3_Init();
  /* USER CODE BEGIN 2 */

	initialisierung();
__enable_irq(); // Tastendruck ab jetzt erlauben
	
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
		if(tick10ms == true) 		// Abfrage Flag ist gestetzt
		{
			tick10ms = false;				// Setze AbfrageFlag zurück, Fuehre genau 1 mal aus	
			zustandsabfrage();
		}
		
		if(tick1500ms == true)
		{
			uint8_t ausgabebuffer[30];	// Speicher anlegen
			tick1500ms = false;
//			sprintf((char*)ausgabebuffer, "Abstand: %d \r\n", mittlererAbstand);
//			HAL_UART_Transmit_IT(&huart2, ausgabebuffer, strlen((char*)ausgabebuffer));
//			
		
			//sprintf((char*)ausgabebuffer, "X-Beschleunigung: %.2f \t Y-Beschleunigung: %.2f \t Z-Beschleunigung: %.2f \n\r X-Winkel: %.2f \t Y-Winkel: %.2f\r\n", [0], lage[1], lage[2], mittlererWinkelX, mittlererWinkelY);

			sprintf((char*)ausgabebuffer, "X-Winkel: %.2f \t Y-Winkel: %.2f\r\n", mittlererWinkelX, mittlererWinkelY);
			HAL_UART_Transmit_IT(&huart2, ausgabebuffer, strlen((char*)ausgabebuffer));
		}	
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
	__NOP();
}

void initialisierung(void)
{
	TIM11->SR = 0; // UIF Flag im SR Register wird gelöscht;Sonst vorzeitiger Timerinterrupt
	TIM13->SR = 0;
	//HAL_Delay(1);
	InitMPU6050();		// Abbruchbedingung, falls MPU nicht erkannt wird
	stickyRelais(); // Falls Relais kleben bleibt, sofort wieder aus machen
}

void zustandsabfrage()
{
	abstandsfunktion(&spannung);	// Abstand abfragen
	lED_Ansteuerung();						// setze AMPEL LED
	lagefunktion();								// Lage abfragen
}

void lED_Ansteuerung()
{
	if(abstandOK == 1 && foehnzustand == 0)															// Ausgangslage Gruene LED leuchtet
	{
		HAL_GPIO_WritePin(LED_R_GPIO_Port, LED_R_Pin, GPIO_PIN_RESET);		//Rote LED
		HAL_GPIO_WritePin(LED_G_GPIO_Port, LED_G_Pin, GPIO_PIN_SET); 			//Gruene LED
		HAL_GPIO_WritePin(LED_Y_GPIO_Port, LED_Y_Pin, GPIO_PIN_RESET); 		//GELBE LED
	}
	else if(abstandOK == 0 && foehnzustand == 0)												// Abstand zu gering Rote LED
	{
		HAL_GPIO_WritePin(LED_R_GPIO_Port, LED_R_Pin, GPIO_PIN_SET);			//Rote LED
		HAL_GPIO_WritePin(LED_G_GPIO_Port, LED_G_Pin, GPIO_PIN_RESET); 		//Gruene LED
		HAL_GPIO_WritePin(LED_Y_GPIO_Port, LED_Y_Pin, GPIO_PIN_RESET); 		//GELBE LED
	}
	else if(abstandOK == 1 && foehnzustand == 1)												// Foehn an und Abstand OK
	{
		HAL_GPIO_WritePin(LED_R_GPIO_Port, LED_R_Pin, GPIO_PIN_RESET); 		//Rote LED
		HAL_GPIO_WritePin(LED_G_GPIO_Port, LED_G_Pin, GPIO_PIN_RESET); 		//Gruene LED
		HAL_GPIO_WritePin(LED_Y_GPIO_Port, LED_Y_Pin, GPIO_PIN_SET);			//Gelbe LED
	}
	else if(abstandOK == 0 && foehnzustand == 1)												// Foehn an und Abstand zu gering
	{
		HAL_GPIO_WritePin(LED_R_GPIO_Port, LED_R_Pin, GPIO_PIN_SET); 			//Rote LED
		HAL_GPIO_WritePin(LED_G_GPIO_Port, LED_G_Pin, GPIO_PIN_RESET); 		//Gruene LED
		HAL_GPIO_WritePin(LED_Y_GPIO_Port, LED_Y_Pin, GPIO_PIN_RESET);		//Gelbe LED
		foehnabschalten();																								// Foehn korrekt ausschalten
	}
}

void lagefunktion(void) 																							// Funktion beinhaltet alle Funktionen, um die Lage abzufragen
{
	// Anlegen und initialisieren von Speicher
	static FIRFilter lagemessungenX = {{0}, 0, 0};
	static FIRFilter lagemessungenY = {{0}, 0, 0};
	static double lage[5];	
	// Variable zum debuggen benötigt
	// Lageberechnung
	foehnlageberechnung(lage);
	mittlererWinkelX = FIRFilter_Update(&lagemessungenX, lage[3]);
	mittlererWinkelY = FIRFilter_Update(&lagemessungenY, lage[4]);

	if (foehnWinkelBerechnung(mittlererWinkelX, mittlererWinkelY) == 1) // Föhn steht schief
	{
		HAL_GPIO_WritePin(LED_R_GPIO_Port, LED_R_Pin, GPIO_PIN_SET); 			//Rote LED
		HAL_GPIO_WritePin(LED_G_GPIO_Port, LED_G_Pin, GPIO_PIN_SET); 			//Gruene LED
		HAL_GPIO_WritePin(LED_Y_GPIO_Port, LED_Y_Pin, GPIO_PIN_SET);			//Gelbe LED
		Error_Handler();
	}
}

bool abstandsfunktion(uint8_t *spannung)															// Funktion beinhaltet alle Funktionen, um den Abstand abzufragen
{	
	static FIRFilter abstand = {{0}, 0, 0};															//Anlegen von Speicher
	abstandsMessung(spannung);																					// Abstand abfragen
	mittlererAbstand = FIRFilter_Update(&abstand, (double)*spannung);		// Abstand filtern, Mittleren Abstand berechnen
	return abstandOK = abstandInOrdnung(&mittlererAbstand);							// Auswerten, ob der Abstand groß genung ist
}

void stickyRelais(void)
{
	bool aktuellerZustand = HAL_GPIO_ReadPin(Relais_Spannung_GPIO_Port, Relais_Spannung_Pin); //Relais Zustand abfragen
	
//	if (foehnzustand == 0)														// PWM von Servo ausschalten
//	{
//			//HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_2);
//	}
	if(aktuellerZustand == 0 || aktuellerZustand == foehnzustand)					// Relais-Spannung ist 0, oder der Föhn soll an sein
	{
		return;
	}
	else																																	// kritischer Fehler, Relay bleibt kleben, Föhn komplett abschalten
	{
		const uint8_t ausgabebuffer[] = "Föhn ist noch an";
		HAL_UART_Transmit(&huart2, (uint8_t *)ausgabebuffer, strlen((char*)ausgabebuffer), 5);
		Error_Handler(); 																										// NOTAUS
	}
}

bool abstandInOrdnung(uint8_t *abstand)
{		
	if (*abstand > 175)
	{ // Abstand <10 cm
		return false;
	}
	else
		// Abstand > 10cm
	{
		return true;
	}	
}

void foehnStarten() 						// nur ausgefuehrt wenn Taster tatsächlich gedruckt wurde
{	
	if(abstandOK == 1)						// Abstand groß genung -> Starte den Föhn
	{
	HAL_NVIC_DisableIRQ(EXTI0_IRQn);		// Taster Interrupt ausstellen
	HAL_TIM_Base_Stop_IT(&htim13);			// Schalte Timer für Sticky Relais aus
//				while (HAL_NVIC_GetPendingIRQ(TIM8_UP_TIM13_IRQn))		// Sticky Relais ausmachen
//		{		
//			HAL_NVIC_ClearPendingIRQ(TIM8_UP_TIM13_IRQn);
//		}	
	// Tim 11	Reinitialisieren
	TIM11->CNT = 0;
	HAL_TIM_Base_Start_IT(&htim11);		// starte Timer
	foehnzustand = 1;
	HAL_GPIO_WritePin(K3_Foehn_GPIO_Port, K3_Foehn_Pin, GPIO_PIN_RESET); // Foehn anschalten
	//HAL_Delay(1);
		//HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2); 		//Servobetaetigen
	}
}

void foehnabschalten()							// Zeit vorbei, Abstand wurde zu gering -> Schalte den Föhn ab
{
		HAL_GPIO_WritePin(K3_Foehn_GPIO_Port, K3_Foehn_Pin, GPIO_PIN_SET); // foehn abschalten
	//	HAL_Delay(1);
		HAL_TIM_Base_Stop_IT(&htim11);								// Schalte Timer aus
		foehnzustand = 0;
		// starte Timer 13, um Sticky Relay abzufragen
		TIM13->CNT = 0;
		HAL_TIM_Base_Start_IT(&htim13);

			while (HAL_NVIC_GetPendingIRQ(EXTI0_IRQn))		// Taster Interrupt starten, Pending Bit = 0 setzen
		{
		__HAL_GPIO_EXTI_CLEAR_IT(EXTI_PIN_MASK);					
			HAL_NVIC_ClearPendingIRQ(EXTI0_IRQn);			// falls Taster waehrend foehnen betaetigt wurde, Pending Bit loeschen
		}				
		HAL_NVIC_EnableIRQ(EXTI0_IRQn);								// Mach Interrupt für Tasterdruck wieder an
}


void 	HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) 		//Aufruf, wenn Periode von Timer vorbei ist
{
	if(htim == &htim11) // Foehndauer vorbei
	{
		foehnabschalten();
	}
	if(htim == &htim13) // Dauer nach Foehnen vorbei -> Relais abfragen
	{
		HAL_TIM_Base_Stop_IT(&htim13);								// Schalte Timer aus
		stickyRelais();
	}	
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)		// Aufruf bei Tasterbetätigung
{
	HAL_Delay(1);
	// Taster betätigung länger als 1 ms(keine Fehltriggerung)
	if(	HAL_GPIO_ReadPin(Taster_GPIO_Port, Taster_Pin) == 0)
	{
		return;
	}
	else
	{
	foehnStarten();
	}
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
	spannung = HAL_ADC_GetValue(&hadc1);
	HAL_ADC_Stop_IT(&hadc1);
}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
	
	__disable_irq();
		HAL_GPIO_WritePin(K2_Haupt_GPIO_Port, K2_Haupt_Pin, GPIO_PIN_RESET); //betätige Hauptschalter 
		HAL_GPIO_WritePin(K3_Foehn_GPIO_Port, K3_Foehn_Pin, GPIO_PIN_SET); // foehn abschalten
		HAL_GPIO_WritePin(K4_Selbst_GPIO_Port, K4_Selbst_Pin, GPIO_PIN_RESET); // Selbsthaltung abschalten
		
		HAL_GPIO_WritePin(LED_R_GPIO_Port, LED_R_Pin, GPIO_PIN_SET); 		//Rote LED
		HAL_GPIO_WritePin(LED_G_GPIO_Port, LED_G_Pin, GPIO_PIN_SET); 	//Gruene LED
		HAL_GPIO_WritePin(LED_Y_GPIO_Port, LED_Y_Pin, GPIO_PIN_SET);		//Gelbe LED
	
  while (1)
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
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
