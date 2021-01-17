/*
 * temp_display.c
 *
 *  Created on: Jan 17, 2021
 *      Author: jcera
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "main.h"
#include "adc.h"
#include "ssd1306.h"
#include "dsb18b20.h"
#include "value_counter.h"

#define RUN_PERIOD 100
#define SAMPLE_PERIOD 10000

static int sample_tick = 0;
static int run_tick = 0;

static value_counter_t ds_counter;
static value_counter_t lm2_counter;
static value_counter_t lm1_counter;

static int x_offset = 0;
char data[512];


static void make_output()
{
//    GPIO_InitTypeDef GPIO_InitStruct = {0};
//    GPIO_InitStruct.Pin = IN_7_Pin;
//    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
//    GPIO_InitStruct.Pull = GPIO_NOPULL;
//    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
//    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    // Streamlined configuration for PB07 as output OD
    // Set MODE7 = 0x03 (50 MHz Output) , CNF7 = 0x01 (open drain)
    GPIOB->CRL |= 0x03 << 28 ;
}

static void make_input()
{
//    GPIO_InitTypeDef GPIO_InitStruct = {0};
//    GPIO_InitStruct.Pin = IN_7_Pin;
//    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
//    GPIO_InitStruct.Pull = GPIO_NOPULL;
//    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
//    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    //Streamlined configuration for PB07 as input
    // Set MODE7 = 0x00 (Input) , CNF7 = 0x01 (floating input)
    GPIOB->CRL &= ~(0x03 << 28) ;
}


void display_init()
{
    value_counter_init(&ds_counter, 25);
    value_counter_init(&lm1_counter, 25);
    value_counter_init(&lm2_counter, 25);

    DS18B20_Init(IN_7_GPIO_Port, IN_7_Pin, make_input, make_output);

    ssd1306_Init();
    ssd1306_SetCursor(0, 0);
    ssd1306_WriteString("SSD1306", Font_7x10, White);
    ssd1306_SetCursor(0, 12);
    ssd1306_WriteString("Besig...", Font_11x18, White);
    ssd1306_UpdateScreen();
}

//Run every 100 ms
void display_run()
{
    if(run_tick > HAL_GetTick())
        return;

    run_tick = HAL_GetTick() + RUN_PERIOD;


    int changed = 0;
    if(sample_tick < HAL_GetTick())
    {
        sample_tick = HAL_GetTick() + SAMPLE_PERIOD;

        changed = 1;
        if(x_offset++ > 65)
        {
            x_offset = 0;
        }

        //Sample LM sensors
        float voltages[8] = {0};
        adc_sample(voltages);
        float lm1_temp = (voltages[0] * 100.0) - 273.0;
        float lm2_temp = (voltages[1] * 100.0) - 273.0;

        //sample DS18B20
        float ds_temp = DS18B20_sample(1);

        value_counter_set(&ds_counter, ds_temp);
        value_counter_set(&lm1_counter, lm1_temp);
        value_counter_set(&lm2_counter, lm2_temp);
    }

    changed |= value_counter_tick(&ds_counter);
    changed |= value_counter_tick(&lm1_counter);
    changed |= value_counter_tick(&lm2_counter);


    if(changed)
    {
        ssd1306_Fill(Black);
        sprintf(data, "LM1: %0.1f", lm1_counter.value);
        ssd1306_SetCursor(x_offset, 0);
        ssd1306_WriteString(data, Font_7x10, White);
        sprintf(data, "     % .1f", lm1_counter.value - ds_counter.value);
        ssd1306_SetCursor(x_offset, 11);
        ssd1306_WriteString(data, Font_7x10, White);

        sprintf(data, "DS : %0.1f", ds_counter.value);
        ssd1306_SetCursor(x_offset, 27);
        ssd1306_WriteString(data, Font_7x10, White);

        sprintf(data, "     % .1f", lm2_counter.value - ds_counter.value);
        ssd1306_SetCursor(x_offset, 42);
        ssd1306_WriteString(data, Font_7x10, White);
        sprintf(data, "LM2: %0.1f", lm2_counter.value);
        ssd1306_SetCursor(x_offset, 53);
        ssd1306_WriteString(data, Font_7x10, White);
        ssd1306_UpdateScreen();
    }
}
