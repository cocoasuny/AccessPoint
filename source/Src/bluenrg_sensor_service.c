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
#include "bluenrg_sensor_service.h"


/* accelerometer service and char UUID define */
#define COPY_ACC_SERVICE_UUID(uuid_struct)  COPY_UUID_128(uuid_struct,0x02,0x36,0x6e,0x80, 0xcf,0x3a, 0x11,0xe1, 0x9a,0xb4, 0x00,0x02,0xa5,0xd5,0xc5,0x1b)
#define COPY_FREE_FALL_UUID(uuid_struct)    COPY_UUID_128(uuid_struct,0xe2,0x3e,0x78,0xa0, 0xcf,0x4a, 0x11,0xe1, 0x8f,0xfc, 0x00,0x02,0xa5,0xd5,0xc5,0x1b)
#define COPY_ACC_UUID(uuid_struct)          COPY_UUID_128(uuid_struct,0x34,0x0a,0x1b,0x80, 0xcf,0x4b, 0x11,0xe1, 0xac,0x36, 0x00,0x02,0xa5,0xd5,0xc5,0x1b)

/* Environmental Sensor service and char UUID define */
#define COPY_ENV_SENS_SERVICE_UUID(uuid_struct)  COPY_UUID_128(uuid_struct,0x42,0x82,0x1a,0x40, 0xe4,0x77, 0x11,0xe2, 0x82,0xd0, 0x00,0x02,0xa5,0xd5,0xc5,0x1b)
#define COPY_TEMP_CHAR_UUID(uuid_struct)         COPY_UUID_128(uuid_struct,0xa3,0x2e,0x55,0x20, 0xe4,0x77, 0x11,0xe2, 0xa9,0xe3, 0x00,0x02,0xa5,0xd5,0xc5,0x1b)
#define COPY_PRESS_CHAR_UUID(uuid_struct)        COPY_UUID_128(uuid_struct,0xcd,0x20,0xc4,0x80, 0xe4,0x8b, 0x11,0xe2, 0x84,0x0b, 0x00,0x02,0xa5,0xd5,0xc5,0x1b)
#define COPY_HUMIDITY_CHAR_UUID(uuid_struct)     COPY_UUID_128(uuid_struct,0x01,0xc5,0x0b,0x60, 0xe4,0x8c, 0x11,0xe2, 0xa0,0x73, 0x00,0x02,0xa5,0xd5,0xc5,0x1b)

/* variables ---------------------------------------------------------*/
uint16_t accServHandle, freeFallCharHandle, accCharHandle;
uint16_t envSensServHandle, tempCharHandle, pressCharHandle, humidityCharHandle;


/*******************************************************************************
* Function Name  : Add_Acc_Service
* Description    : 添加三轴传感器服务
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
tBleStatus Add_Acc_Service(void)
{
    tBleStatus ret;

    uint8_t uuid[16];

    /* add service */
    COPY_ACC_SERVICE_UUID(uuid);
    ret = aci_gatt_add_serv(UUID_TYPE_128, uuid,PRIMARY_SERVICE,7,&accServHandle);
    if(ret != BLE_STATUS_SUCCESS) goto fail;    

    /* add Free Fall characteristic */
    COPY_FREE_FALL_UUID(uuid);
    ret = aci_gatt_add_char(accServHandle, UUID_TYPE_128, uuid, 1,
                           CHAR_PROP_NOTIFY, ATTR_PERMISSION_NONE, 0,
                           16, 0, &freeFallCharHandle);
    if (ret != BLE_STATUS_SUCCESS) goto fail;

    /* add accelerometer characteristic */
    COPY_ACC_UUID(uuid);  
    ret =  aci_gatt_add_char(accServHandle, UUID_TYPE_128, uuid, 6,
                           CHAR_PROP_NOTIFY|CHAR_PROP_READ,
                           ATTR_PERMISSION_NONE,
                           GATT_NOTIFY_READ_REQ_AND_WAIT_FOR_APPL_RESP,
                           16, 0, &accCharHandle);
    if (ret != BLE_STATUS_SUCCESS) goto fail;

    return BLE_STATUS_SUCCESS; 

    fail:
    #ifdef Debug_BlueNRF
        DLog("Error while adding ACC service.\n");
    #endif
    return BLE_STATUS_ERROR ;    
}
/*******************************************************************************
* Function Name  : BlueNRG_Update_Acc
* Description    : BlueNRG Update Acceleration Value
* Input          : Structure containing acceleration value in mg
* Output         : None
* Return         : None
*******************************************************************************/
tBleStatus BlueNRG_Update_Acc(AxesRaw_t *data)
{  
    tBleStatus ret;    
    uint8_t buff[6];

    STORE_LE_16(buff,data->AXIS_X);
    STORE_LE_16(buff+2,data->AXIS_Y);
    STORE_LE_16(buff+4,data->AXIS_Z);

    ret = aci_gatt_update_char_value(accServHandle, accCharHandle, 0, sizeof(buff), buff);

    if (ret != BLE_STATUS_SUCCESS)
    {
        App_Error_Check(ret);
    }
    return ret;	
}

