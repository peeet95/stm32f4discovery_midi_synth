#include "main.h"
#include "lfluidsynth.h"


/* Private function prototypes -----------------------------------------------*/
static void SystemClock_Config(void);
static void Error_Handler(void);
static void MPU_Config(void);
static void CPU_CACHE_Enable(void);

#define SD_SOUNDFONT_ROM

#ifdef SD_SOUNDFONT_ROM
FATFS SDFatFs __attribute__((section(".SRAM")));  /* File system object for SD card logical drive */

void SD_init() {
  char SDPath[4]; /* SD card logical drive path */
  if (FATFS_LinkDriver(&SD_Driver, SDPath) == 0)
  {
    if (f_mount(&SDFatFs, (TCHAR const*)SDPath, 0) == FR_OK)
    {
    } else {
      Error_Handler();
    }
  } else {
    Error_Handler();

  }
}
#endif


fluid_synth_t* synth;

//__attribute__((section(".SRAM")))
// __attribute__((section(".DTCM")))
#ifdef AUDIO_FORMAT_32BITS
//uint32_t  buf[AUDIO_BUF_SIZE] __attribute__((section(".DTCM")));
#else
//uint16_t  buf[AUDIO_BUF_SIZE] __attribute__((section(".DTCM")));
#endif

//uint8_t  buf[AUDIO_BUF_SIZE] __attribute__((section(".DTCM")));

uint8_t  buf[AUDIO_BUF_SIZE];

//static int16_t *buf;

uint8_t transfer_callback = 0;

void BSP_AUDIO_OUT_HalfTransfer_CallBack(void)
{
#ifdef AUDIO_FORMAT_32BITS
  fluid_synth_write_s32(synth, AUDIO_BUF_SIZE / 16, (uint32_t *)&buf[0], 0, 2, (uint32_t *)&buf[0], 1, 2 );
#else
  fluid_synth_write_s16(synth, AUDIO_BUF_SIZE / 8, (uint16_t *)&buf[0], 0, 2, (uint16_t *)&buf[0], 1, 2 );
#endif

  transfer_callback = 1;
}


void BSP_AUDIO_OUT_TransferComplete_CallBack(void)
{
#ifdef AUDIO_FORMAT_32BITS
  fluid_synth_write_s32(synth, AUDIO_BUF_SIZE / 16, (uint32_t *)(&buf[AUDIO_BUF_SIZE / 2]), 0, 2, (uint32_t *)(&buf[AUDIO_BUF_SIZE / 2]), 1, 2 );
#else
  fluid_synth_write_s16(synth, AUDIO_BUF_SIZE / 8, (uint16_t *)(&buf[AUDIO_BUF_SIZE / 2]), 0, 2, (uint16_t *)(&buf[AUDIO_BUF_SIZE / 2]), 1, 2 );
#endif
  transfer_callback = 2;

//  fluid_synth_write_s32(synth, AUDIO_BUF_SIZE / 8, (uint32_t *)&buf[0], 0, 2, (uint32_t *)&buf[0], 1, 2 );

}


/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void)
{
  /* Configure the MPU attributes as Write Through */
  MPU_Config();

  /* Enable the CPU Cache */
  CPU_CACHE_Enable();

  /* STM32F7xx HAL library initialization:
       - Configure the Flash ART accelerator on ITCM interface
       - Systick timer is configured by default as source of time base, but user
         can eventually implement his proper time base source (a general purpose
         timer for example or other time source), keeping in mind that Time base
         duration should be kept 1ms since PPP_TIMEOUT_VALUEs are defined and
         handled in milliseconds basis.
       - Set NVIC Group Priority to 4
       - Low Level Initialization
     */
  HAL_Init();

  /* Configure the system clock to 200 MHz */
  SystemClock_Config();

  /* Configure LED1 and LED3 */
  BSP_LED_Init(LED3);

  HAL_Delay(100);

  setbuf(stdout, NULL);

  BSP_LED_Off(LED3);

#ifdef SD_SOUNDFONT_ROM
  SD_init();
#endif

// QSPI
  QSPI_init();

  HAL_Delay(100);

  fluid_settings_t* settings;
  int sfont_id;

  /* Create the settings. */
  settings = new_fluid_settings();
  fluid_settings_setnum(settings, "synth.sample-rate", SAMPLE_RATE);

  fluid_settings_setstr(settings, "synth.reverb.active", "no");
  fluid_settings_setstr(settings, "synth.chorus.active", "no");
  fluid_settings_setint(settings, "synth.polyphony", POLYPHONY);

  /* Create the synthesizer. */
  synth = new_fluid_synth(settings);

  sfont_id = fluid_synth_sfload(synth, SOUNDFONT_FILE, 1);
  fluid_synth_set_interp_method(synth, -1, FLUID_INTERP_NONE);
//  fluid_synth_set_interp_method(synth, -1, FLUID_INTERP_LINEAR);
  BSP_LED_On(LED3);

  /* Make the connection and initialize to USB_OTG/usbdc_core */
  /*USBD_Init(&USBD_Device, &AUDIO_Desc, 0);
  USBD_RegisterClass(&USBD_Device, &USBD_Midi_ClassDriver);
  USBD_Midi_RegisterInterface(&USBD_Device, &USBD_Midi_fops);
  USBD_Start(&USBD_Device);*/

  HAL_Delay(5);

  //BSP_AUDIO_OUT_Init(OUTPUT_DEVICE_AUTO, MASTER_VOLUME, SAMPLE_RATE);
  //BSP_AUDIO_OUT_SetAudioFrameSlot(CODEC_AUDIOFRAME_SLOT_02);  // PCM 2-channel  TODO: do i need this?

#ifdef AUDIO_FORMAT_32BITS
  BSP_AUDIO_OUT_Play((uint32_t *)&buf[0], AUDIO_BUF_SIZE);
#else
  BSP_AUDIO_OUT_Play((uint16_t *)&buf[0], AUDIO_BUF_SIZE);
#endif

//  BSP_LED_Off(LED1);

  while (1)
  {
    BSP_LED_Toggle(LED3);
    HAL_Delay(1000);
  }

}


