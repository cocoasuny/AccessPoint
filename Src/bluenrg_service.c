/**
  ******************************************************************************
  * @file    sensor_service.c
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
#include "bluenrg_service.h"
#include "main.h"
#include "hal_types.h"
#include "bluenrg_gatt_server.h"
#include "bluenrg_gap.h"
#include "string.h"
#include "bluenrg_gap_aci.h"
#include "bluenrg_gatt_aci.h"
#include "hci_const.h"
#include "gp_timer.h"
#include "bluenrg_hal_aci.h"
#include "bluenrg_aci_const.h"
#include "hci.h"
#include "hal.h"
#include "sm.h"
#include "debug.h"
#include <stdlib.h>
#include "bsp_hum_temp.h"
#include "bsp_pressure.h"


/* Ble parameters define */
const char *devicename = "BlueNRG";    //DeviceMaxName:10
uint8_t g_tx_power_level = 6;
uint16_t service_handle, dev_name_char_handle, appearance_char_handle;
volatile uint16_t connection_handle = 0;

/* Private function prototypes -----------------------------------------------*/


/**
   * @brief BlueNRG初始化
   * @param  None
   * @retval None
   */
void BlueNRG_Init(void)
{
#ifdef Debug_BlueNRF
    uint8_t  hwVersion;
    uint16_t fwVersion;
#endif

    /* Initialize the BlueNRG SPI driver */
    BNRG_SPI_Init();

    /* Initialize the BlueNRG HCI */
    HCI_Init();

    /* Reset BlueNRG hardware */
    BlueNRG_RST();

#ifdef Debug_BlueNRF
    /* get the BlueNRG HW and FW versions */
    getBlueNRGVersion(&hwVersion, &fwVersion);
    DLog("HWver %x, FWver %x\r\n", hwVersion, fwVersion);
#endif
    
    /* Advertising Init */
    Advertising_Init();

    /* Service Init */
    Service_Init();

    /* Start Advertise */
    Start_Advertise();
}

/**
   * @brief Advertise 初始化
   * @param None
   * @retval None
   */
void Advertising_Init(void)
{
  /* 
   * Reset BlueNRG again otherwise we won't
   * be able to change its MAC address.
   * aci_hal_write_config_data() must be the first
   * command after reset otherwise it will fail.
   */
    BlueNRG_RST();  
    Ble_AdvAddress_Set();
}

/**
   * @brief Services初始化
   * @param  None
   * @retval None
   */
tBleStatus Service_Init(void)
{
    tBleStatus ret;

    /*gatt_Init*/
    ret = aci_gatt_init();

    if(ret)
    {
        return BLE_GATT_INIT_FAILED;
    }

    ret = aci_gap_init(GAP_PERIPHERAL_ROLE, 0, 0x07, &service_handle, &dev_name_char_handle, &appearance_char_handle);

#ifdef Debug_BlueNRF
    if(ret != BLE_STATUS_SUCCESS)
    {
        DLog("GAP_Init failed...0x%x\r\n",ret);
    }
#endif

    if(ret != BLE_STATUS_SUCCESS)
    {
        return BLE_GAP_INIT_FAILED;
    }
	
    ret = aci_gap_set_auth_requirement(MITM_PROTECTION_REQUIRED,
                                       OOB_AUTH_DATA_ABSENT,
                                       NULL,
                                       7,
                                       16,
                                       USE_FIXED_PIN_FOR_PAIRING,
                                       123456,
                                       BONDING);
#ifdef Debug_BlueNRF
    if (ret == BLE_STATUS_SUCCESS)
    {
        printf("BLE Stack Initialized.\r\n");
    }
#endif

    if (ret != BLE_STATUS_SUCCESS)
    {
        return BLE_STACK_INIT_FAILED;
    }

	/**********  add  SERVICEs ***********/
	Add_Acc_Service();
    Add_Environmental_Sensor_Service();
	Add_RemoteControl_Service();
	
    return ret;
}
/**
   * @brief 开始广播
   * @param None
   * @retval None
   */
tBleStatus Start_Advertise(void)
{
    tBleStatus ret;
    char local_name[DeviceMaxName];
    uint8_t  device_name_len = 0,local_name_len = 0;
    
	ret = aci_gatt_update_char_value(service_handle, dev_name_char_handle, 0,
                                   strlen(devicename), (uint8_t *)devicename);  
    
    /* Set output power level */
	ret = Ble_SetTx_Power(g_tx_power_level);
    
	/* disable scan response */
	hci_le_set_scan_resp_data(0,NULL);
	
    /* Covern devicename to local_name */
    device_name_len = strlen(devicename);
    local_name_len = device_name_len + 1;
    local_name[0] = AD_TYPE_COMPLETE_LOCAL_NAME;
    memcpy(local_name+1,devicename,device_name_len);
    
	ret = aci_gap_set_discoverable(ADV_IND, 0, 0, PUBLIC_ADDR, NO_WHITE_LIST_USE,
                                 local_name_len, local_name, 0, NULL, 0, 0);  

    return ret;
}
/**
   * @brief Set the transmit power
   * @param level: the power
   * @retval None
   */
tBleStatus Ble_SetTx_Power(uint8_t level)
{
    tBleStatus ret;
    
    /* Set output power level */
    ret = aci_hal_set_tx_power_level(1,level);

    return ret;
}
/**
   * @brief set the advertise address
   * @param None 
   * @retval None
   */
