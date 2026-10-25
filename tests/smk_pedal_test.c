/* SPDX-License-Identifier: GPL-3.0-only
 * SLOOP Pro: Copyright (C) 2026 isod89 */
/* SLOOP Pro's sustain pedal (firmware/src/smk_pedal.h): both polarities learnt at power-on, bounce and stray readings,
 * the gap between the thresholds, a pedal of the other polarity plugged in late never stuck */
#include <stdio.h>
#include "smk_pedal.h"

static int fails;
static void ok(int c, const char *what)
{
    printf("%-74s %s\n", what, c ? "ok" : "FAIL");
    fails += !c;
}
/* n readings of raw from ms on (one a ms); -> the last change (-1 none), *changes counted */
static int feed(smk_ped_t *s, unsigned raw, int n, unsigned *ms, int *changes)
{
    int last = -1, e;
    while (n-- > 0)
        if ((e = smk_ped_sample(s, raw, (*ms)++)) >= 0)
            last = e, (*changes)++;
    return last;
}

int main(void)
{
    smk_ped_t s = {0};
    unsigned ms = 0;
    int ch = 0, e;
    /* the measured footswitch: up ~871, down 0 */
    ok(feed(&s, 871, 50, &ms, &ch) < 0 && !ch && s.rest == 871, "power-on: the reading is the rest (up), no change");
    e = feed(&s, 0, 2, &ms, &ch);
    ok(e < 0 && !ch, "two readings down: not yet (bounce)");
    e = feed(&s, 0, 1, &ms, &ch);
    ok(e == 1 && ch == 1, "the third: down");
    ok(feed(&s, 0, 500, &ms, &ch) < 0 && ch == 1, "held down: no more changes");
    ch = 0;
    ok(feed(&s, 600, 50, &ms, &ch) < 0 && !ch, "a reading in the gap (271 away): no change");
    ok(feed(&s, 871, 3, &ms, &ch) == 0 && ch == 1, "back to the rest: up");
    ch = 0;
    for (int i = 0; i < 40; i++) {                      /* contact bounce: down, up, down... one reading each */
        feed(&s, i & 1 ? 871 : 0, 1, &ms, &ch);
    }
    ok(!ch, "bounce (alternate readings): ignored");
    ok(feed(&s, 2000, 10, &ms, &ch) < 0 && !ch, "a reading out of range: ignored");
    /* the other polarity: up 0, down ~871 */
    smk_ped_t r = {0};
    ms = 0, ch = 0;
    feed(&r, 3, 20, &ms, &ch);
    ok(feed(&r, 880, 3, &ms, &ch) == 1 && feed(&r, 5, 3, &ms, &ch) == 0 && ch == 2, "the other polarity: down and up");
    /* nothing in the jack, a noisy reading around a level: never a change */
    {
        int c = 0;
        smk_ped_t m = {0};
        for (unsigned i = 0, t = 0; i < 20000; i++)
            c += smk_ped_sample(&m, 500 + (i * 7919u) % 401u - 200u, t++) >= 0;
        ok(!c, "an open jack with +-200 of noise: never a change");
    }
    /* plugged in late with the other polarity: reads down for good, let go after SMK_PED_STUCK_MS */
    smk_ped_t l = {0};
    ms = 0, ch = 0;
    feed(&l, 871, 10, &ms, &ch);
    ok(feed(&l, 0, 3, &ms, &ch) == 1, "a pedal of the other polarity plugged in: reads down");
    ok(feed(&l, 0, (int)SMK_PED_STUCK_MS - 10, &ms, &ch) < 0, "... held (sustained) until the guard time");
    e = feed(&l, 0, 20, &ms, &ch);
    ok(e == 0 && l.rest == 0, "... then that reading is the rest: up, the sustain let go");
    ch = 0;
    ok(feed(&l, 871, 3, &ms, &ch) == 1 && feed(&l, 0, 3, &ms, &ch) == 0 && ch == 2, "... and the pedal works from then on");
    printf(fails ? "SMK PEDAL TEST FAILED (%d)\n" : "smk pedal test passed\n", fails);
    return fails != 0;
}
