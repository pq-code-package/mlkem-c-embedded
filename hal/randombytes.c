// SPDX-License-Identifier: Apache-2.0
#include "randombytes.h"

#ifdef RISCV
#include <esp_random.h>
int randombytes(uint8_t *obuf, size_t len) {
    // NOTE:
    // After enableing the internal entropy source (SAR ADC), random numbers are then generated based on the thermal noise in the system and the asynchronous clock mismatch.
    // For more detail please refer to https://docs.espressif.com/projects/esp-idf/en/v5.3/esp32c3/api-reference/system/random.html and the technical reference manual
    esp_fill_random(obuf, len);

    return 0;
}

#else
#include <libopencm3/stm32/rng.h>

int randombytes(uint8_t *obuf, size_t len) {
    union {
        unsigned char aschar[4];
        uint32_t asint;
    } random;

    while (len > 4) {
        random.asint = rng_get_random_blocking();
        *obuf++ = random.aschar[0];
        *obuf++ = random.aschar[1];
        *obuf++ = random.aschar[2];
        *obuf++ = random.aschar[3];
        len -= 4;
    }
    if (len > 0) {
        for (random.asint = rng_get_random_blocking(); len > 0; --len) {
            *obuf++ = random.aschar[len - 1];
        }
    }

    return 0;
}
#endif
