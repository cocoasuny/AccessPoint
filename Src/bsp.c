/**
  ******************************************************************************
  * File Name          : bsp.c
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
#include "bsp.h"
#include "usbd_cdc_if.h"
#include "usb_device.h"
#include "main.h"
#include "bsp_hum_temp.h"
#include "bsp_pressure.h"


/** @defgroup bsp_Private_Variables
 * @{
 */
/* uart */
static uint8_t UsbSendData(uint8_t* pBuf, uint16_t nLen);
static UART_HandleTypeDef UartHandle;

/*I2c*/
uint32_t I2C_EXPBD_Timeout = NUCLEO_I2C_EXPBD_TIMEOUT_MAX;    /*<! Value of Timeout when I2C communication fails */
static I2C_HandleTypeDef    I2C_EXPBD_Handle;

/* Link function for PRESSURE peripheral */
PRESSURE_StatusTypeDef LPS25H_IO_Init(void);
void LPS25H_IO_ITConfig( void );
PRESSURE_StatusTypeDef LPS25HB_IO_Init(void);
void LPS25HB_IO_ITConfig( void );
PRESSURE_StatusTypeDef LPS25H_IO_Write(uint8_t* pBuffer, uint8_t DeviceAddr, uint8_t RegisterAddr,
                                       uint16_t NumByteToWrite);
PRESSURE_StatusTypeDef LPS25HB_IO_Write(uint8_t* pBuffer, uint8_t DeviceAddr, uint8_t RegisterAddr,
                                        uint16_t NumByteToWrite);
PRESSURE_StatusTypeDef LPS25H_IO_Read(uint8_t* pBuffer, uint8_t DeviceAddr, uint8_t RegisterAddr,
                                      uint16_t NumByteToRead);
PRESSURE_StatusTypeDef LPS25HB_IO_Read(uint8_t* pBuffer, uint8_t DeviceAddr, uint8_t RegisterAddr,
                                       uint16_t NumByteToRead);
                                       
/* Link function for HUM_TEMP peripheral */
HUM_TEMP_StatusTypeDef HTS221_IO_Init(void);
void HTS221_IO_ITConfig( void );
HUM_TEMP_StatusTypeDef HTS221_IO_Write(uint8_t* pBuffer, uint8_t DeviceAddr, uint8_t RegisterAddr,
                                       uint16_t NumByteToWrite);
HUM_TEMP_StatusTypeDef HTS221_IO_Read(uint8_t* pBuffer, uint8_t DeviceAddr, uint8_t RegisterAddr,
                                      uint16_t NumByteToRead);

static PRESSURE_StatusTypeDef PRESSURE_IO_Init(void);
static PRESSURE_StatusTypeDef PRESSURE_IO_Write(uint8_t* pBuffer, uint8_t DeviceAddr, uint8_t RegisterAddr,
        uint16_t NumByteToWrite);
static PRESSURE_StatusTypeDef PRESSURE_IO_Read(uint8_t* pBuffer, uint8_t DeviceAddr, uint8_t RegisterAddr,
        uint16_t NumByteToRead);
static HUM_TEMP_StatusTypeDef HUM_TEMP_IO_Init(void);
static HUM_TEMP_StatusTypeDef HUM_TEMP_IO_Write(uint8_t* pBuffer, uint8_t DeviceAddr, uint8_t RegisterAddr,
        uint16_t NumByteToWrite);
static HUM_TEMP_StatusTypeDef HUM_TEMP_IO_Read(uint8_t* pBuffer, uint8_t DeviceAddr, uint8_t RegisterAddr,
        uint16_t NumByteToRead);

/************************************I2C**********************************************/
static void I2C_EXPBD_MspInit(void);
static void I2C_EXPBD_Error(uint8_t Addr);
static HAL_StatusTypeDef I2C_EXPBD_Init(void);
static HAL_StatusTypeDef I2C_EXPBD_WriteData(uint8_t* pBuffer, uint8_t Addr, uint8_t Reg, uint16_t Size);
static HAL_StatusTypeDef I2C_EXPBD_ReadData(uint8_t* pBuffer, uint8_t Addr, uint8_t Reg, uint16_t Size);

