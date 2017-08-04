/*
 * Copyright (c) 2015 Francesco Balducci
 *
 * This file is part of nucleo_tests.
 *
 *    nucleo_tests is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU Lesser General Public License as published by
 *    the Free Software Foundation, either version 3 of the License, or
 *    (at your option) any later version.
 *
 *    nucleo_tests is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU Lesser General Public License for more details.
 *
 *    You should have received a copy of the GNU Lesser General Public License
 *    along with nucleo_tests.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <sys\types.h>
#include <sys\time.h>
//#include <time.h>
#include <errno.h>
#include <assert.h>

#include "main.h"
#include "timespec.h"

#ifndef CLOCK_GETTIME_SYNC_DISABLED
#  include "timesync.h"
#endif

#define SYSTICK_NSEC 1000000
#define SYSTICK_FREQ_HZ (NSECS_IN_SEC/SYSTICK_NSEC)
static TIM_HandleTypeDef Tim2Handle;

void clock_gettime_systick_init(void);

static volatile struct timespec realtime;
static volatile struct timespec monotonic;

static volatile int timer_update_flag;
static const struct timespec systick_step = {
    .tv_sec = 0,
    .tv_nsec = SYSTICK_NSEC
};
static volatile size_t seconds_since_boot=0;
static volatile size_t hal_timer_delay_coutner=0;
static RTC_HandleTypeDef        hRTC_Handle;
#define RTC_ASYNCH_PREDIV       0U
#define RTC_SYNCH_PREDIV        31U
static HAL_StatusTypeDef RTC_Setup() {
	  __IO uint32_t counter = 0U;

	  RCC_OscInitTypeDef        RCC_OscInitStruct;
	  RCC_PeriphCLKInitTypeDef  PeriphClkInitStruct;
	  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSE;
	  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
	  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
	  PeriphClkInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
	  if(HAL_RCC_OscConfig(&RCC_OscInitStruct) == HAL_OK)
	   {
	     PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_RTC;
	     if(HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) == HAL_OK)
	     {
	       /* Enable RTC Clock */
	       __HAL_RCC_RTC_ENABLE();
	       /* The time base should be 1ms
	          Time base = ((RTC_ASYNCH_PREDIV + 1) * (RTC_SYNCH_PREDIV + 1)) / RTC_CLOCK
	          HSE as RTC clock
	            Time base = ((99 + 1) * (9 + 1)) / 1Mhz
	                      = 1ms
	          LSE as RTC clock
	            Time base = ((31 + 1) * (0 + 1)) / 32.768Khz
	                      = ~1ms
	          LSI as RTC clock
	            Time base = ((31 + 1) * (0 + 1)) / 32Khz
	                      = 1ms
	       */
	       hRTC_Handle.Instance = RTC;
	       hRTC_Handle.Init.HourFormat = RTC_HOURFORMAT_24;
	       hRTC_Handle.Init.AsynchPrediv = RTC_ASYNCH_PREDIV;
	       hRTC_Handle.Init.SynchPrediv = RTC_SYNCH_PREDIV;
	       hRTC_Handle.Init.OutPut = RTC_OUTPUT_DISABLE;
	       hRTC_Handle.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
	       hRTC_Handle.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
	       HAL_RTC_Init(&hRTC_Handle);

	       /* Disable the write protection for RTC registers */
	       __HAL_RTC_WRITEPROTECTION_DISABLE(&hRTC_Handle);

	       /* Disable the Wake-up Timer */
	       __HAL_RTC_WAKEUPTIMER_DISABLE(&hRTC_Handle);

	       /* In case of interrupt mode is used, the interrupt source must disabled */
	       __HAL_RTC_WAKEUPTIMER_DISABLE_IT(&hRTC_Handle,RTC_IT_WUT);

	       /* Wait till RTC WUTWF flag is set  */
	       while(__HAL_RTC_WAKEUPTIMER_GET_FLAG(&hRTC_Handle, RTC_FLAG_WUTWF) == RESET)
	       {
	         if(counter++ == (SystemCoreClock /48U))
	         {
	           return HAL_ERROR;
	         }
	       }

	       /* Clear PWR wake up Flag */
	       __HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);

	       /* Clear RTC Wake Up timer Flag */
	       __HAL_RTC_WAKEUPTIMER_CLEAR_FLAG(&hRTC_Handle, RTC_FLAG_WUTF);
	       HAL_NVIC_SetPriority(RTC_WKUP_IRQn, 0U, 0U);
	       HAL_NVIC_EnableIRQ(RTC_WKUP_IRQn);



	       return HAL_OK;
	     }
		  return HAL_ERROR;
	   }
}
static void clock_handler();

