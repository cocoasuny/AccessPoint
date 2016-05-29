/**
  ******************************************************************************
  * @file    platform.h
  * @author  CL
  * @version V1.0.0
  * @date    04-July-2014
  * @brief   
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2014 STMicroelectronics</center></h2>
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
  
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __PLATFORM_H_
#define __PLATFORM_H_

/* Includes ------------------------------------------------------------------*/
#include "stm32l0xx_hal.h"


/* Debug Switch */
#define PRINTFLOG
#define Debug_BlueNRF
#define Debug_Sensor_Humidity
#define Debug_Sensor_Press
#define Debug_Sensor_Temperature
#define Debug_LedControl


/******** Task define ********************/
/* BlueNRG HCI Process Task */
#define Task_BlueNRGHCI_Stack        500    //task stack
#define Task_BlueNRGHCI_Priority     2      //task priority



#ifdef PRINTFLOG
    #define DLog  printf
#else
    #define DLog  USBLog
#endif    

/* Button Pin define  */
#define	B1_Pin 			GPIO_PIN_13
#define B1_GPIO_Port 	GPIOC

/* USART Pin define */
#define USART_TX_Pin 	GPIO_PIN_2
#define USART_TX_GPIO_	Port GPIOA
#define USART_RX_Pin 	GPIO_PIN_3
#define USART_RX_GPIO_	Port GPIOA

/* LED Pin define */
#define LD2_Pin 		GPIO_PIN_5
#define LD2_GPIO_Port 	GPIOA


#endif /* __PLATFORM_H_ */


