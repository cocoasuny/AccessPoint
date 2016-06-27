/*********************************Copyright (c)*********************************
** File Name:               rtc_shell.c
** Last modified Date:      2014/3/5 9:27:49
** Last Version:            V2.0  
** Description:             文件操作命令解释
** HW_CMU:                  ANSIC
** Libraries:               NONE
** version                  NONE
******************************************************************************/


#define RTC_SHELL       //注释掉屏蔽RTC Shell功能


#include "shell.h"          //Shell֧���ļ�,��bool,uint8_t..�Լ����������շ�����

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
	" rtc rd info\t\t- Read RTC info.\r\n"
	" rtc rd time\t\t- Read RTC date and time.\r\n"
	" rtc wr time <Hour>:<Minute>:<Second>    - Write time.\r\n"
	" rtc wr date <Year>-<Month>-<Day> <Week> - Warning Week=[1..7]\r\n"
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
    int         i;
    uint8_t     arg[32];

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
        printf("Time:%s\r\n",arg); 
    }
    else if(StrComp(ptRxd,"rd info\r\n"))      //读取RTC信息
    {
        //��ӡ��ǰʱ����ϴθ�λʱ��
   
    }
    else if(StrComp(ptRxd,"wr time "))      //
    {

    }
    else if(StrComp(ptRxd,"wr date "))      //
    {

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

