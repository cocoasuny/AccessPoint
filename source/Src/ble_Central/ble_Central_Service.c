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
#include "ble_Central_service.h"


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


/* Private function declare */
static void Reset_bleMasterConnectList_ALL(void);

/**
   * @brief ble_CentralService_Init初始化
   * @param  None
   * @retval None
   */
tBleStatus ble_CentralService_Init(void)
{
	tBleStatus ret=0;
	
	Reset_bleMasterConnectList_ALL();
	ret = ble_Central_Add_Acc_Service();
	
	return ret;
}	

/**
 * @brief  GAP_Discovery_Service,找到当前连接所包含的Service
 * @param  conn_handle
 * @retval status
 */
tBleStatus GAP_Discovery_Service(uint16_t conn_handle)
{
	tBleStatus ret;
	
	/* discover all primary services on the server */
	ret = aci_gatt_disc_all_prim_services(conn_handle);
	bleMasterGattProcedure = Blue_Gatt_Procedure_Discovery_Service;    // Discovering service status
    
	#ifdef Debug_BlueNRG_Scan
	printf("Discovery Service :");
	if (ret != BLE_STATUS_SUCCESS) 
	{
		printf("aci_gatt_disc_all_prim_services failed: 0x%02x\r\n", ret);
	}
	else 
	{
		printf("aci_gatt_disc_all_prim_services  OK\r\n");
	}
	#endif
	
	return ret;	
}

/**
 * @brief  GAP_Discovery_Characteristics,找到当前连接所有Service所包含的characteristics
 * @param  conn_handle
 * @retval status
 */
tBleStatus GAP_Discovery_Characteristics(uint16_t conn_handle)
{
    uint8_t Location = 0;
    tBleStatus ret = BLE_STATUS_ERROR;
    
	bleMasterGattProcedure = Blue_Gatt_Procedure_Discovery_Character;  // Discovering character status
	
    GetMasterConnectListLocationFromHandle(&Location,conn_handle);

    if(
		(bleMasterConnectList[Location].bleCentralAccService.isServiceValid == true) &&
		(bleMasterConnectList[Location].bleCentralAccService.isServiceCharacterFindComplete != true)
	)   //是否完成三轴服务所有Character的查找
    {
		/* 查找三轴服务中所有Character */
        ret = aci_gatt_disc_all_charac_of_serv(bleMasterConnectList[Location].connHandle,
                                                bleMasterConnectList[Location].bleCentralAccService.serviceHandle,
                                                bleMasterConnectList[Location].bleCentralAccService.endGroupHandle
                                               ); 
    }
//	else if(
//	
//	)  //已完成上一个服务所有Character的查找，查找下一个服务的Character
//	{
//		
//	}
	else  //所有服务的Character已完成查找,进入Enable Noticification状态
	{
		bleMasterGattProcedure = Blue_Gatt_Procedure_Enable_Noticification;  // Enable Noticification status
		
		GAP_Discovery_AllCharacter_Complete_CB(conn_handle);  //所有Character查找完成后，使能各Character Noticification
	}
    
    return ret;
}
/**
 * @brief  GAP_Discovery_Characteristics_CB,找到当前连接characteristics CB,处理发现Character结果
 * @param  conn_handle
 * @retval status
 */