/*
*********************************************************************************************************
*	函 数 名: Bsp_Init
*	功能说明: Bsp Init
*	形    参：None
*	返 回 值: None
*********************************************************************************************************/
void Bsp_Init(void)
{
    /* Initialize all configured peripherals */
    MX_GPIO_Init();
    UART_Init();
    MX_SDIO_SD_Init();
    
    /* init code for USB_DEVICE */
    MX_USB_DEVICE_Init();

    /* init code for FATFS */
    MX_FATFS_Init();  
    
    /* init code for humidity & temperature sensor */
    BSP_HUM_TEMP_Init();
    
    /* init code for pressure sensor */
    BSP_PRESSURE_Init();
    
    #ifndef PRINTFLOG
        HAL_Delay(5000);
    #endif
    DLog("Start...\r\n");
}    




/**
  * @brief  Configures LED GPIO.
  * @param  Led: LED to be configured. 
  *          This parameter can be one of the following values:
  *            @arg  LED1
  *            @arg  LED2
  *            @arg  LED3
  *            @arg  LED4
  */
void BSP_LED_Init(void)
{
    GPIO_InitTypeDef  GPIO_InitStruct;

    /* Enable the GPIO_LED clock */
    __GPIOB_CLK_ENABLE();

    /* Configure the GPIO_LED pin */
    GPIO_InitStruct.Pin = GPIO_PIN_3;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FAST;

    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}

/**
  * @brief  Turns selected LED On.
  * @param  Led: LED to be set on 
  *          This parameter can be one of the following values:
  *            @arg  LED1
  *            @arg  LED2
  *            @arg  LED3
  *            @arg  LED4
  */
void BSP_LED_On(void )
{
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_SET); 
}

/**
  * @brief  Turns selected LED Off. 
  * @param  Led: LED to be set off
  *          This parameter can be one of the following values:
  *            @arg  LED1
  *            @arg  LED2
  *            @arg  LED3
  *            @arg  LED4
  */
void BSP_LED_Off(void)
{
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_RESET); 
}

/**
  * @brief  Toggles the selected LED.
  * @param  Led: LED to be toggled
  *          This parameter can be one of the following values:
  *            @arg  LED1
  *            @arg  LED2
  *            @arg  LED3
  *            @arg  LED4
  */
void BSP_LED_Toggle(void)
{
    HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_3);
}

/*
*********************************************************************************************************
*	函 数 名: UsbSendData
*	功能说明: 向虚拟串口发送数据
*	形    参：pBuf, 数据缓冲地址
*			  nLen, 数据长度，以字节为单位
*	返 回 值:	=USBD_OK，发送成功
*				=USBD_BUSY，Tx忙，需要重发
*				=USBD_FAIL，发送失败
*	注    释：
*	作    者：碧云天书
*********************************************************************************************************
*/
static uint8_t UsbSendData(uint8_t* pBuf, uint16_t nLen)
{
	USBD_CDC_SetTxBuffer(&hUsbDeviceFS, (uint8_t*)pBuf, nLen);
	return USBD_CDC_TransmitPacket(&hUsbDeviceFS);
}

/*
*********************************************************************************************************
*	函 数 名: UsbPrintf
*	功能说明: 向虚拟串口打印字符串, 接上串口线后，打开PC机的超级终端软件可以观察结果。语法与printf相同。
*	形    参：lpszFormat, 格式描述字串
*			  ..., 不定参数
*	返 回 值: 无
*	注    释：
*	作    者：碧云天书
*********************************************************************************************************
*/
void USBLog(const char* lpszFormat, ...)
{
	int nLen;
	char szBuffer[CMD_BUFFER_LEN+1];
	va_list args;
	va_start(args, lpszFormat);
	nLen = vsnprintf(szBuffer, CMD_BUFFER_LEN+1, lpszFormat, args);
	UsbSendData((uint8_t*)szBuffer, nLen);
	va_end(args);
}


