/**
  ******************************************************************************
  * File Name          : ble_Central_Connect.c
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


/* Global variables declare */

/* Typedefs -------------------------------------------------------------------*/
BLE_DEVICE_LIST_INFO_T                  bleScanList[MAX_SUPPORT_SCAN_NBR];
BLE_DIRECTCONNECT_PARAMT_T              uxBleDirectConnectParamt;
BLE_MASTER_CONNECT_CONTEXT_INFO_T       bleMasterConnectList[MAX_SUPPORT_CONNECT_NBR];
BLE_MASTER_GATT_PROCEDURE_T				bleMasterGattProcedure = Blue_Gatt_Procedure_Default;

/* Private function declare ---------------------------------------------------*/
static void  Reset_Discovery_List(BLE_DEVICE_LIST_INFO_T *pScanList, uint8_t len);

/**
 * @brief  Start_Scan_Procedure,开启一次扫描设备
 * @param  None
 * @retval None
 */
void Start_Scan_Procedure(void)
{
	tBleStatus  ret;
	
	/* Rest the Discovery List */
	Reset_Discovery_List(bleScanList,MAX_SUPPORT_SCAN_NBR);
	
	/* Start scan */
	ret = aci_gap_start_general_discovery_proc(0x10, 0x10, 
												 0x00, /* public address */
												 0x00);
	#ifdef Debug_BlueNRG_Scan
	if (ret != BLE_STATUS_SUCCESS)
	{
		printf("start scan failed: 0x%x\n",ret);
	}
	else 
	{
		printf("start scaning\n");  
	}
	#endif		
}
/**
 * @brief  Stop_Scan_Procedure,停止扫描设备
 * @param  None
 * @retval None
 */
tBleStatus Stop_Scan_Procedure(void)
{
	tBleStatus  ret;
	
	/* terminate gap procedure */
	ret = aci_gap_terminate_gap_procedure(0x02); // GENERAL_DISCOVERY_PROCEDURE
	
	#ifdef Debug_BlueNRG_Scan
	if (ret != BLE_STATUS_SUCCESS) 
	{
		printf("aci_gap_terminate_gap_procedure() failed: 0x%02x\n", ret);
	}
	else 
	{
		printf("aci_gap_terminate_gap_procedure() OK\n");
	}
	#endif
	
	return ret;
}
/**
 * @brief  GAP_Central_Make_Connection,连接设备
 * @param  None
 * @retval None
 */
