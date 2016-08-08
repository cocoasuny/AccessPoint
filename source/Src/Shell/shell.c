/*********************************Copyright (c)*********************************
**---------------------------------File Info------------------------------------
** File Name:               shell.c
** Last modified Date:      2014/3/5 16:43:59
** Last Version:            V2  
** Description:             none
**
**------------------------------------------------------------------------------
** Created By:              wanxuncpx
** Created date:            2014/3/5 16:43:58
** Version:                 V2
** Descriptions:            适合于STM32
**------------------------------------------------------------------------------
** Libraries:               STM32F10x_StdPeriph_Driver
** version                  V3.5
** 使用说明：
** 		（1）:	增加xxx_xxx_shell.c文件
**      （2）:  xxx_xxx_shell.c文件中增加命令帮助文件
**				const char BlueNRGCentral_HelpMsg[] =
						"[BlueNRGCentral contorls]\r\n"
						" ble central help\t\t- help.\r\n"
						" ble central start scan\r\n"
						" ble central stop scan\r\n"
						"\r\n";
**       (3):   增加Shell_xxx_xxx_Service()函数，在该函数中，对指令进行解析。
**       (4):	在shell.c文件中增加命令帮助文件,显示所有可用Shell模块
				const char Shell_HelpMsg[] =
				"Pls Enter [ModuleName help] for more info\r\n"
				"Available Shell Modules:\r\n"
				"  rtc\r\n"
				"  ble central\r\n"
				"\r\n";
**     	 (5):   在void Shell_ProcessorHandler(void)函数中，调用对应的Shell_xxx_xxx_Service()函数。
*******************************************************************************/


#include "shell.h"      //包含shell接口文件

#define  SHELL_HAL_EXT

//是否使用扩展的shell接口
#ifdef SHELL_HAL_EXT
  #include "shell_hal.h"    //本地的shell文件
#else
  #include "console.h"      //标准的shell文件
#endif

/******************************************************************************
********************************* Shell.h定义 *********************************
******************************************************************************/
/*---------------------* 
*     Shell 收发标记
*----------------------*/

#ifdef SHELL_ENABLE

volatile uint16_t   shell_rx_rdy = 0;                     //0:空闲，非零：忙
volatile uint8_t    shell_rx_buff[SHELL_RX_MAX+1]="\0";   //接收缓存


//接收
static volatile uint16_t    shell_rx_index = 0;           //数据接收标记


//命令帮助文件,显示所有可用Shell模块
const char Shell_HelpMsg[] =
	"================================================================\r\n"
    "*                                                              *\r\n"
	"*      Please Enter [ModuleName help] for more info            *\r\n" 
	"*      Available Shell Modules:                                *\r\n"
	"*      rtc                                                     *\r\n"
	"*      ble                                                     *\r\n"
	"*      fil                                                     *\r\n"
	"================================================================\r\n"
	"\r\n";

/**
  * @brief  Shell串口初始化,使用中断单字节接收数据 
  * @param  uint32_t baud 
  */ 
void shell_Init(uint32_t baud)
{
    //已在Bsp初始化中初始化
//    USART_InitTypeDef   USART_InitStructure;
//    NVIC_InitTypeDef    NVIC_UART_Cfg;  //UART中断向量  
//    
//	//--------------------------- 先定义好数据结构 ---------------------------  
//    //定义好USART结构体

//    USART_InitStructure.USART_BaudRate      = baud;
//    USART_InitStructure.USART_WordLength    = USART_WordLength_8b;
//    USART_InitStructure.USART_StopBits      = USART_StopBits_1;
//    USART_InitStructure.USART_Parity        = USART_Parity_No ;
//    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
//    USART_InitStructure.USART_Mode          = USART_Mode_Tx | USART_Mode_Rx;
//    USART_InitStructure.USART_BaudRate = USART_InitStructure.USART_BaudRate;    //防止编译报错  
//    
//    //定义好NVIC:UART中断  
//    NVIC_UART_Cfg.NVIC_IRQChannel = CONSOLE_IRQn;
//    NVIC_UART_Cfg.NVIC_IRQChannelPreemptionPriority = 1;
//    NVIC_UART_Cfg.NVIC_IRQChannelSubPriority = CONSOLE_UART_PRIO;
//    NVIC_UART_Cfg.NVIC_IRQChannelCmd = ENABLE;
//    NVIC_UART_Cfg.NVIC_IRQChannel = NVIC_UART_Cfg.NVIC_IRQChannel;              //防止编译报错  
//    
//    //模式配置  
//    //--------------------------- 中断方式收发数据 ----------------------------
//    CONSOLE_UART_RCC_INIT();                                //打开USART的时钟  
//    USART_Cmd(CONSOLE, DISABLE);                            //关闭UART  
//    
//    USART_Init(CONSOLE, &USART_InitStructure);              //初始化串口  
//    
//    USART_ITConfig(CONSOLE, USART_IT_RXNE, ENABLE);
//    USART_ITConfig(CONSOLE, USART_IT_IDLE, ENABLE);
//    
//    USART_Cmd(CONSOLE, ENABLE);
//    
//    NVIC_Init(&NVIC_UART_Cfg);                              //配置好NVIC  
}

