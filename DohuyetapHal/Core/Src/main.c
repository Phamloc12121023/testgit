#include "main.h"
#include "gp_drive.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
ADC_HandleTypeDef hadc1;
TIM_HandleTypeDef htim2;
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_ADC1_Init(void);
static void MX_TIM2_Init(void);
int readADC_100(void);
uint8_t modein=1;
int flag=0;
int mmHg = 0, adc;
long map(long x, long in_min, long in_max, long out_min, long out_max);
void mode(uint8_t check1);
volatile uint16_t CheckmmHg[200];
volatile uint16_t Checkadc[200];
volatile int16_t Checkadc2[50],maxadc=0,minadc=0;
volatile uint16_t indexx[50],min=0,max=0;
int i=0,y=0;
int solanheartbeat=0;
double heartbeat2=0,heartbeat=0;
uint16_t systolic,diastole;
int calculate=0;
uint16_t systolic,diastole;
/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  HAL_Init();
  SystemClock_Config();
  MX_GPIO_Init();
  MX_ADC1_Init();
  MX_TIM2_Init();
	HAL_TIM_Base_Start_IT(&htim2);
	Digital_Output(PA,5);
	Digital_Output(PB,0);
	Digital_Output(PB,1);
	Digital_Output(PA,7);
  while (1)
  {
		adc = readADC_100();
		mmHg=map(adc,494,1980,0,260);
		if(mmHg>=200)
			modein=3;   
		if(mmHg<50 && modein==3)
		{
			modein=1;
			calculate=1;
		}
		mode(modein); 
		if(calculate==1)
		{
			int flag=0;
			for(int j=0;j<=y;j++)
			{
				if(flag==0 && Checkadc2[j]!=0)
				{
					flag=1;
					max=indexx[0];
					min=indexx[0];
					maxadc=Checkadc2[0];
					minadc=Checkadc2[0];
				}
				if(maxadc<Checkadc2[j] && Checkadc2[j]!=0 && Checkadc2[j]!=-1 && j>1 )
				{
					maxadc=Checkadc2[j];
					max=indexx[j];
				}
					
				if(minadc>Checkadc2[j] && Checkadc2[j]!=0 && Checkadc2[j]!=-1 && j>1)
				{
					minadc=Checkadc2[j];
					min=indexx[j];
				}
				heartbeat=heartbeat+indexx[j];
					
			}
			heartbeat2=round(60/((heartbeat/solanheartbeat)/100));
			systolic=CheckmmHg[min];
			diastole=CheckmmHg[max];
			calculate=0;
		}
	}
  /* USER CODE END 3 */
}

void mode(uint8_t check1)
{
	static uint8_t check2 = 0;
	if(check1 != check2)
	{
		if(check1==1)
		{
			HAL_GPIO_WritePin(GPIOB,GPIO_PIN_0,GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOB,GPIO_PIN_1,GPIO_PIN_RESET);
		}
		else if(check1==2 )
		{
			HAL_GPIO_WritePin(GPIOB,GPIO_PIN_0,GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOB,GPIO_PIN_1,GPIO_PIN_SET);
		}
		else if(check1==3)
		{
			HAL_GPIO_WritePin(GPIOB,GPIO_PIN_0,GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOB,GPIO_PIN_1,GPIO_PIN_RESET);
		}
	}
	check2 =check1;
}

long map(long x, long in_min, long in_max, long out_min, long out_max)
{	
	return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

int readADC_100(void)
{
	int x=0;
	for(int i=0;i<100;i++)
	{
		HAL_ADC_Start(&hadc1);
		x = x + HAL_ADC_GetValue(&hadc1);
	}
	return x=x/100;
}
		
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim->Instance == htim2.Instance)
	{
		if(modein==3 && 200>i)
		{
			CheckmmHg[i]=mmHg;
			Checkadc[i]=adc;
			if(Checkadc[(i-1)]<Checkadc[i] && i>=2)
			{
				Checkadc2[y]=Checkadc[i-1]-Checkadc[i];
				indexx[y]=(i-1);
				solanheartbeat++;
				y++;
			}
			i++;
		}
		if(i>199)
		{
			i=200;
		}
	}
}
/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI_DIV2;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL12;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
  PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV4;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief ADC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC1_Init(void)
{

  /* USER CODE BEGIN ADC1_Init 0 */

  /* USER CODE END ADC1_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */

  /** Common config
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE;
  hadc1.Init.ContinuousConvMode = DISABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 1;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_0;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_1CYCLE_5;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */

  /* USER CODE END ADC1_Init 2 */

}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 480;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 9999;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

}

/* USER CODE BEGIN 4 */

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