tBleStatus GAP_Central_Make_Connection(tBDAddr addr)
{
	tBleStatus      ret;
	uint8_t         i = 0;
	bool            isAdded = true;  //是否需要加入新设备,false:不需要；true:需要
    
//	tBDAddr GAP_Peripheral_address = {0x12, 0x34, 0x00, 0xE1, 0x80, 0x02};
	/* Start the direct connection establishment procedure to the GAP
		peripheral device in general discoverable mode using the following
		connection parameters:
		Scan_Interval: 0x4000;
		Scan_Window: 0x4000;
		Peer_Address_Type: PUBLIC_ADDR (GAP peripheral address type: public
		address);
		Peer_Address: {0xaa, 0x00, 0x00, 0xE1, 0x80, 0x02};
		Own_Address_Type: PUBLIC_ADDR (device address type);
		Conn_Interval_Min: 40 (Minimum value for the connection event
		interval);
		Conn_Interval_Max: 40 (Maximum value for the connection event
		interval);
		Design an application using BlueNRG, BlueNRG-MS ACI APIs PM0237
		60/99 DocID027104 Rev 4
		Conn_Latency: 0 (Slave latency for the connection in a number of
		connection events);
		Supervision_Timeout: 60 (Supervision timeout for the LE Link);
		Conn_Len_Min: 2000 (Minimum length of connection needed for the LE
		connection);
		Conn_Len_Max: 2000 (Maximum length of connection needed for the LE
		connection).
	*/
    /* 将需要连接的设备Mac地址加入bleMasterConnectList */
	for(i=0;i<MAX_SUPPORT_CONNECT_NBR;i++)
	{
		if(memcmp(addr,bleMasterConnectList[i].bdaddr,BLE_MACADDR_LEN) == 0)  //需要连接的设备Mac地址在设备中
		{
			//在设备列表中
			isAdded = false;
			break;
		}
	}  
	if(isAdded == true) //需要将新需要连接的设备mac地址加入设备列表中
	{
		for(i=0;i<MAX_SUPPORT_CONNECT_NBR;i++) //查找可以加入的位置
		{
			if(bleMasterConnectList[i].isListValid == true)
			{
				break;
			}
		}

		if(i<MAX_SUPPORT_CONNECT_NBR)  //在最大支持设备列表数内
		{
			bleMasterConnectList[i].isListValid = false;
			bleMasterConnectList[i].ble_status = DEFAULT;
			memcpy(bleMasterConnectList[i].bdaddr,addr,BLE_MACADDR_LEN);
		}
	}    
	
	ret = aci_gap_create_connection(0x4000, 0x4000, PUBLIC_ADDR,
									addr, PUBLIC_ADDR, 40, 40, 0, 60, 2000 , 2000);
	#ifdef Debug_BlueNRG_Scan						
	if (ret != BLE_STATUS_SUCCESS) 
	{
		printf("aci_gap_create_connection Failed: 0x%02x\n\r\n",ret);
	}
	#endif
	
	return ret;
}
/**
 * @brief  GAP_Central_Make_Disconnection,GAP Central断开设备连接
 * @param  None
 * @retval None
 */
tBleStatus GAP_Central_Make_Disconnection(tBDAddr addr)
{
	tBleStatus  ret;
    uint8_t Location = 0;
	
    /* Get the addr location of bleMasterConnectList */
    GetMasterConnectListLocationFromMACAddr(&Location,addr);
    
	/* terminate gap procedure */
	ret = aci_gap_terminate(bleMasterConnectList[Location].connHandle,HCI_OE_USER_ENDED_CONNECTION); // Disconnect the peer device
	
	#ifdef Debug_BlueNRG_Scan
	printf("Disconnect :");
	if (ret != BLE_STATUS_SUCCESS) 
	{
		printf("aci_gap_terminate failed: 0x%02x\r\n", ret);
	}
	else 
	{
		printf("aci_gap_terminate  OK\r\n");
	}
	#endif
	
	return ret;
}

/**
 * @brief  GAP_Scan_ADVData_CB,处理Central端扫描结果数据
 * @param  *pdata
 * @retval None
 */
void GAP_Scan_ADVData_CB(le_advertising_info *pdata)
{
    /*  le_advertising_info parameters:
        pr->evt_type: event type (advertising packets types);
        pr->bdaddr_type: type of the peer address (PUBLIC_ADDR,RANDOM_ADDR);
        pr->bdaddr: address of the peer device found during scanning;
        pr->length: length of advertising or scan response data;
        pr->data_RSSI[]: length advertising or scan response data + RSSI.
        RSSI is last octect (signed integer).
    */    
	uint8_t i = 0;
//    uint8_t j = 0;
	bool    isAdded = true;  //是否需要加入新设备,false:不需要；true:需要

	for(i=0;i<MAX_SUPPORT_SCAN_NBR;i++)
	{
		if(memcmp(pdata->bdaddr,bleScanList[i].bdaddr,BLE_MACADDR_LEN) == 0)  //新扫描到的设备Mac地址在设备中
		{
			//在设备列表中
			isAdded = false;
			break;
		}
	}
	
	if(isAdded == true) //需要将新扫描到的设备mac地址加入设备列表中
	{
		for(i=0;i<MAX_SUPPORT_SCAN_NBR;i++) //查找可以加入的位置
		{
			if(bleScanList[i].isValid == true)
			{
				break;
			}
		}
		
		if(i<MAX_SUPPORT_SCAN_NBR)  //在最大支持设备列表数内
		{
			bleScanList[i].isValid = false;
			memcpy(bleScanList[i].bdaddr,pdata->bdaddr,BLE_MACADDR_LEN);
			bleScanList[i].data_RSSI = pdata->data_RSSI[pdata->data_length-1];
			bleScanList[i].ble_status = CONNECTABLE;
            
            /* for debug */
//            printf("Adv Data:");
//            for(j=0;j<pdata->data_length;j++)
//            {
//                printf("0x%x,",pdata->data_RSSI[j]);
//            }
//            printf("\r\n");
		}
	}
}
/**
 * @brief  GAP_Discovery_CompleteCB,扫描结束后，产生Discovery procedure terminated事件
 * @param  None
 * @retval None
 */
