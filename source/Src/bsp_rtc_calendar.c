/**
  ******************************************************************************
  * File Name          : main.c
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


/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* RTC handler declaration */
static RTC_HandleTypeDef RtcHandle;


/* Private function variables ------------------------------------------------*/
static void RTC_CalendarConfig(void);
static void Calendar_WeekDayNum(const RTC_DateTypeDef* date_s, week_str_typedef *week_s);

/**
  * @brief  bsp rtc calendar init
  * @param  None
  * @retval None
  */
void BSP_RTC_Calendar_Init(void)
{
    /*##-1- Configure the RTC peripheral #######################################*/
    /* Configure RTC prescaler and RTC data registers */
    /* RTC configured as follows:
      - Hour Format    = Format 24
      - Asynch Prediv  = Value according to source clock
      - Synch Prediv   = Value according to source clock
      - OutPut         = Output Disable
      - OutPutPolarity = High Polarity
      - OutPutType     = Open Drain */
    RtcHandle.Instance = RTC; 
    RtcHandle.Init.HourFormat = RTC_HOURFORMAT_24;
    RtcHandle.Init.AsynchPrediv = RTC_ASYNCH_PREDIV;
    RtcHandle.Init.SynchPrediv = RTC_SYNCH_PREDIV;
    RtcHandle.Init.OutPut = RTC_OUTPUT_DISABLE;
    RtcHandle.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
    RtcHandle.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;

    if(HAL_RTC_Init(&RtcHandle) != HAL_OK)
    {
        /* Initialization Error */
        printf("RTC Init Error\r\n");
    }

    /*##-2- Check if Data stored in BackUp register1: No Need to reconfigure RTC#*/
    /* Read the BackUp Register 1 Data */
    if(HAL_RTCEx_BKUPRead(&RtcHandle, RTC_BKP_DR1) != 0x32F2)
    {  
        /* Configure RTC Calendar */
        RTC_CalendarConfig();
    }
}

/**
  * @brief  get the current time and date.
  * @param  date_s:  pointer to buffer
  * @param  rtc_time: pointer to buffer
  * @retval None
  */
void Calendar_Get(RTC_DateTypeDef *date_s,RTC_TimeTypeDef *rtc_time)
{
    RTC_DateTypeDef         sdatestructureget;
    RTC_TimeTypeDef         stimestructureget;

    /* Get the RTC current Time */
    HAL_RTC_GetTime(&RtcHandle, &stimestructureget, RTC_FORMAT_BIN);
    /* Get the RTC current Date */
    HAL_RTC_GetDate(&RtcHandle, &sdatestructureget, RTC_FORMAT_BIN);

    date_s->Year = sdatestructureget.Year;
    date_s->Month = sdatestructureget.Month;
    date_s->Date = sdatestructureget.Date;
    date_s->WeekDay = sdatestructureget.WeekDay;
    
    rtc_time->Hours = stimestructureget.Hours;
    rtc_time->Minutes = stimestructureget.Minutes;
    rtc_time->Seconds = stimestructureget.Seconds;
}

/**
  * @brief  set the current time and date.
  * @param  date_s:  pointer to buffer
  * @param  rtc_time: pointer to buffer
  * @retval None
  */