/* Private function prototypes -----------------------------------------------*/
#ifdef __GNUC__
/* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
   set to 'Yes') calls __io_putchar() */
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */

/** @defgroup HAL_MSP_Private_Functions
  * @{
  */


/**
  * @brief  Retargets the C library printf function to the USART.
  * @param  None
  * @retval None
  */
int fputc(int ch, FILE *f)
{
    /* Place your implementation of fputc here */
    /* e.g. write a character to the EVAL_COM1 and Loop until the end of transmission */
    HAL_StatusTypeDef status = HAL_UART_Transmit(&UartHandle, (uint8_t *)&ch, 1, 0xFFFF);

    if (status != HAL_OK) {
        //while (1);
        return 0;
    }
    return ch;
}

void UART_Init(void)
{
    /*##-1- Configure the UART peripheral ######################################*/
    /* Put the USART peripheral in the Asynchronous mode (UART Mode) */
    /* UART1 configured as follow:
        - Word Length = 8 Bits
        - Stop Bit = One Stop bit
        - Parity = ODD parity
        - BaudRate = 9600 baud
        - Hardware flow control disabled (RTS and CTS signals) */
    UartHandle.Instance          = USARTx;

    UartHandle.Init.BaudRate     = 115200;
    UartHandle.Init.WordLength   = UART_WORDLENGTH_8B;
    UartHandle.Init.StopBits     = UART_STOPBITS_1;
    UartHandle.Init.Parity       = UART_PARITY_NONE;
    UartHandle.Init.HwFlowCtl    = UART_HWCONTROL_NONE;
    UartHandle.Init.Mode         = UART_MODE_TX;
    UartHandle.Init.OverSampling = UART_OVERSAMPLING_16;

    if(HAL_UART_Init(&UartHandle) != HAL_OK)
    {
        /* Initialization Error */
        while (1);
    }
}


/**
  * @brief UART MSP Initialization
  *        This function configures the hardware resources used in this example:
  *           - Peripheral's clock enable
  *           - Peripheral's GPIO Configuration
  * @param huart: UART handle pointer
  * @retval None
  */
void HAL_UART_MspInit(UART_HandleTypeDef *huart)
{
    GPIO_InitTypeDef  GPIO_InitStruct;

    /*##-1- Enable peripherals and GPIO Clocks #################################*/
    /* Enable GPIO TX/RX clock */
    USARTx_TX_GPIO_CLK_ENABLE();
    USARTx_RX_GPIO_CLK_ENABLE();

    /* Enable USARTx clock */
    USARTx_CLK_ENABLE();

    /*##-2- Configure peripheral GPIO ##########################################*/
    /* UART TX GPIO pin configuration  */
    GPIO_InitStruct.Pin       = USARTx_TX_PIN;
    GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull      = GPIO_PULLUP;
    GPIO_InitStruct.Speed     = GPIO_SPEED_FAST;
    GPIO_InitStruct.Alternate = USARTx_TX_AF;

    HAL_GPIO_Init(USARTx_TX_GPIO_PORT, &GPIO_InitStruct);

    /* UART RX GPIO pin configuration  */
    GPIO_InitStruct.Pin = USARTx_RX_PIN;
    GPIO_InitStruct.Alternate = USARTx_RX_AF;

    HAL_GPIO_Init(USARTx_RX_GPIO_PORT, &GPIO_InitStruct);
}

/**
  * @brief UART MSP De-Initialization
  *        This function frees the hardware resources used in this example:
  *          - Disable the Peripheral's clock
  *          - Revert GPIO and NVIC configuration to their default state
  * @param huart: UART handle pointer
  * @retval None
  */
void HAL_UART_MspDeInit(UART_HandleTypeDef *huart)
{
    /*##-1- Reset peripherals ##################################################*/
    USARTx_FORCE_RESET();
    USARTx_RELEASE_RESET();

    /*##-2- Disable peripherals and GPIO Clocks #################################*/
    /* Configure UART Tx as alternate function  */
    HAL_GPIO_DeInit(USARTx_TX_GPIO_PORT, USARTx_TX_PIN);
    /* Configure UART Rx as alternate function  */
    HAL_GPIO_DeInit(USARTx_RX_GPIO_PORT, USARTx_RX_PIN);

}