tBleStatus GAP_Discovery_Characteristics_CB(evt_att_read_by_type_resp *pdata)
{
    /* evt_att_read_by_type_resp parameters:
        pr->conn_handle: connection handle; 
        pr->event_data_length: total length of the event data; 
        pr->handle_value_pair_length: length of each specific data 
        within the handle_value_pair[];
        pr->handle_value_pair[]: event data.
    */  
    
    tBleStatus 		ret = BLE_STATUS_ERROR;
	uint8_t 		Location = 0;
    uint8_t 		i = 0;
	uint16_t 		attribut_handle = 0;
	uint16_t        value_handle = 0;
	uint8_t         properties = 0;
	uint8_t         uuid[16] = {0};
    
	#ifdef Debug_Ble_Central_Service
		printf("\r\nDiscovery Character:\r\n");
		printf("    conn handle:0x%04x\r\n",pdata->conn_handle);
		printf("    event_data_length:%d\r\n",pdata->event_data_length);
		printf("    handle_value_pair_length:%d\r\n",pdata->handle_value_pair_length);
		printf("data:");
		for(i=0;i<pdata->handle_value_pair_length;i++)
		{
			printf("0x%x,",pdata->handle_value_pair[i]);
		}
		printf("\r\n");
	#endif

	/* 处理Character数据 */
	attribut_handle = (uint16_t)(pdata->handle_value_pair[1]<<8 | pdata->handle_value_pair[0]);
	properties = pdata->handle_value_pair[2];
	value_handle = (uint16_t)(pdata->handle_value_pair[4]<<8 | pdata->handle_value_pair[3]);
	memcpy(uuid,pdata->handle_value_pair+5,sizeof(uuid));
	
	GetMasterConnectListLocationFromHandle(&Location,pdata->conn_handle);
	if(pdata->conn_handle == bleMasterConnectList[Location].connHandle)  //判断属于哪个连接
	{
		/******************  Acc服务处理开始 ****************************************/
		/* 判断是否是Acc Service Free Fall Character*/
		if(memcmp(uuid,bleMasterConnectList[Location].bleCentralAccFreeFallCharacter.uuid ,sizeof(uuid)) == 0)  //都是可以确认长度的数据，因此，可以这样做
		{
			//uuid是bleCentralAccService free fall character
			bleMasterConnectList[Location].bleCentralAccFreeFallCharacter.properties = properties;
			bleMasterConnectList[Location].bleCentralAccFreeFallCharacter.handle = attribut_handle;
			bleMasterConnectList[Location].bleCentralAccFreeFallCharacter.valueHandle = value_handle;
			bleMasterConnectList[Location].bleCentralAccFreeFallCharacter.isCharacterValid = true;
			bleMasterConnectList[Location].bleCentralAccFreeFallCharacter.isCharacterNoticificationEnable = false;
		}
		/* 判断是否是Acc加速度值发送Character */
		else if(memcmp(uuid,bleMasterConnectList[Location].bleCentralAccCharacter.uuid ,sizeof(uuid)) == 0)  //都是可以确认长度的数据，因此，可以这样做
		{
			//uuid是bleCentralAccService data characeter
			bleMasterConnectList[Location].bleCentralAccCharacter.properties = properties;
			bleMasterConnectList[Location].bleCentralAccCharacter.handle = attribut_handle;
			bleMasterConnectList[Location].bleCentralAccCharacter.valueHandle = value_handle;
			bleMasterConnectList[Location].bleCentralAccCharacter.isCharacterValid = true;
			bleMasterConnectList[Location].bleCentralAccCharacter.isCharacterNoticificationEnable = false;
		}
		
		/* Acc服务中所有Character都有效时，说明已完成Acc服务中所有Character查找 */
		if(
			(bleMasterConnectList[Location].bleCentralAccFreeFallCharacter.isCharacterValid == true) &&
			(bleMasterConnectList[Location].bleCentralAccCharacter.isCharacterValid == true)
		)  
		{
			bleMasterConnectList[Location].bleCentralAccService.isServiceCharacterFindComplete = true;
		}
		/************************Acc 服务处理结束 **********************************************/
	}
    
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
    uint8_t i = 0;

    uint8_t uuid[16];

    /* add service */
    CENTRAL_COPY_ACC_SERVICE_UUID(uuid);
    for(i=0;i<MAX_SUPPORT_CONNECT_NBR;i++)
    {
        memcpy(bleMasterConnectList[i].bleCentralAccService.uuid,uuid,sizeof(uuid));
        bleMasterConnectList[i].bleCentralAccService.endGroupHandle = 0;
        bleMasterConnectList[i].bleCentralAccService.serviceHandle = 0;
        bleMasterConnectList[i].bleCentralAccService.isServiceValid = false;
		bleMasterConnectList[i].bleCentralAccService.isServiceCharacterFindComplete = false;
    }
	
	/* add acc service free fall character */
	CENTRAL_COPY_FREE_FALL_UUID(uuid);
    for(i=0;i<MAX_SUPPORT_CONNECT_NBR;i++)
    {
        memcpy(bleMasterConnectList[i].bleCentralAccFreeFallCharacter.uuid,uuid,sizeof(uuid));
        bleMasterConnectList[i].bleCentralAccFreeFallCharacter.properties = 0;
        bleMasterConnectList[i].bleCentralAccFreeFallCharacter.handle = 0;
		bleMasterConnectList[i].bleCentralAccFreeFallCharacter.valueHandle = 0;
        bleMasterConnectList[i].bleCentralAccFreeFallCharacter.isCharacterValid = false;
		bleMasterConnectList[i].bleCentralAccFreeFallCharacter.isCharacterNoticificationEnable = false;
    }	
	
	/* add acc service data character */
	CENTRAL_COPY_ACC_UUID(uuid);
    for(i=0;i<MAX_SUPPORT_CONNECT_NBR;i++)
    {
        memcpy(bleMasterConnectList[i].bleCentralAccCharacter.uuid,uuid,sizeof(uuid));
        bleMasterConnectList[i].bleCentralAccCharacter.properties = 0;
        bleMasterConnectList[i].bleCentralAccCharacter.handle = 0;
		bleMasterConnectList[i].bleCentralAccCharacter.valueHandle = 0;
        bleMasterConnectList[i].bleCentralAccCharacter.isCharacterValid = false; 
		bleMasterConnectList[i].bleCentralAccCharacter.isCharacterNoticificationEnable = false;
    }
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
    uint8_t Location = 0;
	
	/* for debug */
	#ifdef Debug_Ble_Central_Service
		uint8_t i=0;
		printf("\r\nDiscovery Services:\r\n");
		printf("    conn_handle:0x%04x\r\n",pdata->conn_handle);
		printf("    event_data_length:%d\r\n",pdata->event_data_length);
		printf("    attribute_data_length:%d\r\n",pdata->attribute_data_length);
		for(i=0;i<pdata->event_data_length;i++)
		{
			printf("0x%x,",pdata->attribute_data_list[i]);
		}
		printf("\r\n");
    #endif			
	
	if(pdata->attribute_data_length > 16)  //目前只识别128bit的UUID
	{
		memcpy(uuid,pdata->attribute_data_list+4,sizeof(uuid));
//		for(i=0;i<sizeof(uuid);i++)
//		{
//			uuid[i] = pdata->attribute_data_list[i+4];
//		}
	}
	
    
    GetMasterConnectListLocationFromHandle(&Location,pdata->conn_handle);
	//判断该服务属于哪个Connect
	if(pdata->conn_handle == bleMasterConnectList[Location].connHandle)
	{
		/* 判断是否是Acc Service */
		if(memcmp(uuid,bleMasterConnectList[Location].bleCentralAccService.uuid,sizeof(uuid)) == 0)  //都是可以确认长度的数据，因此，可以这样做
		{
			//uuid是bleCentralAccService
			bleMasterConnectList[Location].bleCentralAccService.serviceHandle = (uint16_t)(pdata->attribute_data_list[1]<<8 
                                                                                           | pdata->attribute_data_list[0]);
			bleMasterConnectList[Location].bleCentralAccService.endGroupHandle = (uint16_t)(pdata->attribute_data_list[3]<<8 
                                                                                           | pdata->attribute_data_list[2]);
			bleMasterConnectList[Location].bleCentralAccService.isServiceValid = true;
			bleMasterConnectList[Location].bleCentralAccService.isServiceCharacterFindComplete = false;
		}
	}
}
/**
 * @brief  GAP_Discovery_Service_Complete_CB,找到当前连接包含的所有Service完成后CB
 * @param  *pdata
 * @retval void
 */
