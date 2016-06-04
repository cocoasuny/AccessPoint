/**
  ******************************************************************************
  * @file    bluenrg_sensor_service.c
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    04-July-2014
  * @brief   Add a sample service using a vendor specific profile.
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
#include "bluenrg_remotecontrol_service.h"


/* accelerometer service and char UUID define */
#define COPY_REMOTE_CONTROL_SERVICE_UUID(uuid_struct)  COPY_UUID_128(uuid_struct,0xa3,0xd8,0xb5,0x80, 0x17, 0x80, 0x11,0xe6, 0xbd,0xf4, 0x08,0x00,0x20,0x0c,0x9a,0x66)
#define COPY_LED_CONTROL_UUID(uuid_struct)             COPY_UUID_128(uuid_struct,0xa3,0xd8,0xb5,0x81, 0x17, 0x80, 0x11,0xe6, 0xbd,0xf4, 0x08,0x00,0x20,0x0c,0x9a,0x66)

/* variables ---------------------------------------------------------*/
uint16_t remoteControlServHandle, ledControlCharHandle;


/*******************************************************************************
* Function Name  : Add_RemoteControl_Service
* Description    : 添加远程控制服务
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
tBleStatus Add_RemoteControl_Service(void)
{
    tBleStatus ret;

    uint8_t uuid[16];

    /* add service */
    COPY_REMOTE_CONTROL_SERVICE_UUID(uuid);
    ret = aci_gatt_add_serv(UUID_TYPE_128, uuid,PRIMARY_SERVICE,11,&remoteControlServHandle);
    if(ret != BLE_STATUS_SUCCESS) goto fail;    

    /* add LED Control characteristic */
    COPY_LED_CONTROL_UUID(uuid);
    ret = aci_gatt_add_char(remoteControlServHandle, UUID_TYPE_128, uuid, 20,
                           CHAR_PROP_WRITE | CHAR_PROP_WRITE_WITHOUT_RESP, ATTR_PERMISSION_NONE, GATT_NOTIFY_ATTRIBUTE_WRITE,
                           16, 1, &ledControlCharHandle);
    if (ret != BLE_STATUS_SUCCESS) goto fail;


    return BLE_STATUS_SUCCESS; 

    fail:
    #ifdef Debug_BlueNRF
        DLog("Error while adding Remote Control service.\n");
    #endif
    return BLE_STATUS_ERROR ;    
}








