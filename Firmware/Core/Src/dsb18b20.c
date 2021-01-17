/*
 * dsb18b20.c
 *
 *  Created on: Jan 16, 2021
 *      Author: jcera
 */
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include "utils/cli.h"
#include "utils/utils.h"
#include "gpio.h"

static GPIO_TypeDef *one_wire_port;
static uint32_t one_wire_pin;
static void (*make_input)(void) = 0;
static void (*make_output)(void) = 0;
static uint32_t ms_ticks = 500;
static uint32_t us_ticks = 500000;

uint32_t DWT_Delay_Init(void)
{
    ms_ticks = (HAL_RCC_GetHCLKFreq() / 1000);
    us_ticks = (ms_ticks / 1000);

    /* Disable TRC */
    CoreDebug->DEMCR &= ~CoreDebug_DEMCR_TRCENA_Msk; // ~0x01000000;
    /* Enable TRC */
    CoreDebug->DEMCR |=  CoreDebug_DEMCR_TRCENA_Msk; // 0x01000000;
    /* Disable clock cycle counter */
    DWT->CTRL &= ~DWT_CTRL_CYCCNTENA_Msk; //~0x00000001;
    /* Enable  clock cycle counter */
    DWT->CTRL |=  DWT_CTRL_CYCCNTENA_Msk; //0x00000001;
    /* Reset the clock cycle counter value */
    DWT->CYCCNT = 0;
    /* 3 NO OPERATION instructions */
    __ASM volatile ("NOP");
    __ASM volatile ("NOP");
    __ASM volatile ("NOP");
    /* Check if clock cycle counter has started */
    if(DWT->CYCCNT)
    {
       return 1; /*clock cycle counter started*/
    }
    else
    {
      return 0; /*clock cycle counter not started*/
    }

}

void DWT_Delay_ms(volatile uint32_t au32_milliseconds)
{
  uint32_t au32_initial_ticks = DWT->CYCCNT;
  au32_milliseconds *= ms_ticks;
  while ((DWT->CYCCNT - au32_initial_ticks) < au32_milliseconds);
}

static inline void DWT_Delay_us(volatile uint32_t au32_microseconds)
{
  uint32_t au32_initial_ticks = DWT->CYCCNT;
  au32_microseconds *= us_ticks;
  while ((DWT->CYCCNT - au32_initial_ticks) < au32_microseconds - us_ticks);
}

static int DS18B20_probe()
{
    if(!make_input)
        return 0;

    if(!make_output)
        return 0;

  make_output();
  HAL_GPIO_WritePin(one_wire_port, one_wire_pin, GPIO_PIN_SET);

  DWT_Delay_us(5);
  HAL_GPIO_WritePin(one_wire_port, one_wire_pin, GPIO_PIN_RESET);
  DWT_Delay_us(750);//480-960
  HAL_GPIO_WritePin(one_wire_port, one_wire_pin, GPIO_PIN_SET);
  make_input();
  int t = 0;
  while (HAL_GPIO_ReadPin(one_wire_port, one_wire_pin) == GPIO_PIN_SET)
  {
    t++;
    if (t > 60)
        return 0;

    DWT_Delay_us(1);
  }
  t = 480 - t;
  make_output();
  DWT_Delay_us(t);
  HAL_GPIO_WritePin(one_wire_port, one_wire_pin, GPIO_PIN_SET);
  return 1;
}

void DS18B20_Init(GPIO_TypeDef *port, uint32_t pin, void (*make_input_cb)(void),  void (*make_output_cb)(void))
{
    one_wire_port = port;
    one_wire_pin = pin;
    make_input = make_input_cb;
    make_output = make_output_cb;
    if(!DWT_Delay_Init())
    {
        printf(RED("DWT not starting") "\n");
        return;
    }

    if(DS18B20_probe())
    {
        printf(GREEN("DSB18B20 Probed OK") "\n");
    }
    else
    {
        printf(RED("Probed KO") "\n");
    }
}