void GAP_Discovery_Service_Complete_CB(evt_gatt_procedure_complete *pdata)
{
     /* evt_gatt_procedure_complete parameters:
         pr->conn_handle: connection handle; 
         pr->attribute_data_length: length of the event data; 
         pr->data[]: event data.
    */
    GAP_Discovery_Characteristics(pdata->conn_handle);
}
/**
 * @brief  GAP_Discovery_Character_Complete_CB,找到当前服务中Character完成后CB
 * @param  *pdata
 * @retval void
 */
void GAP_Discovery_AllCharacter_Complete_CB(uint16_t conn_handle)
{
	#ifdef Debug_Ble_Central_Service
		printf("\r\nAll Character Discovery complete\r\n");
	#endif
	
//	/* Enable Charcter Descriptor for noticification */
//	Enable_Character_Descriptor_ForNoticification(conn_handle);  //调试时屏蔽，实际使用中，发现完所有Character后，自动开始Enable
}
/**
 * @brief  Enable_Character_Descriptor_ForNoticification,使能Character Notification属性
 * @param  conn_handle
 * @retval void
 */
void Enable_Character_Descriptor_ForNoticification(uint16_t conn_handle)
{
	 /* evt_gatt_procedure_complete parameters:
		 pr->conn_handle: connection handle; 
		 pr->data_length: < Length of error_code field (always 1). 
		 pr->error_code: Indicates whether the procedure completed with error (BLE_STATUS_FAILED) 
						or was successful (BLE_STATUS_SUCCESS).
	*/
	uint8_t 			Location = 0;
	tBleStatus 			ret;
	
	//for debug
	#ifdef Debug_Ble_Central_Service
		printf("\r\nDiscovery Character Complete\r\n");
		printf("    conn_handle:0x%04x\r\n",conn_handle);
		printf("\r\n");
	#endif
	
	/* 使能notification属性的Character */
	GetMasterConnectListLocationFromHandle(&Location,conn_handle);
	
	if(conn_handle == bleMasterConnectList[Location].connHandle)
	{
		/* 判断Acc三轴服务是否有效*/
		if(bleMasterConnectList[Location].bleCentralAccService.isServiceValid == true)
		{
			/* 判断Acc服务AccCharacter是否有效 */
			if(
				(bleMasterConnectList[Location].bleCentralAccCharacter.isCharacterValid == true) && 
				(bleMasterConnectList[Location].bleCentralAccCharacter.isCharacterNoticificationEnable != true)
			)   //未使能AccCharacter Notification
			{
				/* 使能AccCharacter notification */
				ret = aci_gatt_write_charac_descriptor(
														bleMasterConnectList[Location].connHandle,
														bleMasterConnectList[Location].bleCentralAccCharacter.valueHandle+1,
														0x02, //Length of the value to be written
														(uint8_t *)0x01 //attribute value: 1 for notification
														);
				if(ret != BLE_STATUS_SUCCESS)
				{
					#ifdef Debug_Ble_Central_AccService
						printf("Enable Acc Character notification Fail:0x%x\r\n",ret);
					#endif
				}
				else
				{
					#ifdef Debug_Ble_Central_AccService
						printf("Enable Acc Character notification OK\r\n");
					#endif
					bleMasterConnectList[Location].bleCentralAccCharacter.isCharacterNoticificationEnable = true;
				}
			}
			/* 判断Acc服务Free Fall Character是否有效 */
			else if(
					(bleMasterConnectList[Location].bleCentralAccFreeFallCharacter.isCharacterValid == true) &&
					(bleMasterConnectList[Location].bleCentralAccFreeFallCharacter.isCharacterNoticificationEnable != true)
			)
			{//未使能Acc Free Fall Character nification
				/* 使能Acc Free Fall Character noticification */
				ret = aci_gatt_write_charac_descriptor(
														bleMasterConnectList[Location].connHandle,
														bleMasterConnectList[Location].bleCentralAccFreeFallCharacter.valueHandle+1,
														0x02, //Length of the value to be written
														(uint8_t *)0x01 //attribute value: 1 for notification
														);
				if(ret != BLE_STATUS_SUCCESS)
				{
					#ifdef Debug_Ble_Central_AccService
						printf("Enable Acc Free Fall Character notification Fail:0x%x\r\n",ret);
					#endif
				}
				else
				{
					bleMasterConnectList[Location].bleCentralAccFreeFallCharacter.isCharacterNoticificationEnable = true;
					#ifdef Debug_Ble_Central_AccService
						printf("Enable Acc Free Fall Character notification OK\r\n");
					#endif					
				}
			}
//			else if()
//			{
//				下一个服务的Character
//			}	
			else  //所有服务需要Notification的Character都已Enable完成
			{
				bleMasterGattProcedure = Blue_Gatt_Procedure_Default;  //Default status
			}
		}
		#ifdef Debug_Ble_Central_AccService
		else
		{
			printf("bleCentralAccService Invalid\r\n");
		}
		#endif
	}
	#ifdef Debug_Ble_Central_AccService
	else
	{
		printf("connHandle Invalid\r\n");
	}
	#endif	
}
/**
 * @brief  GATT_AttributeData_Noticification_CB,Gatt收到Peripheral端发送的Noticification数据时调用
 * @param  *pdata
 * @retval void
 */