tBleStatus Ble_AdvAddress_Set(void)
{ 
	uint8_t bdaddr[] = {0x12, 0x34, 0x00, 0xE1, 0x80, 0x02};
	tBleStatus ret = BLE_STATUS_ERROR;
		
	ret = aci_hal_write_config_data(CONFIG_DATA_PUBADDR_OFFSET,
							  CONFIG_DATA_PUBADDR_LEN,
							  bdaddr);
#ifdef Debug_BlueNRF
    if(ret != BLE_STATUS_SUCCESS)
    {
        DLog("Device Address failed...0x%x\r\n",ret);
    }
#endif

    return ret;
}

/**
   * @brief handle the read request
   * @param handle: the handle
   * @retval None
   */
static void Read_Request_CB(uint16_t handle)
{
    //获取handle
	if(handle == accCharHandle + 1)
    {
        BlueNRG_Update_Acc((AxesRaw_t*)&g_Axes_data);
    }
    else if(handle == tempCharHandle + 1)
    {
        int16_t data;
        float fTmp = 0;       
//        Acc_Update((AxesRaw_t*)&axes_data); //FIXME: to overcome issue on Android App
//                                    // If the user button is not pressed within
//                                    // a short time after the connection,
//                                    // a pop-up reports a "No valid characteristics found" error.
        /* Get Temperature */
        BSP_HUM_TEMP_GetTemperature(&fTmp);
        data = (uint16_t)(fTmp*10);
        Temp_Update(data);
    }
    else if(handle == pressCharHandle + 1)
    {
        int32_t data;
        float fPress = 0;
        
        /* Get pressure */
        BSP_PRESSURE_GetPressure(&fPress);
        data = (uint32_t)(fPress*100);
        Press_Update(data);
    }
    else if(handle == humidityCharHandle + 1)
    {
        uint16_t data;
        float fHum = 0;
 
        /* Get Humidity */
        BSP_HUM_TEMP_GetHumidity(&fHum);
        data = (uint16_t)(fHum*10);        
        Humidity_Update(data);
    } 

    
    
    //Exit:
    if(connection_handle != 0) 
    {
        aci_gatt_allow_read(connection_handle);
    }
}

/**
 * @brief  This function is called when there is a LE Connection Complete event.
 * @param  uint8_t Address of peer device
 * @param  uint16_t Connection handle
 * @retval None
 */
void GAP_ConnectionComplete_CB(uint8_t addr[6], uint16_t handle)
{  
//  connected = TRUE;
  connection_handle = handle;
  
//  PRINTF("Connected to device:");
//  for(int i = 5; i > 0; i--){
//    PRINTF("%02X-", addr[i]);
//  }
//  PRINTF("%02X\n", addr[0]);
}
/**
 * @brief  This function is called when the peer device gets disconnected.
 * @param  None 
 * @retval None
 */
void GAP_DisconnectionComplete_CB(void)
{
//  connected = FALSE;
//  PRINTF("Disconnected\n");
//  /* Make the device connectable again. */
//  set_connectable = TRUE;
//  notification_enabled = FALSE;
    Start_Advertise();
}
/**
 * @brief  This function is called attribute value corresponding to 
 *         ledButtonCharHandle characteristic gets modified.
 * @param  Handle of the attribute
 * @param  Size of the modified attribute data
 * @param  Pointer to the modified attribute data
 * @retval None
 */
void Attribute_Modified_CB(uint16_t handle, uint8_t data_length, uint8_t *att_data, uint8_t offset)
{
	uint8_t i = 0;
	
	/* If GATT client has modified 'LED Control characteristic' value, toggle LED2 */
	if(handle == ledControlCharHandle + 1)
	{   
		g_LedFlashTime = att_data[0]*256 + att_data[1];
		#ifdef Debug_LedControl
			printf("remote control:%d,%d\r\n",data_length,g_LedFlashTime);
			for(i=0;i<data_length;i++)
			{
				printf("0x%x,",att_data[i]);
			}
			printf("\r\n");
		#endif
	}
}
/**
 * @brief  Callback processing the ACI events.
 * @note   Inside this function each event must be identified and correctly
 *         parsed.
 * @param  void* Pointer to the ACI packet
 * @retval None
 */
void HCI_Event_CB(void *pckt)
{
	hci_uart_pckt *hci_pckt = pckt;
	
	/* obtain event packet */
	hci_event_pckt *event_pckt = (hci_event_pckt*)hci_pckt->data;

	if(hci_pckt->type != HCI_EVENT_PKT)
	return;

	switch(event_pckt->evt)
	{
		case EVT_DISCONN_COMPLETE:
		{
			GAP_DisconnectionComplete_CB();
		}
		break;

		case EVT_LE_META_EVENT:
		{
			evt_le_meta_event *evt = (void *)event_pckt->data;

			switch(evt->subevent)
			{
				case EVT_LE_CONN_COMPLETE:
				{
					evt_le_connection_complete *cc = (void *)evt->data;
					GAP_ConnectionComplete_CB(cc->peer_bdaddr, cc->handle);
				}
				break;
			}
		}
		break;

		case EVT_VENDOR:
		{
			evt_blue_aci *blue_evt = (void*)event_pckt->data;
			switch(blue_evt->ecode)
			{
				case EVT_BLUE_GATT_ATTRIBUTE_MODIFIED:         
				{
					/* this callback is invoked when a GATT attribute is modified
					extract callback data and pass to suitable handler function */
					
					evt_gatt_attr_modified *evt = (evt_gatt_attr_modified*)blue_evt->data;
					Attribute_Modified_CB(evt->attr_handle, evt->data_length, evt->att_data,evt->offset); 
				}
				break; 

				case EVT_BLUE_GATT_READ_PERMIT_REQ:
				{
					evt_gatt_read_permit_req *pr = (void*)blue_evt->data;                    
					Read_Request_CB(pr->attr_handle);                    
				}
				break;
			}
		}
		break;
	} 
}




