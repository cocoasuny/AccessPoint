/*********************************Copyright (c)*********************************
** File Name:               bluenrg_central_shell.c
** Last modified Date:      2016/6/27
** Last Version:            V1.0  
** Description:             文件操作命令解释
** HW_CMU:                  ANSIC
** Libraries:               NONE
** version                  NONE
******************************************************************************/

#include "main.h"
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


#ifdef BLUENRG_CENTRAL_SHELL
  extern void Shell_BlueNRG_Central_Service(void);
#else
  void Shell_BlueNRG_Central_Service(void){;}
#endif

#ifdef BLUENRG_CENTRAL_SHELL
/*---------------------* 
*       
*----------------------*/
//命令帮助文件
const char BlueNRGCentral_HelpMsg[] =
	"[BlueNRGCentral contorls]\r\n"
	" ble central help\t\t- help.\r\n"
	" ble central start scan\r\n"
	" ble central stop scan\r\n"
	"\r\n";
	
/****************************************************************************** 
********************************* 函 数 声 明 ********************************* 
******************************************************************************/  
  
/****************************************************************************** 
/ 函数功能:文件系统Shel指令处理 
/ 输入参数:输入当前的程序版本 
/ 输出参数:none 
/ 使用说明:none 
********************************************************************************/
void Shell_BlueNRG_Central_Service(void)
{
    uint8_t     *ptRxd;         //用于接收指令处理  
    uint8_t     i;
	tBleStatus  ret;

    //指令初级过滤  
    //--------------------------------------------------------------------------  
    //格式:<->[cmd bytes]<CRLF>  即"-[cmd bytes]\r\n"  
    //指令必须以"-"开始, 以"\r\n"结束 
    i = shell_rx_rdy;
    if( (i < 2) || ('\r' != shell_rx_buff[i-2]) || ('\n' != shell_rx_buff[i-1]))return;
    
    //长度和前缀过滤 
    ptRxd = (uint8_t *)shell_rx_buff;
    if( (' ' != shell_rx_buff[3]) || ('b' != shell_rx_buff[0]) || (i < 14) || 
        ('l' != shell_rx_buff[1]) || ('e' != shell_rx_buff[2]) ||
		('c' != shell_rx_buff[4]) || ('e' != shell_rx_buff[5]) ||
		('n' != shell_rx_buff[6]) || ('t' != shell_rx_buff[7]) || 
		('r' != shell_rx_buff[8]) || ('a' != shell_rx_buff[9]) ||
		('l' != shell_rx_buff[10]) || (' ' != shell_rx_buff[11]) 
	)  
	{
		return;
	}

    //处理指令
    //--------------------------------------------------------------------------
    ptRxd += 12;
    if(StrComp(ptRxd,"start scan"))    //开启扫描指令
    {
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
	else if(StrComp(ptRxd,"stop scan"))  //停止扫描指令
    {
        printf("Stop scan\r\n"); 
    }
    else if(StrComp(ptRxd,"help\r\n"))      //
    {
        printf("%s",BlueNRGCentral_HelpMsg);
    }
    else return;
    
    //退出处理
    //--------------------------------------------------------------------------
    shell_rx_rdy = 0;  //shell_rx_rdy为0,表示指令已被处理完毕,否者下个Shell服务继续调用
}
#endif

/****************** End *********************************************************/