void app_error_handler(uint32_t error_code, uint32_t line_num, const uint8_t * p_file_name)
{
	DLog("\r\nErr = 0x%x,file = %s,line = %d.\r\n",error_code,p_file_name,line_num);
}
/********************************* LINK PRESSURE *****************************/
/**
 * @brief  Configures LPS25H I2C interface
 * @retval PRESSURE_OK in case of success, an error code otherwise
 */
PRESSURE_StatusTypeDef LPS25H_IO_Init(void)
{
    return PRESSURE_IO_Init();
}

/**
 * @brief  Configures LPS25H interrupt lines for NUCLEO boards
 * @retval None
 */
void LPS25H_IO_ITConfig( void )
{
    /* To be implemented */
}

/**
 * @brief  Configures LPS25HB I2C interface
 * @retval PRESSURE_OK in case of success, an error code otherwise
 */
PRESSURE_StatusTypeDef LPS25HB_IO_Init(void)
{
    return PRESSURE_IO_Init();
}

/**
 * @brief  Configures LPS25HB interrupt lines for NUCLEO boards
 * @retval None
 */
void LPS25HB_IO_ITConfig( void )
{
    /* To be implemented */
}

/**
 * @brief  Writes a buffer to the LPS25H sensor
 * @param  pBuffer the pointer to data to be written
 * @param  DeviceAddr the slave address to be programmed
 * @param  RegisterAddr the pressure internal address register to be written
 * @param  NumByteToWrite the number of bytes to be written
 * @retval PRESSURE_OK in case of success, an error code otherwise
 */
PRESSURE_StatusTypeDef LPS25H_IO_Write(uint8_t* pBuffer, uint8_t DeviceAddr, uint8_t RegisterAddr,
                                       uint16_t NumByteToWrite)
{
    return PRESSURE_IO_Write(pBuffer, DeviceAddr, RegisterAddr, NumByteToWrite);
}

/**
 * @brief  Writes a buffer to the LPS25HB sensor
 * @param  pBuffer the pointer to data to be written
 * @param  DeviceAddr the slave address to be programmed
 * @param  RegisterAddr the pressure internal address register to be written
 * @param  NumByteToWrite the number of bytes to be written
 * @retval PRESSURE_OK in case of success, an error code otherwise
 */
PRESSURE_StatusTypeDef LPS25HB_IO_Write(uint8_t* pBuffer, uint8_t DeviceAddr, uint8_t RegisterAddr,
                                        uint16_t NumByteToWrite)
{
    return PRESSURE_IO_Write(pBuffer, DeviceAddr, RegisterAddr, NumByteToWrite);
}

/**
 * @brief  Reads a buffer from the LPS25H sensor
 * @param  pBuffer the pointer to data to be read
 * @param  DeviceAddr the slave address to be programmed
 * @param  RegisterAddr the pressure internal address register to be read
 * @param  NumByteToRead the number of bytes to be read
 * @retval PRESSURE_OK in case of success, an error code otherwise
 */
PRESSURE_StatusTypeDef LPS25H_IO_Read(uint8_t* pBuffer, uint8_t DeviceAddr, uint8_t RegisterAddr,
                                      uint16_t NumByteToRead)
{
    return PRESSURE_IO_Read(pBuffer, DeviceAddr, RegisterAddr, NumByteToRead);
}

/**
 * @brief  Reads a buffer from the LPS25HB sensor
 * @param  pBuffer the pointer to data to be read
 * @param  DeviceAddr the slave address to be programmed
 * @param  RegisterAddr the pressure internal address register to be read
 * @param  NumByteToRead the number of bytes to be read
 * @retval PRESSURE_OK in case of success, an error code otherwise
 */
