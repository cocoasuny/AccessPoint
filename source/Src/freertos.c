/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
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
#include "FreeRTOS.h"
#include "task.h"
#include "freertostask.h"

/**
  * @brief  BlueNRG HCI task handler
  * @param  pvParameters
  * @retval None
  */
void BlueNRGHCITask(void *pvParameters)
{   	
    while(1)
    {
        /* Process any pending HCI events read */
        HCI_Process();
        vTaskDelay(100);
    }
}

/**
  * @brief  Update the IMU parameters to PC through USART
  * @param  pvParameters
  * @retval None
  */
void IMUDataUpdateToPCTask(void *pvParameters)
{
	ANO_DT_Send_Version(4,300,100,400,0);     //Send the Version 
    
    while(1)
    {
		//for test 
		g_AttitudeInfo.roll += 10;
		g_AttitudeInfo.pitch += 10;
		g_AttitudeInfo.yaw +=10;
		g_AttitudeInfo.altitude +=10;
		g_AttitudeInfo.fly_mode = 1;
		g_AttitudeInfo.armed = 1;
		
		g_Acc.AXIS_X += 100;
		g_Acc.AXIS_Y += 100;
		g_Acc.AXIS_Z += 100;
		g_Gyro.AXIS_X += 100;
		g_Gyro.AXIS_Y += 100;
		g_Gyro.AXIS_Z += 100;
		g_Mag.AXIS_X += 100;
		g_Mag.AXIS_Y += 100;
		g_Mag.AXIS_Z += 100;
		
        ANO_DT_Data_Exchange();
        vTaskDelay(100);     //IMU 参数更新频率
    }
}


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