void GAP_Discovery_CompleteCB(void)
{
	uint8_t i = 0;
	uint8_t validDevNum = 0;
	#ifdef Debug_BlueNRG_Scan
		LOG("Scan Terminated\r\n");
	#endif

	//扫描结束，上报扫描到的设备信息
	for(i=0;i<MAX_SUPPORT_SCAN_NBR;i++)  //查找扫描到的设备数量
	{
		if(bleScanList[i].isValid != false)
		{
			break;
		}
	}
	
	validDevNum = i++;	

	LOG("Scan Dev(%d): ID  MAC                       	  Status  RSSI\r\n",validDevNum);
	for(i=0;i<validDevNum;i++)
	{	
		LOG("              %d,  0x%x 0x%x 0x%x 0x%x 0x%x 0x%x    %d      %d\r\n",
				i,bleScanList[i].bdaddr[0],bleScanList[i].bdaddr[1],bleScanList[i].bdaddr[2],
				bleScanList[i].bdaddr[3],bleScanList[i].bdaddr[4],bleScanList[i].bdaddr[5],
				bleScanList[i].ble_status,bleScanList[i].data_RSSI					
			);		
	}	
}

/**
 * @brief  Reset_Discovery_List
 * @param  *pScanList , len
 * @retval None
 */
static void  Reset_Discovery_List(BLE_DEVICE_LIST_INFO_T *pScanList, uint8_t len)
{
	uint8_t i = 0;
	
	for(i=0;i<len;i++)
	{
		pScanList[i].ble_status = DEFAULT;
		pScanList[i].data_RSSI = 0;
		memset(pScanList[i].bdaddr,0,BLE_MACADDR_LEN);
		pScanList[i].isValid = true;
	}
}

/**
* @brief  Reset_bleMasterConnectList_ByHandle,清除对应Handle的连接信息
 * @param  Handle
 * @retval None
 */
void Reset_bleMasterConnectList_ByHandle(uint16_t Handle)
{
    uint8_t Location = 0;
    
    GetMasterConnectListLocationFromHandle(&Location,Handle);
    
    bleMasterConnectList[Location].connHandle = 0;
    bleMasterConnectList[Location].ble_status = DEFAULT;
    bleMasterConnectList[Location].isListValid = true;
    memset(bleMasterConnectList[Location].bdaddr,0,BLE_MACADDR_LEN);
    bleMasterConnectList[Location].bleCentralAccService.isServiceValid = false;
	bleMasterConnectList[Location].bleCentralAccService.isServiceCharacterFindComplete = false;
    bleMasterConnectList[Location].bleCentralAccCharacter.isCharacterValid = false;
	bleMasterConnectList[Location].bleCentralAccCharacter.isCharacterNoticificationEnable = false;
    bleMasterConnectList[Location].bleCentralAccFreeFallCharacter.isCharacterValid = false;    
	bleMasterConnectList[Location].bleCentralAccFreeFallCharacter.isCharacterNoticificationEnable = false;    
}