PRESSURE_StatusTypeDef LPS25HB_IO_Read(uint8_t* pBuffer, uint8_t DeviceAddr, uint8_t RegisterAddr,
                                       uint16_t NumByteToRead)
{
    return PRESSURE_IO_Read(pBuffer, DeviceAddr, RegisterAddr, NumByteToRead);
}

/********************************* LINK HUM_TEMP *****************************/
/**
 * @brief  Configures HTS221 I2C interface
 * @retval HUM_TEMP_OK in case of success, an error code otherwise
 */
HUM_TEMP_StatusTypeDef HTS221_IO_Init(void)
{
    return HUM_TEMP_IO_Init();
}

/**
 * @brief  Configures HTS221 interrupt lines for NUCLEO boards
 * @retval None
 */
void HTS221_IO_ITConfig( void )
{
    /* To be implemented */
}

/**
 * @brief  Writes a buffer to the HTS221 sensor
 * @param  pBuffer the pointer to data to be written
 * @param  DeviceAddr the slave address to be programmed
 * @param  RegisterAddr the humidity and temperature internal address register to be written
 * @param  NumByteToWrite the number of bytes to be written
 * @retval HUM_TEMP_OK in case of success, an error code otherwise
 */
HUM_TEMP_StatusTypeDef HTS221_IO_Write(uint8_t* pBuffer, uint8_t DeviceAddr, uint8_t RegisterAddr,
                                       uint16_t NumByteToWrite)
{
    return HUM_TEMP_IO_Write(pBuffer, DeviceAddr, RegisterAddr, NumByteToWrite);
}

/**
 * @brief  Reads a buffer from the HTS221 sensor
 * @param  pBuffer the pointer to data to be read
 * @param  DeviceAddr the slave address to be programmed
 * @param  RegisterAddr the humidity and temperature internal address register to be read
 * @param  NumByteToRead the number of bytes to be read
 * @retval HUM_TEMP_OK in case of success, an error code otherwise
 */
HUM_TEMP_StatusTypeDef HTS221_IO_Read(uint8_t* pBuffer, uint8_t DeviceAddr, uint8_t RegisterAddr,
                                      uint16_t NumByteToRead)
{
    return HUM_TEMP_IO_Read(pBuffer, DeviceAddr, RegisterAddr, NumByteToRead);
}


/**
 * @brief  Configures humidity and temperature I2C interface
 * @retval HUM_TEMP_OK in case of success, an error code otherwise
 */
static HUM_TEMP_StatusTypeDef HUM_TEMP_IO_Init(void)
{
    if(I2C_EXPBD_Init() != HAL_OK)
    {
        return HUM_TEMP_ERROR;
    }

    return HUM_TEMP_OK;
}

/**
 * @brief  Writes a buffer to the humidity and temperature sensor
 * @param  pBuffer the pointer to data to be written
 * @param  DeviceAddr the slave address to be programmed
 * @param  RegisterAddr the humidity and temperature internal address register to be written
 * @param  NumByteToWrite the number of bytes to be written
 * @retval HUM_TEMP_OK in case of success, an error code otherwise
 */
static HUM_TEMP_StatusTypeDef HUM_TEMP_IO_Write(uint8_t* pBuffer, uint8_t DeviceAddr, uint8_t RegisterAddr,
        uint16_t NumByteToWrite)
{
    HUM_TEMP_StatusTypeDef ret_val = HUM_TEMP_OK;

    /* call I2C_EXPBD Read data bus function */
    if(I2C_EXPBD_WriteData( pBuffer, DeviceAddr, RegisterAddr, NumByteToWrite ) != HAL_OK)
    {
        ret_val = HUM_TEMP_ERROR;
    }

    return ret_val;
}

/**
 * @brief  Reads a buffer from the humidity and temperature sensor
 * @param  pBuffer the pointer to data to be read
 * @param  DeviceAddr the slave address to be programmed
 * @param  RegisterAddr the humidity and temperature internal address register to be read
 * @param  NumByteToRead the number of bytes to be read
 * @retval HUM_TEMP_OK in case of success, an error code otherwise
 */
