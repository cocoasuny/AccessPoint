/**
  ******************************************************************************
  * @file    ble_Central_Service.h
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
#ifndef __BLE_CENTRAL_SERVICE_H_
#define __BLE_CENTRAL_SERVICE_H_

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"


/* ble central service debug switch */
#define Debug_Ble_Central_Service


/** 
  * @brief  Ble Central services structures definition  
  */
typedef struct
{
	uint8_t 		uuid[16];
	uint16_t		serviceHandle;
	uint16_t        endGroupHandle;
    bool            isServiceValid;
	bool            isServiceCharacterFindComplete;
}BLE_CENTRAL_SERVICE_INFO_T;	

/** 
  * @brief  Ble Central Characteristics structures definition  
  */
typedef struct
{
    uint8_t         uuid[16];
    uint8_t         properties;
    uint16_t        handle;
    uint16_t        valueHandle;
    bool            isCharacterValid;
	bool            isCharacterNoticificationEnable;
}BLE_CENTRAL_CHARACTER_INFO_T;
    
  

/* Exported types ------------------------------------------------------------*/



/* Exported functions */
tBleStatus ble_CentralService_Init(void);
tBleStatus ble_Central_Add_Acc_Service(void);  
tBleStatus GAP_Discovery_Service(uint16_t conn_handle);
tBleStatus GAP_Discovery_Characteristics(uint16_t conn_handle);
tBleStatus GAP_Discovery_Characteristics_CB(evt_att_read_by_type_resp *pdata);
void GAP_Discovery_Service_CB(evt_att_read_by_group_resp *pdata);
void GAP_Discovery_Service_Complete_CB(evt_gatt_procedure_complete *pdata);
void GAP_Discovery_AllCharacter_Complete_CB(uint16_t conn_handle);
void Enable_Character_Descriptor_ForNoticification(uint16_t conn_handle);
tBleStatus GATT_AttributeData_Noticification_CB(evt_gatt_attr_notification *pdata);
tBleStatus GetMasterConnectListLocationFromHandle(uint8_t *Location,uint16_t conn_handle);
tBleStatus GetMasterConnectListLocationFromMACAddr(uint8_t *Location,uint8_t addr[6]);
  
#endif /* __BLE_CENTRAL_SERVICE_H_ */



