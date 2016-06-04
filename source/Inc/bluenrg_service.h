/**
  ******************************************************************************
  * @file    bluennrg_service.h 
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
#ifndef _BLUENRG_SERVICE_H_
#define _BLUENRG_SERVICE_H_

#include "stm32f4xx_hal.h"
#include "ble_status.h"


/* Private macros ------------------------------------------------------------*/
#define COPY_UUID_128(uuid_struct, uuid_15, uuid_14, uuid_13, uuid_12, uuid_11, uuid_10, uuid_9, uuid_8, uuid_7, uuid_6, uuid_5, uuid_4, uuid_3, uuid_2, uuid_1, uuid_0) \
do {\
    uuid_struct[0] = uuid_0; uuid_struct[1] = uuid_1; uuid_struct[2] = uuid_2; uuid_struct[3] = uuid_3; \
        uuid_struct[4] = uuid_4; uuid_struct[5] = uuid_5; uuid_struct[6] = uuid_6; uuid_struct[7] = uuid_7; \
            uuid_struct[8] = uuid_8; uuid_struct[9] = uuid_9; uuid_struct[10] = uuid_10; uuid_struct[11] = uuid_11; \
                uuid_struct[12] = uuid_12; uuid_struct[13] = uuid_13; uuid_struct[14] = uuid_14; uuid_struct[15] = uuid_15; \
}while(0)


/*Ble State*/
#define BLE_CONNECTABLE		1
#define BLE_NOCONNECTABLE	0

/* Ble device name max lengh */
#define DeviceMaxName       10


/*adv parameter structure*/
typedef struct
{
    uint8_t               type;                 /**< See @ref BLE_GAP_ADV_TYPES. */
    uint8_t               fp;                   /**< Filter Policy, see @ref BLE_GAP_ADV_FILTER_POLICIES. */
    uint16_t              interval;             /**< Advertising interval between 0x0020 and 0x4000 in 0.625 ms units (20ms to 10.24s), see @ref BLE_GAP_ADV_INTERVALS.
                                                   - If type equals @ref BLE_GAP_ADV_TYPE_ADV_DIRECT_IND, this parameter must be set to 0 for high duty cycle directed advertising.
                                                   - If type equals @ref BLE_GAP_ADV_TYPE_ADV_DIRECT_IND, set @ref BLE_GAP_ADV_INTERVAL_MIN <= interval <= @ref BLE_GAP_ADV_INTERVAL_MAX for low duty cycle advertising.*/
    uint16_t              timeout;              /**< Advertising timeout between 0x0001 and 0x3FFF in seconds, 0x0000 disables timeout. See also @ref BLE_GAP_ADV_TIMEOUT_VALUES. If type equals @ref BLE_GAP_ADV_TYPE_ADV_DIRECT_IND, this parameter must be set to 0 for High duty cycle directed advertising. */

} ble_gap_adv_params_t;

typedef enum 
{
    CLIENT = 0, /**< CLIENT is for Central role. */
    SERVER = 1  /**< SERVER is for Peripheral role. */
} BLE_RoleTypeDef;


extern uint8_t g_tx_power_level;


void BlueNRG_Init(void);
void Advertising_Init(void);
void HCI_Event_CB(void *pckt);
tBleStatus Service_Init(void);
tBleStatus Ble_AdvAddress_Set(void);
tBleStatus Start_Advertise(void);
tBleStatus Ble_SetTx_Power(uint8_t level);

void GAP_ConnectionComplete_CB(uint8_t addr[6], uint16_t handle);
void GAP_DisconnectionComplete_CB(void);
void Attribute_Modified_CB(uint16_t handle, uint8_t data_length, uint8_t *att_data,uint8_t offset);


#endif /* _BLUENRG_SERVICE_H_ */