static void DS18B20_Write(uint8_t data)
{
    make_output();
    for (int i = 0; i < 8; i++)
    {
        HAL_GPIO_WritePin(one_wire_port, one_wire_pin, GPIO_PIN_RESET);
        DWT_Delay_us(10);
        if (data & 1)
            HAL_GPIO_WritePin(one_wire_port, one_wire_pin, GPIO_PIN_SET);
        else
            HAL_GPIO_WritePin(one_wire_port, one_wire_pin, GPIO_PIN_RESET);

        data >>= 1;
        DWT_Delay_us(50);
        HAL_GPIO_WritePin(one_wire_port, one_wire_pin, GPIO_PIN_SET);
    }
}

static uint8_t DS18B20_Read()
{
    make_output();
    HAL_GPIO_WritePin(one_wire_port, one_wire_pin, GPIO_PIN_SET);
    DWT_Delay_us(2);
    uint8_t data = 0;
    for (int i = 0; i < 8; i++)
    {
        HAL_GPIO_WritePin(one_wire_port, one_wire_pin, GPIO_PIN_RESET);
        DWT_Delay_us(1);
        HAL_GPIO_WritePin(one_wire_port, one_wire_pin, GPIO_PIN_SET);
        make_input();
        DWT_Delay_us(5);
        data >>= 1;
        if (HAL_GPIO_ReadPin(one_wire_port, one_wire_pin) == GPIO_PIN_SET)
            data |= 0x80;

        DWT_Delay_us(55);
        make_output();
        HAL_GPIO_WritePin(one_wire_port, one_wire_pin, GPIO_PIN_SET);
    }
    return data;
}

static uint8_t crc8( uint8_t *addr, uint8_t len)
{
    uint8_t crc = 0;

    while (len--) {
        uint8_t inbyte = *addr++;
        for (uint8_t i = 8; i; i--) {
            uint8_t mix = (crc ^ inbyte) & 0x01;
            crc >>= 1;
            if (mix) crc ^= 0x8C;
            inbyte >>= 1;
        }
    }
    return crc;
}

uint8_t read_scratchpad(float *temperature, uint8_t *config)
{
    if (!DS18B20_probe())
        return 0;

    DS18B20_Write (0xCC); // Send skip ROM command
    DS18B20_Write (0xBE); // Read the register, a total of nine bytes, the first two bytes are the conversion value

    uint8_t scratch_pad[9];
    for(int k = 0; k < 9; k++)
    {
        scratch_pad[k] = DS18B20_Read ();
    }

    if(crc8(scratch_pad, 9))
    {
        printf("Incorrect CRC\n");
        diag_dump_buf(scratch_pad, 9);
        return 0;
    }

    int raw_temp = scratch_pad[0] | (scratch_pad[1] << 8);
//    printf("rawT: %d\n", raw_temp);
    *temperature = (float)raw_temp * 0.0625;

    *config = (scratch_pad[4] & 0x60) >> 5;

    return 1;
}

static void sample()
{
    if (!DS18B20_probe())
        return;

    DS18B20_Write (0xCC); // Send skip ROM command
    DS18B20_Write (0x44); // Send reading start conversion command
}

float DS18B20_sample(int check_crc)
{
    sample();
    //Wait for conversion
    HAL_Delay(800);

    float temperature = -9999;
    if(check_crc)
    {
        uint8_t config;
        if(read_scratchpad(&temperature, &config))
            return temperature;

    }
    else
    {
        if (!DS18B20_probe())
            return 0;

        DS18B20_Write (0xCC); // Send skip ROM command
        DS18B20_Write (0xBE); // Read the register, a total of nine bytes, the first two bytes are the conversion value

        int temp = DS18B20_Read();
        temp |= (DS18B20_Read() << 8);
        temperature = (float)temp  * 0.0625;
    }

    return temperature;
}

void dsb_debug(uint8_t argc, char **argv)
{
    uint8_t cfg = 0;
    float temp = 0;

    sample();
    HAL_Delay(800);

    if(read_scratchpad(&temp, &cfg))
    {
        printf("CFG : 0x%02X\n", cfg);
        printf("Temp: %0.3f\n", temp);
    }
    else
    {
        printf("CRC error maybe?\n");
    }
}
const sTermEntry_t timEntry =
{ "ds", "Sample DS sensor", dsb_debug };