void Calendar_Set(RTC_DateTypeDef *date_s,RTC_TimeTypeDef *rtc_time)
{
    RTC_DateTypeDef         sdatestructure;
    RTC_TimeTypeDef         stimestructure;
    week_str_typedef        m_rtc_week;
    
    /* Convent the date to week */
    Calendar_WeekDayNum(&sdatestructure, &m_rtc_week);
    
    /* Get the date and time */
    sdatestructure.Year = date_s->Year;
    sdatestructure.Month = date_s->Month;
    sdatestructure.Date = date_s->Date;
    sdatestructure.WeekDay = m_rtc_week.u8WkDayNum;
    
    stimestructure.Hours = rtc_time->Hours;
    stimestructure.Minutes = rtc_time->Minutes;
    stimestructure.Seconds = rtc_time->Seconds;
    stimestructure.TimeFormat = RTC_HOURFORMAT12_AM;
    stimestructure.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
    stimestructure.StoreOperation = RTC_STOREOPERATION_RESET;
    
    /* Set Date */
    if(HAL_RTC_SetDate(&RtcHandle,&sdatestructure,RTC_FORMAT_BIN) != HAL_OK)
    {
        /* Initialization Error */
        printf("Set Date Err\r\n");
    } 
    
    /* Set Time */
    if(HAL_RTC_SetTime(&RtcHandle,&stimestructure,RTC_FORMAT_BIN) != HAL_OK)
    {
        /* Initialization Error */
        printf("Set Time Err\r\n");
    }
}

/**
  * @brief  Configure the current time and date.
  * @param  None
  * @retval None
  */
static void RTC_CalendarConfig(void)
{
    RTC_DateTypeDef sdatestructure;
    RTC_TimeTypeDef stimestructure;

    /*##-1- Configure the Date #################################################*/
    /* Set Date: Tuesday April 14th 2015 */
    sdatestructure.Year = 0x15;
    sdatestructure.Month = RTC_MONTH_APRIL;
    sdatestructure.Date = 0x14;
    sdatestructure.WeekDay = RTC_WEEKDAY_TUESDAY;

    if(HAL_RTC_SetDate(&RtcHandle,&sdatestructure,RTC_FORMAT_BIN) != HAL_OK)
    {
        /* Initialization Error */
//        Error_Handler(); 
    } 

    /*##-2- Configure the Time #################################################*/
    /* Set Time: 00:00:00 */
    stimestructure.Hours = 0x00;
    stimestructure.Minutes = 0x00;
    stimestructure.Seconds = 0x00;
    stimestructure.TimeFormat = RTC_HOURFORMAT12_AM;
    stimestructure.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
    stimestructure.StoreOperation = RTC_STOREOPERATION_RESET;

    if(HAL_RTC_SetTime(&RtcHandle,&stimestructure,RTC_FORMAT_BIN) != HAL_OK)
    {
        /* Initialization Error */
//        Error_Handler(); 
    }

    /*##-3- Writes a data in a RTC Backup data Register1 #######################*/
    HAL_RTCEx_BKUPWrite(&RtcHandle, RTC_BKP_DR1, 0x32F2);
}

/**
  * @brief  Determines the week number, the day number and the week day number.
  * @param  None
  * @retval None
  */
static void Calendar_WeekDayNum(const RTC_DateTypeDef* date_s, week_str_typedef *week_s)
{
  uint32_t a = 0, b = 0, c = 0, s = 0, e = 0, f = 0, g = 0, d = 0;
  int32_t n = 0;
  if (date_s->Month < 3)
  {
    a = date_s->Year - 1;
  }
  else
  {
    a = date_s->Year;
  }

  b = (a / 4) - (a / 100) + (a / 400);
  c = ((a - 1) / 4) - ((a - 1) / 100) + ((a - 1) / 400);
  s = b - c;
  if (date_s->Month < 3)
  {
    e = 0;
    f =  date_s->Date - 1 + 31 * (date_s->Month - 1);
  }
  else
  {
    e = s + 1;
    f = date_s->Date + (153 * (date_s->Month - 3) + 2) / 5 + 58 + s;
  }
  g = (a + b) % 7;
  d = (f + g - e) % 7;
  n = f + 3 - d;
  if (n < 0)
  {
    week_s->u8WkNum = 53 - ((g - s) / 5);
  }
  else if (n > (364 + s))
  {
    week_s->u8WkNum = 1;
  }
  else
  {
    week_s->u8WkNum = (n / 7) + 1;
  }
  week_s->u8WkDayNum = d + 1;
  week_s->u8DayNum = f + 1;
}




