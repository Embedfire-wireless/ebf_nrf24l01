/**
  ******************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2019-xx-xx
  * @brief   无线模块/nrf24l01+/单板测试
  ******************************************************************
  * @attention
  *
  * 实验平台:野火 STM32H750 开发板 
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
  *
  ******************************************************************
  */  
#include "stm32h7xx.h"
#include "main.h"
#include "./led/bsp_led.h"
#include "./delay/core_delay.h" 
#include "./mpu/bsp_mpu.h" 
#include "./usart/bsp_usart.h"
#include "./NRF24L01/bsp_nrf24l01.h"
#include "./key/bsp_key.h"


uint8_t status;	               // 用于判断接收/发送状态
uint8_t status2;                // 用于判断接收/发送状态
uint8_t txbuf[32]={0,1,2,3,4,5,6};	   // 发送缓冲
uint8_t rxbuf[32];			         // 接收缓冲
int i=0;

void Self_Test(void);

/**
  * @brief  主函数
  * @param  无
  * @retval 无
  */
int main(void)
{    
    
    /* 系统时钟初始化成480MHz */
    SystemClock_Config();

    SCB_EnableICache();    // 使能指令 Cache
    SCB_EnableDCache();    // 使能数据 Cache

    /* LED 端口初始化 */
    LED_GPIO_Config();	

    /* 配置串口1为：115200 8-N-1 */
    UARTx_Config();

        /* 初始化NRF1 */
    SPI_NRF1_Init();

    /* 初始化NRF2 */
    SPI_NRF2_Init();

    Key_GPIO_Config();
    printf("\r\n 这是一个 NRF24L01 无线传输实验 \r\n");
    printf("\r\n 这是无线传输 主机端 的反馈信息\r\n");
    printf("\r\n   正在检测NRF与MCU是否正常连接。。。\r\n");

    /*收发数据测试*/
    Self_Test();	
		
}

/**
  * @brief  NRF模块测试函数，NRF1和NRF2之间循环发送数据
  * @param  无
  * @retval 无
  */
void Self_Test(void)
{
  /*检测 NRF 模块与 MCU 的连接*/
  status = NRF1_Check(); 

  /*判断连接状态*/  
  if(status == SUCCESS)	   
    printf("\r\n      NRF1与MCU连接成功！ 按 K1 发送数据\r\n");  
  else	  
    printf("\r\n  NRF1与MCU连接失败，请重新检查接线。\r\n");
	
	
	/*检测 NRF2 模块与 MCU 的连接*/
  status = NRF2_Check();
	
	 /*判断连接状态*/  
  if(status == SUCCESS)	   
    printf("\r\n      NRF2与MCU连接成功！ 按 K2 发送数据\r\n");  
  else	  
    printf("\r\n  NRF2与MCU连接失败，请重新检查接线。\r\n");

  NRF1_RX_Mode();     // NRF1 进入接收模式
  NRF2_RX_Mode();    // NRF2 进入接收模式

  while(1)
  {
    /* 等待 NRF1 接收数据 */
    status = NRF1_Rx_Dat(rxbuf);

    /* 判断接收状态 */
    if(status == RX_DR)
    {
      for(i=0;i<32;i++)
      {	
        printf("\r\n NRF1 接收数据为：%d \r\n",rxbuf[i]); 
      }
      
      printf("\r\n按 K1 使用NRF1 发送数据\n"); 
      printf("\r\n按 K2 使用NRF2 发送数据\r\n"); 
    }
    
    /* NRF1 发送数据 */
    if (Key_Scan(KEY2_GPIO_PORT, KEY2_PIN) == KEY_ON)    // 按键按下，开始送数据
    { 
			LED1_TOGGLE;
      /* 发送数据 */
      NRF1_TX_Mode();
           
      status = NRF1_Tx_Dat(txbuf);
      
      /* 发送数据的状态 */
       if(status == TX_DS)
      {
        printf("\r\nNRF1 发送数据成功\r\n");
      }
      else
      {
        printf("\r\nNRF1 发送数据失败  %d\r\n", status);
      }
      
      printf("\r\nNRF1 进入接收模式\r\n"); 

      NRF1_RX_Mode();
    }
    
    /* 等待 NRF2 接收数据 */
    status2 = NRF2_Rx_Dat(rxbuf);

    /* 判断接收状态 */
    if(status2 == RX_DR)
    {
      for(i=0;i<32;i++)
      {	
        printf("\r\n NRF2 接收数据为：%d \r\n",rxbuf[i]); 
      }
      
      printf("\r\n按 K1 使用NRF1 发送数据\n"); 
      printf("\r\n按 K2 使用NRF2 发送数据\r\n"); 
    }
    
    /* NRF2 发送数据 */
    if (Key_Scan(KEY1_GPIO_PORT, KEY1_PIN) == KEY_ON)    // 按键 2 按下，开始送数据
    { 
			LED2_TOGGLE;
      /* 发送数据 */
      NRF2_TX_Mode();
           
      status2 = NRF2_Tx_Dat(txbuf);
      
      /* 发送数据的状态 */
       if(status2 == TX_DS)
      {
        printf("\r\nNRF2 发送数据成功\r\n");
      }
      else
      {
        printf("\r\nNRF2 发送数据失败  %d\r\n", status);
      }
      
      printf("\r\nNRF2 进入接收模式\r\n"); 

      NRF2_RX_Mode();
    }
  }
}

/**
  * @brief  System Clock 配置
  *         system Clock 配置如下: 
	*            System Clock source  = PLL (HSE)
	*            SYSCLK(Hz)           = 480000000 (CPU Clock)
	*            HCLK(Hz)             = 240000000 (AXI and AHBs Clock)
	*            AHB Prescaler        = 2
	*            D1 APB3 Prescaler    = 2 (APB3 Clock  120MHz)
	*            D2 APB1 Prescaler    = 2 (APB1 Clock  120MHz)
	*            D2 APB2 Prescaler    = 2 (APB2 Clock  120MHz)
	*            D3 APB4 Prescaler    = 2 (APB4 Clock  120MHz)
	*            HSE Frequency(Hz)    = 25000000
	*            PLL_M                = 5
	*            PLL_N                = 192
	*            PLL_P                = 2
	*            PLL_Q                = 4
	*            PLL_R                = 2
	*            VDD(V)               = 3.3
	*            Flash Latency(WS)    = 4
  * @param  None
  * @retval None
  */
/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** 启用电源配置更新
  */
  HAL_PWREx_ConfigSupply(PWR_LDO_SUPPLY);
  /** 配置主内稳压器输出电压
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE0);

  while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}
  /** 初始化CPU、AHB和APB总线时钟
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 5;
  RCC_OscInitStruct.PLL.PLLN = 192;
  RCC_OscInitStruct.PLL.PLLP = 2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  RCC_OscInitStruct.PLL.PLLR = 2;
  RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_2;
  RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
  RCC_OscInitStruct.PLL.PLLFRACN = 0;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
		while(1);
  }
  /** 初始化CPU、AHB和APB总线时钟
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2
                              |RCC_CLOCKTYPE_D3PCLK1|RCC_CLOCKTYPE_D1PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV2;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV2;
  RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
		while(1);
  }
}
/****************************END OF FILE***************************/