/**
  * @brief  UART error callbacks.
  * @param  huart: pointer to a UART_HandleTypeDef structure that contains
  *                the configuration information for the specified UART module.
  * @retval None
  */
void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
  /* Prevent unused argument(s) compilation warning */

  /* NOTE: This function Should not be modified, when the callback is needed,
           the HAL_UART_ErrorCallback could be implemented in the user file
   */ 
    printf("Uart Err:0x%x\r\n",huart->ErrorCode);   
}

/**
  * @brief  Rx Transfer completed callbacks.
  * @param  huart: pointer to a UART_HandleTypeDef structure that contains
  *                the configuration information for the specified UART module.
  * @retval None
  */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  /* Prevent unused argument(s) compilation warning */
  
  /* NOTE: This function Should not be modified, when the callback is needed,
           the HAL_UART_TxCpltCallback could be implemented in the user file
   */
//    uint8_t i=0;
	if(huart == &huart1)
	{        
        //接收中断
        if(shell_rx_rdy)
        {
            shell_rx_index = 0;     //忙模式收到字节,重置接收指针
        }
        else
        {
            if( shell_rx_index < SHELL_RX_MAX)
            {
                shell_rx_buff[shell_rx_index] = g_aRxBuffer[0];
                shell_rx_index++;
            }
            else
            {
                shell_rx_index = 0; 
            }
        }
        
        if(shell_rx_rdy)
        {
            shell_rx_index = 0;     //忙模式收到字节,重置接收指针
        }
        else
        {
            if( (shell_rx_index >=2) && ('\r' == shell_rx_buff[shell_rx_index-2]) &&
                ('\n' == shell_rx_buff[shell_rx_index-1])   )       //以"\r\n"结尾  
            {
//                  // for test 
//                printf("\r\n");
//                for(i=0;i<shell_rx_index-2;i++)
//                {
//                    printf("%c",shell_rx_buff[i]);
//                }
//                printf("\r\n");
                shell_rx_rdy = shell_rx_index;
                shell_rx_index = 0;
            }
            else if( (shell_rx_index > 0) && ('\b' == shell_rx_buff[shell_rx_index-1]) ) //以\b结尾  
            {
                shell_rx_index = shell_rx_index <2? 0:shell_rx_index-2;
                printf(" \b");      //发送辅助删除           
            }
        }
        /* Put UART peripheral in reception process ###########################*/  
        if(HAL_UART_Receive_IT(&huart1, (uint8_t *)g_aRxBuffer, RXBUFFERSIZE) != HAL_OK)
        {
            //printf("Uart Init Error\r\n");
        }
        else
        {
            //printf("Next Rx\r\n");
        }         
	}
}
/**
  * @brief  Shell_Invalid_Service
  * @param  None
  * @retval None
  */
