/*
 * dsb18b20.h
 *
 *  Created on: Jan 16, 2021
 *      Author: jcera
 */

#ifndef INC_DSB18B20_H_
#define INC_DSB18B20_H_
#include <stdint.h>

void DS18B20_Init(GPIO_TypeDef *port, uint32_t pin, void (*make_input_cb)(void),  void (*make_output_cb)(void));
float DS18B20_sample(int check_crc);

extern const sTermEntry_t timEntry;

#endif /* INC_DSB18B20_H_ */
