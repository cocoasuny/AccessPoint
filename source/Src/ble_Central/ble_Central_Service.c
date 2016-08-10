/**
  ******************************************************************************
  * File Name          : ble_Central_Service.c
  * Description        : Main program body
  ******************************************************************************
  *
  * COPYRIGHT(c) 2016 STMicroelectronics
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
/* Includes ------------------------------------------------------------------*/
#include "main.h"


/* accelerometer service and char UUID define */
#define CENTRAL_COPY_ACC_SERVICE_UUID(uuid_struct)  COPY_UUID_128(uuid_struct,0x02,0x36,0x6e,0x80, 0xcf,0x3a, 0x11,0xe1, 0x9a,0xb4, 0x00,0x02,0xa5,0xd5,0xc5,0x1b)
#define CENTRAL_COPY_FREE_FALL_UUID(uuid_struct)    COPY_UUID_128(uuid_struct,0xe2,0x3e,0x78,0xa0, 0xcf,0x4a, 0x11,0xe1, 0x8f,0xfc, 0x00,0x02,0xa5,0xd5,0xc5,0x1b)
#define CENTRAL_COPY_ACC_UUID(uuid_struct)          COPY_UUID_128(uuid_struct,0x34,0x0a,0x1b,0x80, 0xcf,0x4b, 0x11,0xe1, 0xac,0x36, 0x00,0x02,0xa5,0xd5,0xc5,0x1b)

/* Environmental Sensor service and char UUID define */
#define CENTRAL_COPY_ENV_SENS_SERVICE_UUID(uuid_struct)  COPY_UUID_128(uuid_struct,0x42,0x82,0x1a,0x40, 0xe4,0x77, 0x11,0xe2, 0x82,0xd0, 0x00,0x02,0xa5,0xd5,0xc5,0x1b)
#define CENTRAL_COPY_TEMP_CHAR_UUID(uuid_struct)         COPY_UUID_128(uuid_struct,0xa3,0x2e,0x55,0x20, 0xe4,0x77, 0x11,0xe2, 0xa9,0xe3, 0x00,0x02,0xa5,0xd5,0xc5,0x1b)
#define CENTRAL_COPY_PRESS_CHAR_UUID(uuid_struct)        COPY_UUID_128(uuid_struct,0xcd,0x20,0xc4,0x80, 0xe4,0x8b, 0x11,0xe2, 0x84,0x0b, 0x00,0x02,0xa5,0xd5,0xc5,0x1b)
#define CENTRAL_COPY_HUMIDITY_CHAR_UUID(uuid_struct)     COPY_UUID_128(uuid_struct,0x01,0xc5,0x0b,0x60, 0xe4,0x8c, 0x11,0xe2, 0xa0,0x73, 0x00,0x02,0xa5,0xd5,0xc5,0x1b)



/* Typedefs -------------------------------------------------------------------*/
BLE_CENTRAL_SERVICE_INFO_T		bleCentralAccService;         //Central设备三轴传感器服务

/**
   * @brief ble_CentralService_Init初始化
   * @param  None
   * @retval None
   */
tBleStatus ble_CentralService_Init(void)
{
	tBleStatus ret=0;
	
	ret = ble_Central_Add_Acc_Service();
	
	return ret;
}	

/**
   * @brief ble_Central_Add_Acc_Service,ble Central端添加三轴传感器服务
   * @param  None
   * @retval None
   */
tBleStatus ble_Central_Add_Acc_Service(void)
{
    tBleStatus ret=0;

    uint8_t uuid[16];

    /* add service */
    CENTRAL_COPY_ACC_SERVICE_UUID(uuid);
	memcpy(bleCentralAccService.uuid,uuid,sizeof(uuid));
	bleCentralAccService.endGroupHandle = 0;
	bleCentralAccService.serviceHandle = 0;
	bleCentralAccService.isServiceValid = false; 

	return 	ret;
}

/**
 * @brief  GAP_Discovery_Service_CB,找到当前连接所包含的Service cb
 * @param  *pdata
 * @retval void
 */
void GAP_Discovery_Service_CB(evt_att_read_by_group_resp *pdata)
{
	/* evt_att_read_by_group_resp parameters:
		pr->conn_handle: connection handle;
		pr->event_data_length: total length of the event data;
		pr->attribute_data_length: length of each specific data within the
		attribute_data_list[];
		pr->attribute_data_list[]: event data.
		   *  A list of Attribute Data where the attribute data is composed by:
		   *  @li 2 octets for Attribute Handle
		   *  @li 2 octets for End Group Handle
		   *  @li (attribute_data_length - 4) octets for Attribute Value
	PS: 实测event_data_length比attribute_data_length多1个字节，目前暂不知这多出的一个字节是干什么的？
	*/
	
    uint8_t uuid[16] = {0};
	
	/* for debug */
	uint8_t i=0;
	printf("\r\nDiscovery Services:\r\n");
	printf("    conn_handle:0x%04x\r\n",pdata->conn_handle);
	printf("	event_data_length:%d\r\n",pdata->event_data_length);
	printf("	attribute_data_length:%d\r\n",pdata->attribute_data_length);
	for(i=0;i<pdata->event_data_length;i++)
	{
		printf("0x%x,",pdata->attribute_data_list[i]);
	}
	printf("\r\n");
	
	if(pdata->attribute_data_length > 16)  //目前只识别128bit的UUID
	{
		memcpy(uuid,pdata->attribute_data_list+4,sizeof(uuid));
//		for(i=0;i<sizeof(uuid);i++)
//		{
//			uuid[i] = pdata->attribute_data_list[i+4];
//		}
	}
	
	//判断该服务属于哪个Connect
	if(pdata->conn_handle == connection_handle)
	{
		if(memcmp(uuid,bleCentralAccService.uuid,sizeof(uuid)) == 0)  //都是可以确认长度的数据，因此，可以这样做
		{
			//uuid是bleCentralAccService
			bleCentralAccService.serviceHandle = (uint16_t)(pdata->attribute_data_list[1]<<8 | pdata->attribute_data_list[0]);
			bleCentralAccService.endGroupHandle = (uint16_t)(pdata->attribute_data_list[3]<<8 | pdata->attribute_data_list[2]);
			bleCentralAccService.isServiceValid = true;
		}
	}
}
















