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
#include "Log.h"


#define LogHeaderLen      22  //Log前缀信息长度"[2016-12-31 23:59:59:]"

#ifdef Debug_LogRecordEnable
#define DLog(...) DsLog(__VA_ARGS__)
#else
#define DLog(...) 
#endif


/**
  * @brief  创建Log文件
  * @param  None
  * @retval 是否创建成功         
  */
uint8_t DLog_Creat(void)
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

/**
  * @brief  记录Log信息到Log文件中,应该在时间配置完成，OS开始调度之后使用
  * @param  lpszFormat 形参
  *         ... 不定参数，用法与printf一致
  * @retval 是否记录成功        
  */
uint8_t DsLog(const char* lpszFormat, ...)
{
	int nLen;
	char szBuffer[CMD_BUFFER_LEN+1];
	char wrData[CMD_BUFFER_LEN+1+LogHeaderLen] = "0";
	va_list args;
    RTC_DateTypeDef date_s;
    RTC_TimeTypeDef rtc_time;
    char fileName[20] = "a.log";
    uint8_t status = false;
    FRESULT ret = FR_NO_FILE;	
	uint32_t bw = 0;
	FSIZE_t len = 0;
	
		
	va_start(args, lpszFormat);
	nLen = vsnprintf(szBuffer, CMD_BUFFER_LEN+1, lpszFormat, args);
	va_end(args);
	
//	printf("%s",szBuffer);
    /* 以时间信息为文件名称 */
    Calendar_Get(&date_s,&rtc_time);
    sprintf(fileName,"%d%d%d%d.log",20,date_s.Year,date_s.Month,date_s.Date);
	
	/* 获取Log信息前缀，以时间信息为前缀 */
	sprintf(wrData,"[%d%d-%d-%d %d:%d:%d]:",20,date_s.Year,date_s.Month,date_s.Date,
											rtc_time.Hours,rtc_time.Minutes,rtc_time.Seconds);
	memcpy(wrData+LogHeaderLen,szBuffer,nLen);

    /*##-3- Create and Open a new text file object with write access #####*/
    ret = f_open(&MyFile, fileName, FA_OPEN_ALWAYS | FA_WRITE);
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
		len = f_size(&MyFile);
		f_lseek(&MyFile,len);
		if(f_tell(&MyFile) == len)
		{
			ret = f_write(&MyFile, wrData,(LogHeaderLen+nLen), (void *)&bw);
			if(ret != FR_OK)
			{
				/* Log file Open for write Error */
				#ifdef Debug_Log
					printf("Log File Writr Err:%d\r\n",ret);
				#endif
				status = false;
			}
			else
			{
				status = true;
			}
		}
		#ifdef Debug_Log
		else
		{	
			printf("f_lseek Err in Log Write\r\n");
		}
		#endif
        f_close(&MyFile);
    }	
	return status;
}