void SysTick_Handler(void)
{
	clock_handler();
}


void TIM2_IRQHandler(void)
{
  HAL_TIM_IRQHandler(&Tim2Handle);
}
void HAL_TIM_OC_DelayElapsedCallback(TIM_HandleTypeDef *htim){
	if(htim == &Tim2Handle) {
		 __HAL_TIM_DISABLE_IT(htim, TIM_IT_CC1);
		TIM_CCxChannelCmd(Tim2Handle.Instance, TIM_CHANNEL_1, TIM_CCx_DISABLE);
		hal_timer_delay_coutner =0;
	}
}
static void SetTim3OC1Deplay(uint32_t delay) {
	hal_timer_delay_coutner = delay;
	if(delay > 0 && delay < Tim2Handle.Instance->ARR){
		delay += __HAL_TIM_GET_COUNTER(&Tim2Handle);
		Tim2Handle.Instance->CCR1 = delay;
		 __HAL_TIM_ENABLE_IT(&Tim2Handle, TIM_IT_CC1);
		TIM_CCxChannelCmd(Tim2Handle.Instance, TIM_CHANNEL_1, TIM_CCx_ENABLE);
	}
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim == &Tim2Handle) {
		++seconds_since_boot;
		clock_handler();
		if(hal_timer_delay_coutner > Tim2Handle.Instance->ARR) {
			SetTim3OC1Deplay(hal_timer_delay_coutner - Tim2Handle.Instance->ARR);
		}
	}
}
uint32_t GetTimerFreq() {
	 RCC_ClkInitTypeDef    clkconfig;
	 uint32_t              pFLatency;
	 /* Get clock configuration */
	 HAL_RCC_GetClockConfig(&clkconfig, &pFLatency);
	 uint32_t freq = HAL_RCC_GetPCLK1Freq();
	  /* Get APB1 prescaler */
	 switch(clkconfig.APB1CLKDivider){
	 case RCC_HCLK_DIV1: return freq;
	 case RCC_HCLK_DIV2: return freq*2;
	 case RCC_HCLK_DIV4: return freq*4;
	 case RCC_HCLK_DIV8: return freq*8;
	 case RCC_HCLK_DIV16: return freq*16;
	 default:
		 return freq*2;  // never should get here
	 }

}
uint32_t RTC_Seconds() {
    RTC_TimeTypeDef sTime;
    HAL_RTC_GetTime(&hRTC_Handle, &sTime, RTC_FORMAT_BIN);
    return sTime.Seconds;
}
// hall timer
HAL_StatusTypeDef HAL_InitTick(uint32_t TickPriority){
	(void)TickPriority;

	  /*##-1- Enable peripherals and GPIO Clocks #################################*/
	  /* TIMx Peripheral clock enable */
	  __HAL_RCC_TIM2_CLK_ENABLE();

	  /*##-2- Configure the NVIC for TIMx ########################################*/
	  /* Set the TIMx priority */
	  HAL_NVIC_SetPriority(TIM2_IRQn, 0, 1);

	  /* Enable the TIMx global Interrupt */
	  HAL_NVIC_EnableIRQ(TIM2_IRQn);

	  /* Compute the prescaler value to have TIM3 counter clock equal to 10 KHz */
	 // volatile uint32_t uwPrescalerValue = (uint32_t) ((SystemCoreClock /2) / 10000) - 1;

	  // 1 a sec
	  /* Compute the prescaler value to have TIM6 counter clock equal to 1MHz */
	  // if we are /2 check template if we want to change this
	//  uint32_t uwPrescalerValue = (uint32_t) (((SystemCoreClock/2) / 1000000U) - 1U);
	  uint32_t timer_frequency = GetTimerFreq();
	  /* Compute the prescaler value to have TIM2 counter clock equal to 1MHz */
	  uint32_t uwPrescalerValue = (uint32_t) ((timer_frequency / 1000000U) - 1U);


	  Tim2Handle.Instance = TIM2;

	  /* Initialize TIM3 peripheral as follows:
		   + Period = 500 - 1
		   + Prescaler = ((SystemCoreClock/2)/10000) - 1
		   + ClockDivision = 0
		   + Counter direction = Up
	  */
	  Tim2Handle.Init.Period = 1000000U - 1; // once a sec
	  Tim2Handle.Init.Prescaler = uwPrescalerValue;
	  Tim2Handle.Init.ClockDivision = 0;
	  Tim2Handle.Init.CounterMode = TIM_COUNTERMODE_UP;
	  assert(HAL_TIM_Base_Init(&Tim2Handle) == HAL_OK);
	  assert(HAL_TIM_Base_Start_IT(&Tim2Handle) == HAL_OK);



	  assert(RTC_Setup() == HAL_OK);


      uint32_t sec_start = RTC_Seconds()+1;
      while(RTC_Seconds()!=sec_start);
      volatile uint32_t tick_start = TIM2->CNT;
      while(RTC_Seconds()!=(sec_start+1));
      volatile uint32_t tickend = TIM2->CNT;
	  return HAL_OK;
}