void Shell_Invalid_Service(void)
{
    int         tx_len,i;
    uint8_t *   ptSrc;
    uint8_t *   ptDst;
    uint8_t     tmp_buff[64];
	RTC_DateTypeDef date_s;
    RTC_TimeTypeDef rtc_time;
    
    //指令识别  
    if(2 > shell_rx_rdy)
    {
        shell_rx_buff[0]  = 0;
        return;
    }
    else if( ('\r' == shell_rx_buff[shell_rx_rdy-2]) && ('\n' == shell_rx_buff[shell_rx_rdy-1]) )
    {
        ptSrc = (uint8_t *)shell_rx_buff;
        if(2 == shell_rx_rdy)
        {
            //填写数据  
            tx_len = (uint16_t)sprintf((void *)tmp_buff,"AT:OK!");
        
            //发送数据 
            printf("%s\r\n",tmp_buff);
        }
		else if(StrComp(ptSrc,"help\r\n"))
		{
			//显示所有可用Shell Module
			printf("%s",Shell_HelpMsg);
		}
		else if(StrComp(ptSrc,"the time\r\n"))
		{
			//显示RTC时间
			Calendar_Get(&date_s,&rtc_time);
			printf("\r\nRTC:  %02d-%02d-%02d %d %0.2d:%0.2d:%0.2d\r\n",2000 + date_s.Year,date_s.Month, date_s.Date,date_s.WeekDay, 
                                                               rtc_time.Hours, rtc_time.Minutes, rtc_time.Seconds); 
		}
		else if(StrComp(ptSrc,"restart\r\n")) //重启设备
		{
			Sys_SoftReset();
		}
        else goto ERROR_LOOP;
    }
    else
    {
ERROR_LOOP:
        //填写指令码
        tx_len = (uint16_t)sprintf((void *)tmp_buff,"\r\nAT: Cmd Error:\"");
        
        //计算地址,填写数据,填写尾部 
        ptDst = tmp_buff + tx_len;
        ptSrc = (uint8_t *)shell_rx_buff;
        if(shell_rx_rdy > 32)
        {
            for(i=0; i<32; i++)
            { 
                if( (*ptSrc > 126) || (*ptSrc < 32) )
                {
                    *ptDst++ = '?';
                     ptSrc++;   
                }
                else
                {
                    *ptDst++ = *ptSrc++; 
                } 
            }
            *(ptDst-2) = '-';
            *(ptDst-1) = '>';
            tx_len += 32;
        }
        else
        {
            for(i=0; i<shell_rx_rdy; i++)
            { 
                *ptDst++ = *ptSrc++;
                tx_len++; 
            }
            *(ptDst-2) = '<';
            *(ptDst-1) = '-';
        }
        tx_len += (uint16_t)sprintf((void *)ptDst,"\"\r\n");
  
        //发送数据 
        printf("%s\r\n",tmp_buff);
    }
    
    //清除数据返回程序  
    shell_rx_buff[0]  = 0;
    shell_rx_rdy      = 0;
	printf("-->");
}

/****************************************************************************** 
/ 函数功能:字符串测试匹配指令 
/ 修改日期:2014/3/5 19:30:22 
/ 输入参数:none 
/ 输出参数:none 
/ 使用说明:none 
******************************************************************************/  
bool StrComp(void * buffer,void * StrCmd)
{
    uint8_t i;
    uint8_t * ptBuf;
    uint8_t * ptCmd;
    
    ptBuf = (uint8_t *)buffer;
    ptCmd = (uint8_t *)StrCmd;
    for(i=0; i<255; i++)
    {
        if(ptCmd[i])
        {
            if(ptBuf[i] != ptCmd[i])return false;
        }
        else 
        {
            if(i)return i;
            else return false;    
        }
    }
    return false;
}
/**
  * @brief  Shell_ProcessorHandler
  * @param  None
  * @retval None
  */
void Shell_ProcessorHandler(void)
{
	if(shell_rx_rdy)
	{
		#ifdef RTC_SHELL
			Shell_RTC_Service();
		#endif
		#ifdef BLUENRG_CENTRAL_SHELL
			Shell_BlueNRG_Central_Service();
		#endif
		#ifdef FATFS_SHELL
			Shell_Fatfs_Service();
		#endif
		Shell_Invalid_Service();  //指令无效的缺省处理
	}
}

#endif
