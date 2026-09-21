#include "main.h"
ADC_HandleTypeDef hadc1;

I2C_HandleTypeDef hi2c1;

UART_HandleTypeDef huart2;

/* RTOS Objects */
QueueHandle_t LogQueue;
SemaphoreHandle_t TempMutex;
EventGroupHandle_t TempEventGroup;

TaskHandle_t SensorTaskHandle;
TaskHandle_t LCDTaskHandle;
TaskHandle_t LoggerTaskHandle;
TaskHandle_t AlarmTaskHandle;

/* Event Bits */
#define TEMP_NORMAL_BIT    (1<<0)
#define TEMP_WARNING_BIT   (1<<1)
#define TEMP_CRITICAL_BIT  (1<<2)

/* Thresholds */
#define TEMP_WARNING_LEVEL   35.0f
#define TEMP_CRITICAL_LEVEL  45.0f

typedef struct
{
    uint16_t adc;
    float voltage;
    float temperature;
}TempData_t;

void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_ADC1_Init(void);
static void MX_I2C1_Init(void);
static void MX_USART2_UART_Init(void);

/* Shared Data */
TempData_t gTempData;

/* Function Prototypes */
uint16_t Read_ADC(void);

void SensorTask(void *pvParameters);
void LCDTask(void *pvParameters);
void LoggerTask(void *pvParameters);
void AlarmTask(void *pvParameters);

int main(void)
{

  HAL_Init();
  SystemClock_Config();
  MX_GPIO_Init();
  MX_ADC1_Init();
  MX_I2C1_Init();
  MX_USART2_UART_Init();

  lcd_init();
  lcd_clear();
  /* RTOS Objects */
  	  // creating the Mutex, allow to access the resource 1 task at a time
      TempMutex = xSemaphoreCreateMutex();
      // creating the Queue for storing the data
      LogQueue = xQueueCreate(
                      10,
                      sizeof(TempData_t));
      // creating the event group to represent the 3-levels in single byte
      TempEventGroup =
              xEventGroupCreate();

      /* Tasks */

          xTaskCreate(
                  SensorTask,
                  "Sensor",
                  256,
                  NULL,
                  3,
                  &SensorTaskHandle);

          xTaskCreate(
                  LCDTask,
                  "LCD",
                  256,
                  NULL,
                  2,
                  &LCDTaskHandle);

          xTaskCreate(
                  LoggerTask,
                  "Logger",
                  256,
                  NULL,
                  1,
                  &LoggerTaskHandle);

          xTaskCreate(
                  AlarmTask,
                  "Alarm",
                  256,
                  NULL,
                  4,
                  &AlarmTaskHandle);

          vTaskStartScheduler();

          while(1);
}



void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};


  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);


  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }


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

static void MX_ADC1_Init(void)
{


  ADC_ChannelConfTypeDef sConfig = {0};


  hadc1.Instance = ADC1;
  hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV2;
  hadc1.Init.Resolution = ADC_RESOLUTION_12B;
  hadc1.Init.ScanConvMode = DISABLE;
  hadc1.Init.ContinuousConvMode = DISABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 1;
  hadc1.Init.DMAContinuousRequests = DISABLE;
  hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }

  sConfig.Channel = ADC_CHANNEL_0;
  sConfig.Rank = 1;
  sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

}


static void MX_I2C1_Init(void)
{


  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 100000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }


}


static void MX_USART2_UART_Init(void)
{


  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }


}


static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5|GPIO_PIN_10, GPIO_PIN_RESET);

  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_RESET);

  GPIO_InitStruct.Pin = GPIO_PIN_5|GPIO_PIN_10;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = GPIO_PIN_5;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);


}