void HAL_SuspendTick(void){}
void HAL_ResumeTick(void){}



uint32_t HAL_GetTick() {
	uint32_t sec,usec;
	do {
		sec = seconds_since_boot;
		usec = __HAL_TIM_GET_COUNTER(&Tim2Handle);
	} while(sec != seconds_since_boot);
	return (sec * 1000U) + (usec/1000U);
}
void HAL_Delay(__IO uint32_t Delay)
{
#ifdef USE_OC
	SetTim3OC1Deplay(Delay);
	while(hal_timer_delay_coutner) __WFI();
#else
	uint32_t tickstart = HAL_GetTick() ;
	while((HAL_GetTick()  - tickstart) < Delay);
#endif
	// wait on interrupt.. heck might even be able to go to sleep here

#if 0
	if(Delay< 0x10000) {
		Tim3Handle.Instance->CCR1 = __HAL_TIM_GET_COUNTER(&Tim3Handle) + Delay;

		TIM_CCxChannelCmd(Tim3Handle.Instance, TIM_CHANNEL_1, TIM_CCx_ENABLE);

		int wait = __HAL_TIM_GET_COUNTER(&Tim3Handle) + Delay;
		while((int)(__HAL_TIM_GET_COUNTER(&Tim3Handle)-wait)>=0);
	} else {
		uint32_t tickstart = HAL_GetTicks() ;
		while((HAL_GetTicks()  - tickstart) < Delay);
	}
#endif
}


/**
  * @brief Resume Tick increment.
  * @note In the default implementation , SysTick timer is the source of time base. It is
  *       used to generate interrupts at regular time intervals. Once HAL_ResumeTick()
  *       is called, the SysTick interrupt will be enabled and so Tick increment
  *       is resumed.
  * @note This function is declared as __weak to be overwritten in case of other
  *       implementations in user file.
  * @retval None
  */



// we use TIM2 for the time counter
static uint32_t I_GetTimerTicks() {
	//return  SysTick->VAL;
	return __HAL_TIM_GET_COUNTER(&Tim2Handle);
}






static int clock_gettime_mutex;
static void clock_gettime_mutex_lock() {
	int ret = clock_gettime_mutex +1;
	while(clock_gettime_mutex>0);
	clock_gettime_mutex = ret;
}
static void clock_gettime_mutex_unlock() {
	int ret = clock_gettime_mutex -1;
	assert(clock_gettime_mutex>0);
	clock_gettime_mutex = ret;
}
static
volatile struct timespec *clock_get(clockid_t clock_id)
{
    volatile struct timespec *clk;

    switch(clock_id)
    {
        case CLOCK_MONOTONIC:
            clk = &monotonic;
            break;
        case CLOCK_REALTIME:
            clk = &realtime;
            break;
        default:
            clk = NULL;
            break;
    }
    return clk;
}