/*******************************************************************************
* Function Name  : Add_Environmental_Sensor_Service
* Description    : 添加环境传感器服务：温度、气压、湿度
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
tBleStatus Add_Environmental_Sensor_Service(void)
{
    tBleStatus ret;
    uint8_t uuid[16];
    uint16_t uuid16;
    charactFormat charFormat;
    uint16_t descHandle;

    COPY_ENV_SENS_SERVICE_UUID(uuid);
    ret = aci_gatt_add_serv(UUID_TYPE_128,  uuid, PRIMARY_SERVICE, 10,
                          &envSensServHandle);
    if (ret != BLE_STATUS_SUCCESS) goto fail;

    /* Temperature Characteristic */
    COPY_TEMP_CHAR_UUID(uuid);  
    ret =  aci_gatt_add_char(envSensServHandle, UUID_TYPE_128, uuid, 2,
                           CHAR_PROP_READ, ATTR_PERMISSION_NONE,
                           GATT_NOTIFY_READ_REQ_AND_WAIT_FOR_APPL_RESP,
                           16, 0, &tempCharHandle);
    if (ret != BLE_STATUS_SUCCESS) goto fail;

    charFormat.format = FORMAT_SINT16;
    charFormat.exp = -1;
    charFormat.unit = UNIT_TEMP_CELSIUS;
    charFormat.name_space = 0;
    charFormat.desc = 0;

    uuid16 = CHAR_FORMAT_DESC_UUID;

    ret = aci_gatt_add_char_desc(envSensServHandle,
                               tempCharHandle,
                               UUID_TYPE_16,
                               (uint8_t *)&uuid16, 
                               7,
                               7,
                               (void *)&charFormat, 
                               ATTR_PERMISSION_NONE,
                               ATTR_ACCESS_READ_ONLY,
                               0,
                               16,
                               FALSE,
                               &descHandle);
    if (ret != BLE_STATUS_SUCCESS) goto fail;

    /* Pressure Characteristic */
    COPY_PRESS_CHAR_UUID(uuid);  
    ret =  aci_gatt_add_char(envSensServHandle, UUID_TYPE_128, uuid, 3,
                             CHAR_PROP_READ, ATTR_PERMISSION_NONE,
                             GATT_NOTIFY_READ_REQ_AND_WAIT_FOR_APPL_RESP,
                             16, 0, &pressCharHandle);
    if (ret != BLE_STATUS_SUCCESS) goto fail;

    charFormat.format = FORMAT_SINT24;
    charFormat.exp = -5;
    charFormat.unit = UNIT_PRESSURE_BAR;
    charFormat.name_space = 0;
    charFormat.desc = 0;

    uuid16 = CHAR_FORMAT_DESC_UUID;

    ret = aci_gatt_add_char_desc(envSensServHandle,
                                 pressCharHandle,
                                 UUID_TYPE_16,
                                 (uint8_t *)&uuid16, 
                                 7,
                                 7,
                                 (void *)&charFormat, 
                                 ATTR_PERMISSION_NONE,
                                 ATTR_ACCESS_READ_ONLY,
                                 0,
                                 16,
                                 FALSE,
                                 &descHandle);
    if (ret != BLE_STATUS_SUCCESS) goto fail;
   
    /* Humidity Characteristic */
    COPY_HUMIDITY_CHAR_UUID(uuid);  
    ret =  aci_gatt_add_char(envSensServHandle, UUID_TYPE_128, uuid, 2,
                             CHAR_PROP_READ, ATTR_PERMISSION_NONE,
                             GATT_NOTIFY_READ_REQ_AND_WAIT_FOR_APPL_RESP,
                             16, 0, &humidityCharHandle);
    if (ret != BLE_STATUS_SUCCESS) goto fail;

    charFormat.format = FORMAT_UINT16;
    charFormat.exp = -1;
    charFormat.unit = UNIT_UNITLESS;
    charFormat.name_space = 0;
    charFormat.desc = 0;

    uuid16 = CHAR_FORMAT_DESC_UUID;

    ret = aci_gatt_add_char_desc(envSensServHandle,
                                 humidityCharHandle,
                                 UUID_TYPE_16,
                                 (uint8_t *)&uuid16, 
                                 7,
                                 7,
                                 (void *)&charFormat, 
                                 ATTR_PERMISSION_NONE,
                                 ATTR_ACCESS_READ_ONLY,
                                 0,
                                 16,
                                 FALSE,
                                 &descHandle);
    if (ret != BLE_STATUS_SUCCESS) goto fail;

    return BLE_STATUS_SUCCESS; 

    fail:
    DLog("Error while adding ENV_SENS service.\n");
    return BLE_STATUS_ERROR ;  
}
/*******************************************************************************
* Function Name  : Temp_Update
* Description    : 更新温度传感器数据
* Input          : temp
* Output         : None
* Return         : None
*******************************************************************************/
tBleStatus Temp_Update(int16_t temp)
{  
    tBleStatus ret;

    ret = aci_gatt_update_char_value(envSensServHandle, tempCharHandle, 0, 2,
                                   (uint8_t*)&temp);

    if (ret != BLE_STATUS_SUCCESS)
    {
        #ifdef Debug_Sensor_Temperature
            DLog("Error while updating TEMP characteristic.\n") ;
        #endif
        return BLE_STATUS_ERROR ;
    }
    return BLE_STATUS_SUCCESS;
	
}

