/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   2.4g无线模块/nrf24l01+/单板 测试  只使用一块开发板，
	*					 对2个NRF模块进行收发测试
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火  STM32 F407 开发板
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */
  
#include "stm32f4xx.h"
#include "./usart/bsp_debug_usart.h"
#include "./nrf/bsp_spi_nrf.h"
#include "./key/bsp_key.h"


u8 status;	               // 用于判断接收/发送状态
u8 txbuf[32]={0,1,2,3};	   // 发送缓冲
u8 rxbuf[32];			         // 接收缓冲
int i=0;

void Self_Test(void);

/**
  * @brief  主函数
  * @param  无
  * @retval 无
  */
int main(void)                  
{   
	/* 初始化NRF1 */
  SPI_NRF_Init();

  /* 串口1初始化 */
  Debug_USART_Config();
  
  /* 按键初始化 */
  Key_GPIO_Config();

  printf("\r\n 这是一个 NRF24L01 无线传输实验 \r\n");
  printf("\r\n 这是无线传输 主机端 的反馈信息\r\n");
  printf("\r\n   正在检测NRF与MCU是否正常连接。。。\r\n");

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
  status = NRF_Check(); 

  /*判断连接状态*/  
  if(status == SUCCESS)	   
    printf("\r\n      NRF1与MCU连接成功！ 按 K1 发送数据\r\n");  
  else	  
    printf("\r\n  NRF1与MCU连接失败，请重新检查接线。\r\n");

  NRF_RX_Mode();     // NRF1 进入接收模式

  while(1)
  {
    /* 等待 NRF1 接收数据 */
    status = NRF_Rx_Dat(rxbuf);

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
    if (Key_Scan(KEY1_GPIO_PORT, KEY1_PIN) == KEY_ON)    // 按键按下，开始送数据
    { 
      /* 发送数据 */
      NRF_TX_Mode();
           
      status = NRF_Tx_Dat(txbuf);
      
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

      NRF_RX_Mode();
    }
  }
}

/*********************************************END OF FILE**********************/

