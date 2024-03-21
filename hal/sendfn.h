// SPDX-License-Identifier: Apache-2.0
#ifndef SENDFN_H
#define SENDFN_H

#include "hal.h"

__attribute__((unused))
static void send_unsigned(const char *s, unsigned int c) {
    int i = 0;
    char outs[11] = {0};
    if (c < 10) {
        outs[0] = '0' + c;
    } else {
        for (i = 9; c != 0; c /= 10, i -= 1) {
            unsigned d = c % 10;
            outs[i] = '0' + d;
        }
        i += 1;
    }
    hal_send_str(s);
    hal_send_str(outs + i);
}

__attribute__((unused))
static void send_unsignedll(const char *s, unsigned long long c) {
    int i = 0;
    char outs[21] = {0};
    if (c < 10) {
        outs[0] = '0' + c;
    } else {
        i = 19;
        while (c != 0) {
            /* Method adapted from ""hackers delight":
               Creates an approximation of q = (8/10) */
            unsigned long long q = (c >> 1) + (c >> 2);
            q = q + (q >> 4);
            q = q + (q >> 8);
            q = q + (q >> 16);
            q = q + (q >> 32);
            /* Now q = (1/10) */
            q = q >> 3;
            /* Since q contains an error due to the bits shifted out of the value, we
               only use it to determine the remainder.  */
            unsigned long long r = c - ((q << 3) + (q << 1));
            c = q;
            /* The remainder might be off by 10, so q may be off by 1 */
            if (r > 9) {
                c += 1;
                r -= 10;
            }
            outs[i] = '0' + (unsigned) r;
            i -= 1;
        }
        i += 1;
    }
    hal_send_str(s);
    hal_send_str(outs + i);
}

#endif /* SENDFN_H */