static
void systick_fraction_to_timespec(uint32_t fraction, struct timespec *tp)
{
    uint32_t ticks;
    ticks = (SystemCoreClock/SYSTICK_FREQ_HZ)- fraction;
    tp->tv_sec = 0; /* assuming  SYSTICK_NSEC < NSECS_IN_SEC */
    tp->tv_nsec = ticks * (SystemCoreClock/SYSTICK_FREQ_HZ);
}

#ifndef CLOCK_GETTIME_SYNC_DISABLED

/* Empty default that does nothing, in case
 * there is no implementation of timesync
 * linked in the program.
 */
__attribute__((__weak__))
int timesync_timespec(struct timespec *t)
{
    (void)t;
    return 0;
}

#endif

int clock_gettime(clockid_t clock_id, struct timespec *tp)
{
    int ret;
    volatile struct timespec *clk;

    clk = clock_get(clock_id);
    if (clk == NULL)
    {
        ret = -1;
        errno = EINVAL;
    }
    else
    {
        int flag_before;
        int flag_after;
        uint32_t fraction_ticks;
        struct timespec fraction_ts;

        do {
            flag_before = timer_update_flag;
            *tp = *clk;
            fraction_ticks = I_GetTimerTicks();
            flag_after = timer_update_flag;
            /* if they are the same, no systick occurred.
             * note that seqlock is unnecessary because
             * systick is an interrupt, not a thread.
             */
        } while (flag_before != flag_after);
        systick_fraction_to_timespec(fraction_ticks, &fraction_ts);
        timespec_incr(tp, &fraction_ts);
#ifndef CLOCK_GETTIME_SYNC_DISABLED
        if (clock_id == CLOCK_REALTIME)
        {
            /* We use clock_gettime_mutex
             * so that we do not get stuck in a recursion.
             * Note that this Implementation is not completely thread-safe.
             */
            if (!clock_gettime_mutex)
            {
                int sync_ret;

                clock_gettime_mutex_lock();

                sync_ret = timesync_timespec(tp);

                (void)sync_ret; /* ignore */

                clock_gettime_mutex_unlock();
            }
        }
#endif
        ret = 0;
    }
    return ret;
}

int clock_settime(clockid_t clock_id, const struct timespec *tp)
{
    int ret;
    volatile struct timespec *clk;

    clk = clock_get(clock_id);
    if (clk == NULL)
    {
        ret = -1;
        errno = EINVAL;
    }
    else if (clock_id == CLOCK_MONOTONIC)
    {
        ret = -1;
        errno = EINVAL;
    }
    else if (tp == NULL)
    {
        ret = -1;
        errno = EINVAL;
    }
    else if (tp->tv_nsec < 0)
    {
        ret = -1;
        errno = EINVAL;
    }
    else if (tp->tv_nsec >= NSECS_IN_SEC)
    {
        ret = -1;
        errno = EINVAL;
    }
    else
    {
        int flag_before;
        int flag_after;
        do {
            flag_before = timer_update_flag;
            *clk = *tp;
            flag_after = timer_update_flag;
            /* if they are the same, no systick occurred.
             * note that seqlock is unnecessary because
             * systick is an interrupt, not a thread.
             */
        } while (flag_before != flag_after);
        ret = 0;
    }

    return ret;
}

int clock_getres(clockid_t clock_id, struct timespec *res)
{
    int ret;

    if (clock_get(clock_id) == NULL)
    {
        ret = -1;
        errno = EINVAL;
    }
    else
    {
        ret = 0;
        if (res != NULL)
        {
            systick_fraction_to_timespec(1, res);
        }
    }
    return ret;
}

static
void sys_tick_incr(clockid_t clock_id)
{
    struct timespec *clk;

    /* we can discard volatile because we are already in sys_tick_handler */
    clk = (struct timespec *)clock_get(clock_id);

    timespec_incr(clk, &systick_step);
}



void clock_handler(void)
{
    sys_tick_incr(CLOCK_MONOTONIC);
    sys_tick_incr(CLOCK_REALTIME);
    timer_update_flag++;
}

void setup_counter() {

}


void clock_gettime_systick_init(void)
{
	//SysTick_Config(SystemCoreClock / SYSTICK_FREQ_HZ);
	setup_counter();
}