/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follow :
  *            System Clock source            = PLL (HSE)
  *            SYSCLK(Hz)                     = 180000000
  *            HCLK(Hz)                       = 180000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 4
  *            APB2 Prescaler                 = 2
  *            HSE Frequency(Hz)              = 8000000
  *            PLL_M                          = 8
  *            PLL_N                          = 360
  *            PLL_P                          = 2
  *            PLL_Q                          = 7
  *            VDD(V)                         = 3.3
  *            Main regulator output voltage  = Scale1 mode
  *            Flash Latency(WS)              = 5
  * @param  None
  * @retval None
  */
static void SystemClock_Config(void)
{
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_OscInitTypeDef RCC_OscInitStruct;

  /* Enable Power Control clock */
  __HAL_RCC_PWR_CLK_ENABLE();

  /* The voltage scaling allows optimizing the power consumption when the device is
     clocked below the maximum system frequency, to update the voltage scaling value
     regarding system frequency refer to product datasheet.  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /* Enable HSE Oscillator and activate PLL with HSE as source */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 360;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  HAL_RCC_OscConfig(&RCC_OscInitStruct);

  /* Activate the Over-Drive mode */
  HAL_PWREx_EnableOverDrive();

  /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2
     clocks dividers */
  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;
  HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5);
}

/**
  * @brief  Configure the MPU attributes as Write Through for SDRAM.
  * @note   The Base Address for SDRAM is 0xC0000000.
  *         The Region Size is 8MB, it is related to SDRAM memory size.
  * @param  None
  * @retval None
  */
static void MPU_Config(void)
{
  //MPU_Region_InitTypeDef MPU_InitStruct;

  /* Disable the MPU */
  HAL_MPU_Disable();

  /* Configure the MPU attributes as WT for SDRAM */
    /*MPU_InitStruct.Enable = MPU_REGION_ENABLE;
  #ifdef USE_SDRAM
    MPU_InitStruct.BaseAddress = 0xC0000000;
    MPU_InitStruct.Size = MPU_REGION_SIZE_8MB;
  #else
    MPU_InitStruct.BaseAddress = 0x20010000;
    MPU_InitStruct.Size = MPU_REGION_SIZE_256KB;
  #endif
  MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;
  MPU_InitStruct.IsBufferable = MPU_ACCESS_NOT_BUFFERABLE;
  MPU_InitStruct.IsCacheable = MPU_ACCESS_CACHEABLE;
  MPU_InitStruct.IsShareable = MPU_ACCESS_NOT_SHAREABLE;
  MPU_InitStruct.Number = MPU_REGION_NUMBER0;
  MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL0;
  MPU_InitStruct.SubRegionDisable = 0x00;
  MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_ENABLE;

  HAL_MPU_ConfigRegion(&MPU_InitStruct);*/

  /* Enable the MPU */
  //HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);
}

/**
  * @brief  CPU L1-Cache enable.
  * @param  None
  * @retval None
  */
static void CPU_CACHE_Enable(void)
{
  /* Enable branch prediction */
  //SCB->CCR |= (1 << 18);
  __DSB();

  /* Enable I-Cache */
  SCB_EnableICache();

  /* Enable D-Cache */
  SCB_EnableDCache();
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
static void Error_Handler(void)
{
  while (1)
  {
    /* LED1 blinks */
    BSP_LED_Toggle(LED3);
    HAL_Delay(100);
  }
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
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
