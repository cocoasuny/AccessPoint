/*********************************Copyright (c)*********************************
** File Name:               fatfs_shell.c
** Last modified Date:      2016/7/26
** Last Version:            V1.0
** Description:             文件操作命令解释
** HW_CMU:                  ANSIC
** Libraries:               NONE
** version                  NONE
******************************************************************************/


#include "shell.h"          //Shell

/*---------------------* 
*     Shell版本判断
*----------------------*/
#ifdef SHELL_VER
  #if (SHELL_VER < 2)
    #error "shell版本太低"
  #endif
#else
    #error "未找到Shell文件，或Shell版本信息"
#endif


#ifdef RTC_SHELL
  extern void Shell_Fatfs_Service(void);
#else
  void Shell_Fatfs_Service(void){;}
#endif

#ifdef FATFS_SHELL
/*---------------------* 
*       
*----------------------*/
//命令帮助文件
const char Fatfs_HelpMsg[] =
	"[Fatfs contorls]\r\n"
	" fil help\t\t   -  help.\r\n"
	" fil cap\t\t    -  the capacity of SD Card.\r\n"
	" fil creat\t\t  -  creat a file.\r\n"
	"\r\n";
	
/****************************************************************************** 
********************************* 函 数 声 明 ********************************* 
******************************************************************************/  
  
/****************************************************************************** 
/ 函数功能:文件系统Shel指令处理 
/ 修改日期:2016-7-26
/ 输入参数:输入当前的程序版本 
/ 输出参数:none 
/ 使用说明:none 
********************************************************************************/
void Shell_Fatfs_Service(void)
{
    uint8_t     *ptRxd;         //用于接收指令处理  
    uint8_t     i;
	int     	Hour = 0;
	int     	Min  = 0;
	int     	Sec  = 0;
	int         Year = 0;
	int         Mon  = 0;
	int         Date = 0;
	uint16_t    retval; 
    
    RTC_DateTypeDef date_s;
    RTC_TimeTypeDef rtc_time;
	SD_CardInfo  CardInfo;
	char fileName[20] = "a.txt";
	uint32_t bw;

    //指令初级过滤  
    //--------------------------------------------------------------------------  
    //格式:<->[cmd bytes]<CRLF>  即"-[cmd bytes]\r\n"  
    //指令必须以"-"开始, 以"\r\n"结束 
    i = shell_rx_rdy;
    if( (i < 2) || ('\r' != shell_rx_buff[i-2]) || ('\n' != shell_rx_buff[i-1]))return;
    
    //长度和前缀过滤 
    ptRxd = (uint8_t *)shell_rx_buff;
    if( (' ' != shell_rx_buff[3]) || ('f' != shell_rx_buff[0]) || (i < 6) || 
        ('i' != shell_rx_buff[1]) || ('l' != shell_rx_buff[2]) )  return;
       
    //处理指令
    //--------------------------------------------------------------------------
    ptRxd += 4;
    if(StrComp(ptRxd,"cap"))    //按包读取指令
    {
		BSP_SD_Init();
		BSP_SD_GetCardInfo(&CardInfo);
		printf("SD Card info:\r\n");
		printf("Card ManufacturerID:%d\r\n",CardInfo.SD_cid.ManufacturerID);	    //
		printf("Card RCA:%d\r\n",CardInfo.RCA);								        //
		printf("Card Capacity:%d MB\r\n",(uint32_t)(CardInfo.CardCapacity>>20));	//
		printf("Card BlockSize:%d\r\n\r\n",CardInfo.CardBlockSize);			        //
    }
    else if(StrComp(ptRxd,"creat\r\n"))    //创建文件
    {
		/* 以时间信息为文件名称 */
		Calendar_Get(&date_s,&rtc_time);
		sprintf(fileName,"%d%d-%d-%d.txt",20,date_s.Year,date_s.Month,date_s.Date);
		
		/*##-1- Register the file system object to the FatFs module ##############*/
		if(f_mount(&SDFatFs, (TCHAR const*)SDPath, 0) != FR_OK)
		{
			#ifdef Debug_FatFs_Driver
				/* FatFs Initialization Error */
				printf("f_mount Err in fatfs_shell\r\n"); 
			#endif
		}
		else  
		{
			/*##-2- Create a FAT file system (format) on the logical drive #########*/
			/* WARNING: Formatting the uSD card will delete all content on the device */
			if(f_mkfs((TCHAR const*)SDPath, 0, 0) != FR_OK)
			{
				/* FatFs Format Error */
				#ifdef Debug_FatFs_Driver
					printf("FatFs Format Err in fatfs_shell\r\n");
				#endif
			}
		}

		/*##-3- Create and Open a new text file object with write access #####*/
		if(f_open(&MyFile, fileName, FA_CREATE_ALWAYS | FA_WRITE) != FR_OK)
		{
			/* 'STM32.TXT' file Open for write Error */
			#ifdef Debug_FatFs_Driver
				printf("f_open Err in fatfs_shell\r\n");
			#endif
		}
		else
		{
			#ifdef Debug_FatFs_Driver
				printf("Creat file success\r\n");
			#endif
			//f_write(&MyFile, "FatFS Write Demo \r\n www.armfly.com \r\n", 34, &bw);
			f_close(&MyFile);
		}
    }
    else if(StrComp(ptRxd,"wr time "))      //
    {
		retval = sscanf((void*)shell_rx_buff,"%*s%*s%*s%d:%d:%d %d-%d-%d",&Hour,&Min,&Sec,&Year,&Mon,&Date);  
        if(3 != retval)
		{
			return;   //没有接收到3个输入数据,直接退出  
		}
		else
		{
			printf("Set Time :%d,%d,%d\r\n",Hour,Min,Sec);
		}
    }
    else if(StrComp(ptRxd,"wr date "))      //
    {
		retval = sscanf((void*)shell_rx_buff,"%*s%*s%*s%d-%d-%d %d:%d:%d",&Year,&Mon,&Date,&Hour,&Min,&Sec);  
		if(6 != retval)
		{
			return;   //没有接收到4个输入数据,直接退出 
		}
		else
		{
			printf("Set Date:%d-%d-%d %d:%d:%d\r\n",Year,Mon,Date,Hour,Min,Sec);
            date_s.Year = (uint8_t)(Year % 100);
            date_s.Month = (uint8_t)Mon;
            date_s.Date = (uint8_t)Date;
            
            rtc_time.Hours = (uint8_t)Hour;
            rtc_time.Minutes = (uint8_t)Min;
            rtc_time.Seconds = (uint8_t)Sec;
           
            Calendar_Set(&date_s,&rtc_time);
		}
    }
    else if(StrComp(ptRxd,"help\r\n"))      //
    {
        printf("%s",Fatfs_HelpMsg);
    }
    else return;
    
    //退出处理
    //--------------------------------------------------------------------------
    shell_rx_rdy = 0;  //shell_rx_rdy为0,表示指令已被处理完毕,否者下个Shell服务继续调用
}
#endif

/****************** End *********************************************************/