tBleStatus GATT_AttributeData_Noticification_CB(evt_gatt_attr_notification *pdata)
{
	/* notification event parameters:
		evt->conn_handle: the connection handle which notified the attribute;
		evt->event_data_length: length of attribute value + handle (2 bytes); 
		evt->attr_handle: attribute handle;
		evt->attr_value: pointer to attribute value (length is event_data_length – 2).   
	*/
	
	tBleStatus ret = 0;
	
	//for debug
	#ifdef Debug_Ble_Central_Service
//		uint8_t i = 0;
		printf("\r\nNoticification data:\r\n");
//		printf("    conn_handle:0x%04x\r\n",pdata->conn_handle);
//		printf("    attr_handle:0x%04x\r\n",pdata->attr_handle);
//		for(i=0;i<(pdata->event_data_length-2);i++)
//		{
//			printf("0x%x,",pdata->attr_value[i]);
//		}
//		printf("\r\n");
    #endif		
		
	return ret;
}

/**
 * @brief  GetMasterConnectListLocationFromHandle,通过conn_handle查找所连接的设备地址
 * @param  uint8_t *Location,uint16_t conn_handle
 * @retval status
 */
tBleStatus GetMasterConnectListLocationFromHandle(uint8_t *Location,uint16_t conn_handle)
{
    uint8_t i = 0;
    tBleStatus ret = 0;
    
    for(i= 0;i<MAX_SUPPORT_CONNECT_NBR;i++)
    {
        if(conn_handle == bleMasterConnectList[i].connHandle)
        {
            *Location = i;
            break;
        }
    }
    if(i<=MAX_SUPPORT_CONNECT_NBR)
    {
        ret = BLE_STATUS_SUCCESS;
    }
    else
    {
        ret = BLE_STATUS_ERROR;
    }
    
    return ret;
}