/*******************************************************************************
* Function Name  : Press_Update
* Description    : 更新气压传感器数据
* Input          : press
* Output         : None
* Return         : None
*******************************************************************************/
tBleStatus Press_Update(int32_t press)
{  
    tBleStatus ret;

    ret = aci_gatt_update_char_value(envSensServHandle, pressCharHandle, 0, 3,
                                   (uint8_t*)&press);

    if (ret != BLE_STATUS_SUCCESS)
    {
        #ifdef Debug_Sensor_Press
            DLog("Error while updating TEMP characteristic.\n");
        #endif
        return BLE_STATUS_ERROR ;
    }
    return BLE_STATUS_SUCCESS;	
}

/*******************************************************************************
* Function Name  : Humidity_Update
* Description    : 更新湿度传感器数据
* Input          : humidity
* Output         : None
* Return         : None
*******************************************************************************/
tBleStatus Humidity_Update(uint16_t humidity)
{  
    tBleStatus ret;

    ret = aci_gatt_update_char_value(envSensServHandle, humidityCharHandle, 0, 2,
                                    (uint8_t*)&humidity);

    if (ret != BLE_STATUS_SUCCESS)
    {
        #ifdef Debug_Sensor_Humidity
            DLog("Error while updating TEMP characteristic.\n") ;
        #endif
        return BLE_STATUS_ERROR ;
    }
    return BLE_STATUS_SUCCESS;  
}

