/* SPDX-License-Identifier: GPL-3.0-only
 * SLOOP Pro, Clipper Pro: Copyright (C) 2026 isod89 */
/* The SMK-37's PEDAL jack as a sustain pedal (seq.c smk_perf_block, Clipper Pro; mux C address 6, smk_input.h fm1_in.pedal). No
 * hardware here: the host tests (tests/smk_pedal_test.c) run it.
 * A footswitch opens or shorts the jack; pedals come in both polarities (a switch under many of them). As most keyboards
 * do, the reading at power-on is the pedal UP: the first reading (once the inputs settle) is the rest, and the pedal is
 * DOWN while the reading is SMK_PED_ON or more away from it, UP again at SMK_PED_OFF or less (the gap between: no
 * change). A change counts after SMK_PED_N readings in a row on its side (contact bounce, a stray reading). A pedal
 * plugged in after power-on with the other polarity would read DOWN for good: after SMK_PED_STUCK_MS down without a
 * change, that reading becomes the rest (the pedal UP), so a sustain is never stuck for more than that.
 * Measured on an SMK-37 Pro with a footswitch (ChanceTheMaker's sloop-smk, its docs): released ~871, pressed 0. */
#pragma once
#include <stdint.h>

#define SMK_PED_ON 450u            /* this far from the rest: down */
#define SMK_PED_OFF 250u           /* this close to the rest: up */
#define SMK_PED_N 3u               /* readings in a row on the new side */
#define SMK_PED_STUCK_MS 120000u   /* down this long without a change: the reading is the new rest (up) */

typedef struct {
    uint16_t rest;                 /* the reading with the pedal up */
    uint8_t learnt, down, cand, cnt;
    uint32_t since;                /* ms when it went down */
} smk_ped_t;

/* one reading (0..1023) at ms -> 1 the pedal went down, 0 it went up, -1 no change */
static int smk_ped_sample(smk_ped_t *s, uint32_t raw, uint32_t ms)
{
    uint32_t d, side;
    if (raw > 1023u)
        return -1;
    if (!s->learnt) {
        s->rest = (uint16_t)raw;
        s->learnt = 1;
        s->down = s->cand = 0;
        s->cnt = 0;
        return -1;
    }
    d = raw > s->rest ? raw - s->rest : s->rest - raw;
    if (d >= SMK_PED_ON)
        side = 1u;
    else if (d <= SMK_PED_OFF)
        side = 0u;
    else {                                    /* (between: neither side, a reading on the way) */
        s->cnt = 0;
        return -1;
    }
    if (side != s->cand) {
        s->cand = (uint8_t)side;
        s->cnt = 0;
    }
    if (s->cnt < SMK_PED_N)
        s->cnt++;
    if (s->cnt < SMK_PED_N)
        return -1;
    if (side == s->down) {
        if (side && (uint32_t)(ms - s->since) >= SMK_PED_STUCK_MS) {   /* the other polarity, plugged in late */
            s->rest = (uint16_t)raw;
            s->down = s->cand = 0;
            s->cnt = 0;
            return 0;
        }
        return -1;
    }
    s->down = (uint8_t)side;
    if (side)
        s->since = ms;
    return (int)side;
}
