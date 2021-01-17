/*
 * commands.c
 *
 *  Created on: May 1, 2020
 *      Author: jerasmus
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Utils/cli.h"
#include "adc.h"
#include "gpio.h"
#include "rtc.h"
#include "dsb18b20.h"

void adcDebug(uint8_t argc, char **argv)
{
    float voltages[8];
    if(adc_sample(voltages) == HAL_OK)
    {
        for(int k = 0; k < 8; k++)
        {
            printf("ADC[%d]: %0.3f\n", k, voltages[k]);
        }
    }
}

const sTermEntry_t adcEntry =
{ "a", "Sample ADC", adcDebug };

//extern void sonoffDebug(uint8_t argc, char **argv);
//const sTermEntry_t sonoffEntry =
//{ "s", "Send msg via Sonoff", sonoffDebug };

const char *getDayName(int week_day)
{
  switch(week_day)
  {
  case RTC_WEEKDAY_MONDAY:
    return "Monday";
  case RTC_WEEKDAY_TUESDAY:
    return "Tuesday";
  case RTC_WEEKDAY_WEDNESDAY:
    return "Wednesday";
  case RTC_WEEKDAY_THURSDAY:
    return "Thursday";
  case RTC_WEEKDAY_FRIDAY:
    return "Friday";
  case RTC_WEEKDAY_SATURDAY:
    return "Saturday";
  case RTC_WEEKDAY_SUNDAY:
    return "Sunday";
  }

  return 0;
}

void rtc_debug(uint8_t argc, char **argv)
{
  RTC_TimeTypeDef sTime;
  RTC_DateTypeDef sDate;

  if(argc > 5)
  {
    printf("Setting date %d\n", atoi(argv[5]));

    sDate.WeekDay = RTC_WEEKDAY_MONDAY;
    sDate.Year = atoi(argv[1]) - 2000;
    sDate.Month = atoi(argv[2]);
    sDate.Date = atoi(argv[3]);
    sTime.Hours = atoi(argv[4]);
    sTime.Minutes = atoi(argv[5]);
    sTime.Seconds = 0;

    RCC->APB1ENR |= (RCC_APB1ENR_BKPEN | RCC_APB1ENR_PWREN);
    //PWR->CR |= PWR_CR_DBP;
    HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BIN);
    HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
  }


  HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
  HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);

  printf("RTC date: %s\n", getDayName(sDate.WeekDay));
  printf(" - %04d-%02d-%02d ", 2000 +sDate.Year, sDate.Month, sDate.Date);
  printf("%02d:%02d:%02d\n", sTime.Hours, sTime.Minutes, sTime.Seconds);
}
void rtc_debug(uint8_t argc, char **argv);
const sTermEntry_t rtcEntry =
{ "date", "RTC date", rtc_debug };

void in_debug(uint8_t argc, char **argv)
{
	uint8_t in[8];
	gpio_sample_in(in);
	for (int k = 0; k < 8; ++k)
	{
		printf("I[%d]: %u\n", k, in[k]);
	}
}
const sTermEntry_t inEntry =
{ "i", "Read Inputs", in_debug };

void out_debug(uint8_t argc, char **argv)
{
	uint8_t out[8] = {0};
	static int i = 0;
	out[i] = 1;
	gpio_set_out(out);
	for (int k = 0; k < 8; ++k)
	{
		printf("O[%d]: %u\n", k, out[k]);
	}

	if(++i > 7)
		i = 0;
}
const sTermEntry_t outEntry =
{ "o", "Set Outputs", out_debug };

void temp_debug(uint8_t argc, char **argv)
{
    float voltages[8];
    if(adc_sample(voltages) == HAL_OK)
    {
    	//first ADC is LM335
    	float temp = (voltages[0] * 100.0) - 273.0;
    	printf("Temperature is %0.3f\n", temp);
    }
}
const sTermEntry_t tempEntry =
{ "t", "Read Temp", temp_debug };

extern void tx_usart2(uint8_t *data, int len);

void rs_debug(uint8_t argc, char **argv)
{
	uint8_t data[64];
	memset(data, 0x55, 64);
    tx_usart2(data, 5);
}
const sTermEntry_t rsEntry =
{ "r", "TX on RS485", rs_debug };

const sTermEntry_t *cli_entries[] =
{
      &hEntry,
      &helpEntry,
      &histEntry,
      &rebootEntry,
      &bootEntry,
      &adcEntry,
	  &inEntry,
	  &outEntry,
	  &tempEntry,
	  &rsEntry,
      &rtcEntry,
      &timEntry,
	  0
};
