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


/* Typedefs -------------------------------------------------------------------*/
BLE_DEVICE_LIST_INFO_T bleScanList[MAX_SUPPORT_SCAN_NBR];


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
		printf("start scan OK\n");  
	}
	#endif		
}

/**
 * @brief  GAP_Scan_ADVData_CB,处理Central端扫描结果数据
 * @param  *pdata
 * @retval None
 */
void GAP_Scan_ADVData_CB(le_advertising_info *pdata)
{
	uint8_t i = 0;
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
			bleScanList[i].data_RSSI[0] = pdata->data_RSSI[0];
			bleScanList[i].ble_status = CONNECTABLE;
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

	LOG("Scan Dev(%d):  ID  MAC                       	Status  RSSI\r\n",validDevNum);
	for(i=0;i<validDevNum;i++)
	{	
		LOG("**************%d,  0x%x 0x%x 0x%x 0x%x 0x%x 0x%x    %d      %d\r\n",
				i,bleScanList[i].bdaddr[0],bleScanList[i].bdaddr[1],bleScanList[i].bdaddr[2],
				bleScanList[i].bdaddr[3],bleScanList[i].bdaddr[4],bleScanList[i].bdaddr[5],
				bleScanList[i].ble_status,bleScanList[i].data_RSSI[0]					
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
		pScanList[i].data_RSSI[0] = 0;
		memset(pScanList[i].bdaddr,0,BLE_MACADDR_LEN);
		pScanList[i].isValid = true;
	}
}




