/**
  ******************************************************************************
  * File Name          : Log.c
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


/**
  * @brief  创建Log文件
  * @param  None
  * @retval 是否创建成功         
  */
uint8_t Log_Creat(void)
{
    RTC_DateTypeDef date_s;
    RTC_TimeTypeDef rtc_time;
    char fileName[20] = "a.txt";
    uint8_t status = false;
    FRESULT ret = FR_NO_FILE;
    
    /* 以时间信息为文件名称 */
    Calendar_Get(&date_s,&rtc_time);
    sprintf(fileName,"%d%d%d%d.log",20,date_s.Year,date_s.Month,date_s.Date);

    /*##-3- Create and Open a new text file object with write access #####*/
    ret = f_open(&MyFile, fileName, FA_CREATE_ALWAYS | FA_WRITE);
    if(ret != FR_OK)
    {
        /* Log file Open for write Error */
        #ifdef Debug_Log
            printf("Log File Creat Err:%d\r\n",ret);
        #endif
        status = false;
    }
    else
    {
        f_close(&MyFile);
        status = true;
    }
    return status;    
}








