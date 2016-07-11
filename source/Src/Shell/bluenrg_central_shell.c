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
	" ble central connmac <add1>,<add2>,<add3>,<add4>,<add5>,<add6>\r\n"
	" ble central disconn\r\n"
	" ble central dscryservs\r\n"
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
	tBDAddr     add;
	uint16_t    retval;
	
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
		Start_Scan_Procedure();
    }
	else if(StrComp(ptRxd,"stop scan"))  //停止扫描指令
    {
		/* Stop Scan */
        Stop_Scan_Procedure(); 
    }
	else if(StrComp(ptRxd,"connmac "))   //连接设备指令
	{
		retval = sscanf((void*)shell_rx_buff,"%*s%*s%*s%x,%x,%x,%x,%x,%x",(int *)&add[0],(int *)&add[1],(int *)&add[2],
														   (int *)&add[3],(int *)&add[4],(int *)&add[5]);  
        if(6 != retval)
		{
			return;   //没有接收到6个输入数据,直接退出  
		}
		else
		{
			printf("connmac :0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x\r\n",
					add[0],add[1],add[2],add[3],add[4],add[5]);
		}
		/* Connect device */
		GAP_Central_Make_Connection(add);
	}
	else if (StrComp(ptRxd,"disconn"))  //断开连接设备指令
	{
		/* Disconnect device */
		GAP_Central_Make_Disconnection(connection_handle);
	}
	else if(StrComp(ptRxd,"dscryservs"))  //发现服务指令
	{
		/* Discovery services */
		GAP_Discovery_Service(connection_handle);
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

