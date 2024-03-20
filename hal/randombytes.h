// SPDX-License-Identifier: Apache-2.0
#ifndef RANDOMBYTES_H
#define RANDOMBYTES_H

#include <stdint.h>
#include <unistd.h>

int randombytes(uint8_t *output, size_t n);

#endif /* RANDOMBYTES_H */
