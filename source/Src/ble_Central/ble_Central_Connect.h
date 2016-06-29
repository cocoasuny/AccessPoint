/**
  ******************************************************************************
  * @file    ble_Central_Connect.h
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
#ifndef __BLE_CENTRAL_CONNECT_H_
#define __BLE_CENTRAL_CONNECT_H_

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"


/* Exported types ------------------------------------------------------------*/

/** 
  * @brief  Ble connect status structures definition  
  */
typedef enum
{
	CONNECTED,
	DISCONNECT,
	CONNECTABLE,
	DEFAULT
}BLE_CONNECT_STATUS_T;


/** 
  * @brief  Ble device information structures definition  
  */
typedef struct
{
	tBDAddr 						bdaddr;					//蓝牙设备地址,Bluetooth 48 bit address (in little-endian order).
	uint8_t 						data_RSSI[0]; 			// RSSI is last octect (signed integer).
	BLE_CONNECT_STATUS_T			ble_status;             //蓝牙连接状态	
	bool                            isValid;                //设备列表是否可用，不可用，则加入到下一个位置
}BLE_DEVICE_LIST_INFO_T;
 


/* Exported macro ------------------------------------------------------------*/
#define MAX_SUPPORT_SCAN_NBR          20
#define BLE_MACADDR_LEN               6   	//Bluetooth 48 bit address (in little-endian order).



void GAP_Scan_ADVData_CB(le_advertising_info *pdata);
void GAP_Discovery_CompleteCB(void);
void Start_Scan_Procedure(void);


#endif /* __BLE_CENTRAL_CONNECT_H_ */




