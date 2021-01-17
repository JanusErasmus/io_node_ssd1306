/*
 * value_counter.c
 *
 *  Created on: Jan 17, 2021
 *      Author: jcera
 */
#include "value_counter.h"

void value_counter_init(value_counter_t *ctx, float value)
{
    if(!ctx)
        return;

    ctx->prev_value = value;
    ctx->value = value;
}

void value_counter_set(value_counter_t *ctx, float new_value)
{
    if(!ctx)
        return;

    if(((ctx->prev_value - 0.5) > new_value) || (new_value > (ctx->prev_value + 0.5)))
    {
        ctx->prev_value = new_value;
    }

}

int value_counter_tick(value_counter_t *ctx)
{
    if(!ctx)
        return 0;

    int changed = 0;
    if(((ctx->prev_value - 0.1) > ctx->value) || (ctx->value > (ctx->prev_value + 0.1)))
    {
        if(ctx->prev_value > ctx->value)
        {
            changed = 1;
            ctx->value += 0.1;
        }

        if(ctx->prev_value < ctx->value)
        {
            changed = 1;
            ctx->value -= 0.1;
        }
    }

    return changed;
}
