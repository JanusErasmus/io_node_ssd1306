/*
 * value_counter.h
 *
 *  Created on: Jan 17, 2021
 *      Author: jcera
 */

#ifndef _COUNTER_H_
#define _COUNTER_H_


typedef struct {
    float prev_value;
    float value;
}value_counter_t;

void value_counter_init(value_counter_t *ctx, float value);
void value_counter_set(value_counter_t *ctx, float new_value);
int value_counter_tick();

#endif /* SRC_VALUE_COUNTER_H_ */
