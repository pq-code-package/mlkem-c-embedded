
// SPDX-License-Identifier: Apache-2.0 or CC0-1.0#ifndef VERIFY_H
#ifndef VERIFY_H
#define VERIFY_H

#include <stddef.h>
#include <stdint.h>
#include "params.h"

#define verify MLKEM_NAMESPACE(verify)
int verify(const uint8_t *a, const uint8_t *b, size_t len);

#define cmov MLKEM_NAMESPACE(cmov)
void cmov(uint8_t *r, const uint8_t *x, size_t len, uint8_t b);

#endif