/**
 * @brief  GetMasterConnectListLocationFromHandle,通过conn_handle查找所连接的设备地址
 * @param  uint8_t *Location,uint16_t conn_handle
 * @retval status
 */
tBleStatus GetMasterConnectListLocationFromMACAddr(uint8_t *Location,uint8_t addr[6])
{
    uint8_t i = 0;
    tBleStatus ret = 0;
    
    for(i= 0;i<MAX_SUPPORT_CONNECT_NBR;i++)
    {
        if(memcmp(addr,bleMasterConnectList[i].bdaddr,BLE_MACADDR_LEN) == 0)
        {
            *Location = i;
            break;
        }
    }
    if(i<=MAX_SUPPORT_CONNECT_NBR)
    {
        ret = BLE_STATUS_SUCCESS;
    }
    else
    {
        ret = BLE_STATUS_ERROR;
    }
    
    return ret;
}

/**
* @brief  Reset_bleMasterConnectList_ALL,上电清楚所有连接信息
 * @param  None
 * @retval None
 */
static void Reset_bleMasterConnectList_ALL(void)
{
	uint8_t i = 0;
	for(i=0;i<MAX_SUPPORT_CONNECT_NBR;i++) //查找可以加入的位置
	{
		bleMasterConnectList[i].connHandle = 0;
		bleMasterConnectList[i].ble_status = DEFAULT;
		bleMasterConnectList[i].isListValid = true;
		memset(bleMasterConnectList[i].bdaddr,0,BLE_MACADDR_LEN);
		bleMasterConnectList[i].bleCentralAccService.isServiceValid = false;
		bleMasterConnectList[i].bleCentralAccService.isServiceCharacterFindComplete = false;
		bleMasterConnectList[i].bleCentralAccCharacter.isCharacterValid = false;
		bleMasterConnectList[i].bleCentralAccCharacter.isCharacterNoticificationEnable = false;
		bleMasterConnectList[i].bleCentralAccFreeFallCharacter.isCharacterValid = false;
		bleMasterConnectList[i].bleCentralAccFreeFallCharacter.isCharacterNoticificationEnable = false;
	}
}









