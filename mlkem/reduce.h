// SPDX-License-Identifier: Apache-2.0 or CC0-1.0
#ifndef REDUCE_H
#define REDUCE_H

#include <stdint.h>
#include "params.h"

#define barrett_reduce MLKEM_NAMESPACE(barrett_reduce)
int16_t barrett_reduce(int16_t a);

#endif
