/*********************************Copyright (c)*********************************
** File Name:               rtc_shell.c
** Last modified Date:      2014/3/5 9:27:49
** Last Version:            V2.0  
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
  extern void Shell_RTC_Service(void);
#else
  void Shell_RTC_Service(void){;}
#endif

#ifdef RTC_SHELL
/*---------------------* 
*       
*----------------------*/
//命令帮助文件
const char RTC_HelpMsg[] =
	"[RTC contorls]\r\n"
	" rtc help\t\t- help.\r\n"
	" rtc rd info\t\t- Read RTC info in gather time.\r\n"
	" rtc rd time\t\t- Read RTC date and time.\r\n"
	" rtc wr time <Hour>:<Minute>:<Second>  - Not use.\r\n"
	" rtc wr date <Year>-<Month>-<Day>  <Hour>:<Minute>:<Second>  - Write date and time \r\n"
	"\r\n";
	
/****************************************************************************** 
********************************* 函 数 声 明 ********************************* 
******************************************************************************/  
  
/****************************************************************************** 
/ 函数功能:文件系统Shel指令处理 
/ 修改日期:2013/9/10 19:04:15 
/ 输入参数:输入当前的程序版本 
/ 输出参数:none 
/ 使用说明:none 
********************************************************************************/
void Shell_RTC_Service(void)
{
    uint8_t     *ptRxd;         //用于接收指令处理  
    uint8_t     i;
	int     	Hour = 0;
	int     	Min  = 0;
	int     	Sec  = 0;
	int         Year = 0;
	int         Mon  = 0;
	int         Date = 0;
    uint8_t     arg[32];
	uint16_t    retval; 
    
    RTC_DateTypeDef date_s;
    RTC_TimeTypeDef rtc_time;

    //指令初级过滤  
    //--------------------------------------------------------------------------  
    //格式:<->[cmd bytes]<CRLF>  即"-[cmd bytes]\r\n"  
    //指令必须以"-"开始, 以"\r\n"结束 
    i = shell_rx_rdy;
    if( (i < 2) || ('\r' != shell_rx_buff[i-2]) || ('\n' != shell_rx_buff[i-1]))return;
    
    //长度和前缀过滤 
    ptRxd = (uint8_t *)shell_rx_buff;
    if( (' ' != shell_rx_buff[3]) || ('r' != shell_rx_buff[0]) || (i < 6) || 
        ('t' != shell_rx_buff[1]) || ('c' != shell_rx_buff[2]) )  return;
       
    //处理指令
    //--------------------------------------------------------------------------
    ptRxd += 4;
    if(StrComp(ptRxd,"rd time"))    //按包读取指令
    {
        Calendar_Get(&date_s,&rtc_time);
        printf("\r\nRTC:  %02d-%02d-%02d %d %0.2d:%0.2d:%0.2d\r\n",2000 + date_s.Year,date_s.Month, date_s.Date,date_s.WeekDay, 
                                                               rtc_time.Hours, rtc_time.Minutes, rtc_time.Seconds); 
    }
    else if(StrComp(ptRxd,"rd info\r\n"))      //读取RTC信息
    {
        printf("Time:%s\r\n",arg);
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
        printf("%s",RTC_HelpMsg);
    }
    else return;
    
    //退出处理
    //--------------------------------------------------------------------------
    shell_rx_rdy = 0;  //shell_rx_rdy为0,表示指令已被处理完毕,否者下个Shell服务继续调用
}
#endif

/****************** End *********************************************************/