static HUM_TEMP_StatusTypeDef HUM_TEMP_IO_Read(uint8_t* pBuffer, uint8_t DeviceAddr, uint8_t RegisterAddr,
        uint16_t NumByteToRead)
{
    HUM_TEMP_StatusTypeDef ret_val = HUM_TEMP_OK;

    /* call I2C_EXPBD Read data bus function */
    if(I2C_EXPBD_ReadData( pBuffer, DeviceAddr, RegisterAddr, NumByteToRead ) != HAL_OK)
    {
        ret_val = HUM_TEMP_ERROR;
    }

    return ret_val;
}
/**
 * @brief  Configures pressure I2C interface
 * @retval PRESSURE_OK in case of success, an error code otherwise
 */
static PRESSURE_StatusTypeDef PRESSURE_IO_Init(void)
{
    if(I2C_EXPBD_Init() != HAL_OK)
    {
        return PRESSURE_ERROR;
    }

    return PRESSURE_OK;
}

/**
 * @brief  Writes a buffer to the pressure sensor
 * @param  pBuffer the pointer to data to be written
 * @param  DeviceAddr the slave address to be programmed
 * @param  RegisterAddr the pressure internal address register to be written
 * @param  NumByteToWrite the number of bytes to be written
 * @retval PRESSURE_OK in case of success, an error code otherwise
 */
static PRESSURE_StatusTypeDef PRESSURE_IO_Write(uint8_t* pBuffer, uint8_t DeviceAddr, uint8_t RegisterAddr,
        uint16_t NumByteToWrite)
{
    PRESSURE_StatusTypeDef ret_val = PRESSURE_OK;

    /* call I2C_EXPBD Read data bus function */
    if(I2C_EXPBD_WriteData(pBuffer, DeviceAddr, RegisterAddr, NumByteToWrite) != HAL_OK)
    {
        ret_val = PRESSURE_ERROR;
    }

    return ret_val;
}

/**
 * @brief  Reads a buffer from the pressure sensor
 * @param  pBuffer the pointer to data to be read
 * @param  DeviceAddr the slave address to be programmed
 * @param  RegisterAddr the pressure internal address register to be read
 * @param  NumByteToRead number of bytes to be read
 * @retval PRESSURE_OK in case of success, an error code otherwise
 */
static PRESSURE_StatusTypeDef PRESSURE_IO_Read(uint8_t* pBuffer, uint8_t DeviceAddr, uint8_t RegisterAddr,
        uint16_t NumByteToRead)
{
    PRESSURE_StatusTypeDef ret_val = PRESSURE_OK;

    /* call I2C_EXPBD Read data bus function */
    if(I2C_EXPBD_ReadData(pBuffer, DeviceAddr, RegisterAddr, NumByteToRead) != HAL_OK)
    {
        ret_val = PRESSURE_ERROR;
    }

    return ret_val;
}

/******************************* I2C Routines**********************************/
/**
 * @brief  Configures I2C interface
 * @retval HAL status
 */
static HAL_StatusTypeDef I2C_EXPBD_Init(void)
{
    HAL_StatusTypeDef ret_val = HAL_OK;

    if(HAL_I2C_GetState(&I2C_EXPBD_Handle) == HAL_I2C_STATE_RESET)
    {
        /* I2C_EXPBD peripheral configuration */
        I2C_EXPBD_Handle.Init.ClockSpeed = NUCLEO_I2C_EXPBD_SPEED;
        I2C_EXPBD_Handle.Init.DutyCycle = I2C_DUTYCYCLE_2;
        I2C_EXPBD_Handle.Init.OwnAddress1 = 0x33;
        I2C_EXPBD_Handle.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
        I2C_EXPBD_Handle.Instance = NUCLEO_I2C_EXPBD;

        /* Init the I2C */
        I2C_EXPBD_MspInit();
        ret_val = HAL_I2C_Init(&I2C_EXPBD_Handle);
    }

    return ret_val;
}

/**
 * @brief  Write a value in a register of the device through the bus
 * @param  pBuffer the pointer to data to be written
 * @param  Addr the device address on bus
 * @param  Reg the target register address to be written
 * @param  Size the size in bytes of the value to be written
 * @retval HAL status
 */

