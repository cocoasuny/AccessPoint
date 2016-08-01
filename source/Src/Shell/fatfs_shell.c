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
	" fil help\t\t              -  help.\r\n"
	" fil cap\t\t               -  the capacity of SD Card.\r\n"
	" fil creat\t\t             -  creat a file.\r\n"
    " fil open <Name>\t\t       -  open a file by name.\r\n"
	" fil wr <Data>\t\t         -  write data to the file.\r\n"
	" fil dir\t\t               -  display all files in current directory \r\n"
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
    
    RTC_DateTypeDef date_s;
    RTC_TimeTypeDef rtc_time;
	SD_CardInfo  CardInfo;
	char fileName[20] = "a.txt";
    char openfileName[20] = "0";
	char *wrdataBuf = "0";
	uint32_t bw = 0;
	char buff[256] = {0};
    FRESULT ret = FR_NO_FILE;

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
		sprintf(fileName,"%d%d%d%d.txt",20,date_s.Year,date_s.Month,date_s.Date);
		
		/*##-3- Create and Open a new text file object with write access #####*/
        ret = f_open(&MyFile, fileName, FA_CREATE_ALWAYS | FA_WRITE);
		if(ret != FR_OK)
		{
			/* 'STM32.TXT' file Open for write Error */
			#ifdef Debug_FatFs_Driver
				printf("f_open Err in fatfs_shell\r\n");
			#endif
		}
		else
		{
			#ifdef Debug_FatFs_Driver
                printf("Creat file success:%d\r\n",ret);
			#endif
			//f_write(&MyFile, "FatFS Write Demo \r\n www.armfly.com \r\n", 34, &bw);
			f_close(&MyFile);
		}
    }
    else if(StrComp(ptRxd,"dir"))      //显示当前路径下所有文件
    {
        strcpy(buff, "/");
        scan_files(buff);
    }
    else if(StrComp(ptRxd,"open"))      //根据文件名称打开文件
    {
		sscanf((void*)shell_rx_buff,"%*s%*s %s",openfileName);  

		/* Open the file by name */	
		ret = f_open(&MyFile, openfileName, FA_CREATE_ALWAYS | FA_WRITE);	
		if(ret != FR_OK)
		{
			#ifdef Debug_FatFs_Driver
				printf("open the file:%s err\r\n",openfileName);
			#endif
		}
		else
		{
			#ifdef Debug_FatFs_Driver
                printf("open the file:%s Ok\r\n",openfileName);
			#endif
		}			
    }
    else if(StrComp(ptRxd,"wr"))      //根据文件名称打开文件
    {
		sscanf((void*)shell_rx_buff,"%*s%*s %s",wrdataBuf);  
		
		/*##-4- Write data to the text file ################################*/
		ret = f_write(&MyFile, wrdataBuf, sizeof(wrdataBuf), (void *)&bw);

		if((bw == 0) || (ret != FR_OK))
		{
			#ifdef Debug_FatFs_Driver
				/* 'STM32.TXT' file Write or EOF Error */
				printf("f_write data %s Err\r\n",wrdataBuf);
			#endif
		}
		else
		{
			#ifdef Debug_FatFs_Driver
				/* 'STM32.TXT' file Write or EOF Error */
				printf("f_write data %s,%d OK\r\n",wrdataBuf,sizeof(wrdataBuf));
			#endif			
			/*##-5- Close the open text file #################################*/
			f_sync(&MyFile);
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