void SensorTask(void *pvParameters)
{
    TempData_t data;

    while(1)
    {
        data.adc = Read_ADC();

        data.voltage =
                ((float)data.adc * 3.3f)
                / 4095.0f;

        data.temperature =
                data.voltage * 100.0f;

        xSemaphoreTake(
                TempMutex,
                portMAX_DELAY);

        gTempData = data;   // shared resource updating

        xSemaphoreGive(
                TempMutex);

        xQueueSend(
                LogQueue,
                &data,
                0);

        if(data.temperature <
                TEMP_WARNING_LEVEL)
        {
            xEventGroupClearBits(
                    TempEventGroup,
                    TEMP_WARNING_BIT |
                    TEMP_CRITICAL_BIT);

            xEventGroupSetBits(
                    TempEventGroup,
                    TEMP_NORMAL_BIT);
        }
        else if(data.temperature <
                TEMP_CRITICAL_LEVEL)
        {
            xEventGroupClearBits(
                    TempEventGroup,
                    TEMP_NORMAL_BIT |
                    TEMP_CRITICAL_BIT);

            xEventGroupSetBits(
                    TempEventGroup,
                    TEMP_WARNING_BIT);

            xTaskNotifyGive(
                    AlarmTaskHandle);
        }
        else
        {
            xEventGroupClearBits(
                    TempEventGroup,
                    TEMP_NORMAL_BIT |
                    TEMP_WARNING_BIT);

            xEventGroupSetBits(
                    TempEventGroup,
                    TEMP_CRITICAL_BIT);

            xTaskNotifyGive(
                    AlarmTaskHandle);
        }

        vTaskDelay(
                pdMS_TO_TICKS(500));
    }
}

void LCDTask(void *pvParameters)
{
    TempData_t local;
    char lcd_buf[20];

    EventBits_t bits;

    while(1)
    {
        xSemaphoreTake(
                TempMutex,
                portMAX_DELAY);

        local = gTempData;

        xSemaphoreGive(
                TempMutex);

        bits =
            xEventGroupGetBits(
                    TempEventGroup);

        lcd_clear();

        sprintf(
                lcd_buf,
                "T=%.1fC",
                local.temperature);

        lcd_put_cur(0,0);
        lcd_send_string(lcd_buf);

        lcd_put_cur(1,0);

        if(bits & TEMP_NORMAL_BIT)
        {
            lcd_send_string("NORMAL");
        }
        else if(bits &
                TEMP_WARNING_BIT)
        {
            lcd_send_string("WARNING");
        }
        else if(bits &
                TEMP_CRITICAL_BIT)
        {
            lcd_send_string("CRITICAL");
        }

        vTaskDelay(
                pdMS_TO_TICKS(500));
    }
}
void LoggerTask(void *pvParameters)
{
    TempData_t data;

    char txbuf[100];

    while(1)
    {
        if(xQueueReceive(
                LogQueue,
                &data,
                portMAX_DELAY)
                == pdPASS)
        {
            sprintf(
                txbuf,
                "ADC=%u V=%.3f T=%.2f\r\n",
                data.adc,
                data.voltage,
                data.temperature);

            HAL_UART_Transmit(
                    &huart2,
                    (uint8_t*)txbuf,
                    strlen(txbuf),
                    HAL_MAX_DELAY);
        }
    }
}
void AlarmTask(void *pvParameters)
{
    EventBits_t bits;

    while(1)
    {
        ulTaskNotifyTake(
                pdTRUE,
                portMAX_DELAY);

        bits =
            xEventGroupGetBits(
                    TempEventGroup);

        if(bits &
                TEMP_CRITICAL_BIT)
        {

            HAL_GPIO_WritePin(
                    GPIOB,
                    GPIO_PIN_5,
                    GPIO_PIN_SET);

            HAL_GPIO_WritePin(
                    GPIOA,
                    GPIO_PIN_10,
                    GPIO_PIN_SET);
        }
        else if(bits &
                TEMP_WARNING_BIT)
        {
            HAL_GPIO_TogglePin(
                    GPIOA,
                    GPIO_PIN_10);
        }
        else
        {
            HAL_GPIO_WritePin(
                    GPIOA,
                    GPIO_PIN_10,
                    GPIO_PIN_RESET);

            HAL_GPIO_WritePin(
                    GPIOB,
                    GPIO_PIN_5,
                    GPIO_PIN_RESET);
        }
    }
}
uint16_t Read_ADC(void)
{
    uint16_t adc_value;

    HAL_ADC_Start(&hadc1);

    HAL_ADC_PollForConversion(
            &hadc1,
            HAL_MAX_DELAY);

    adc_value =
            HAL_ADC_GetValue(
                    &hadc1);

    HAL_ADC_Stop(&hadc1);

    return adc_value;
}


void vAssertCalled(void)//When any error is detected from FreeRTOS, this function stops the RTOS tasks and CPU got to sleep
 	{
 		taskDISABLE_INTERRUPTS();

 		while (1) {

 		}
 	}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{

  if (htim->Instance == TIM6)
  {
    HAL_IncTick();
  }

}


void Error_Handler(void)
{

  __disable_irq();
  while (1)
  {
  }
}
#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t *file, uint32_t line)
{

}
#endif