static HAL_StatusTypeDef I2C_EXPBD_WriteData(uint8_t* pBuffer, uint8_t Addr, uint8_t Reg, uint16_t Size)
{
    HAL_StatusTypeDef status = HAL_OK;

    status = HAL_I2C_Mem_Write(&I2C_EXPBD_Handle, Addr, (uint16_t)Reg, I2C_MEMADD_SIZE_8BIT, pBuffer, Size,
                               I2C_EXPBD_Timeout);

    /* Check the communication status */
    if(status != HAL_OK)
    {
        /* Execute user timeout callback */
        I2C_EXPBD_Error(Addr);
    }

    return status;
}


/**
 * @brief  Read the value of a register of the device through the bus
 * @param  pBuffer the pointer to data to be read
 * @param  Addr the device address on bus
 * @param  Reg the target register address to be read
 * @param  Size the size in bytes of the value to be read
 * @retval HAL status.
 */
static HAL_StatusTypeDef I2C_EXPBD_ReadData(uint8_t* pBuffer, uint8_t Addr, uint8_t Reg, uint16_t Size)
{
    HAL_StatusTypeDef status = HAL_OK;

    status = HAL_I2C_Mem_Read(&I2C_EXPBD_Handle, Addr, (uint16_t)Reg, I2C_MEMADD_SIZE_8BIT, pBuffer, Size,
                              I2C_EXPBD_Timeout);

    /* Check the communication status */
    if(status != HAL_OK)
    {
        /* Execute user timeout callback */
        I2C_EXPBD_Error(Addr);
    }

    return status;
}

/**
 * @brief  Manages error callback by re-initializing I2C
 * @param  Addr I2C Address
 * @retval None
 */
static void I2C_EXPBD_Error(uint8_t Addr)
{
    /* De-initialize the I2C comunication bus */
    HAL_I2C_DeInit(&I2C_EXPBD_Handle);

    /*FIXME: We need to wait a while in order to have I2C that works fine after deinit */
    HAL_Delay(1);

    /* Re-Initiaize the I2C comunication bus */
    I2C_EXPBD_Init();
}

/**
 * @brief  I2C MSP Initialization
 * @retval None
 */

static void I2C_EXPBD_MspInit(void)
{
    GPIO_InitTypeDef  GPIO_InitStruct;

    /* Enable I2C GPIO clocks */
    NUCLEO_I2C_EXPBD_SCL_SDA_GPIO_CLK_ENABLE();

    /* I2C_EXPBD SCL and SDA pins configuration -------------------------------------*/
    GPIO_InitStruct.Pin = NUCLEO_I2C_EXPBD_SCL_PIN | NUCLEO_I2C_EXPBD_SDA_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
    GPIO_InitStruct.Speed = GPIO_SPEED_FAST;
    GPIO_InitStruct.Pull  = GPIO_NOPULL;
    GPIO_InitStruct.Alternate  = NUCLEO_I2C_EXPBD_SCL_SDA_AF;
    HAL_GPIO_Init(NUCLEO_I2C_EXPBD_SCL_SDA_GPIO_PORT, &GPIO_InitStruct);

    /* Enable the I2C_EXPBD peripheral clock */
    NUCLEO_I2C_EXPBD_CLK_ENABLE();

    /* Force the I2C peripheral clock reset */
    NUCLEO_I2C_EXPBD_FORCE_RESET();

    /* Release the I2C peripheral clock reset */
    NUCLEO_I2C_EXPBD_RELEASE_RESET();

    /* Enable and set I2C_EXPBD Interrupt to the highest priority */
    HAL_NVIC_SetPriority(NUCLEO_I2C_EXPBD_EV_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(NUCLEO_I2C_EXPBD_EV_IRQn);

    /* Enable and set I2C_EXPBD Interrupt to the highest priority */
    HAL_NVIC_SetPriority(NUCLEO_I2C_EXPBD_ER_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(NUCLEO_I2C_EXPBD_ER_IRQn);
}

