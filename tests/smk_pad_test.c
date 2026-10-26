/* SPDX-License-Identifier: GPL-3.0-only
 * SLOOP Pro, Clipper Pro: Copyright (C) 2026 isod89 */
/* SMK-37 pads (firmware/hal/smk_input.h smk__pad): hits from simulated force readings, one every 1.8 ms as the scan
 * gives them, with noise. A hard hit sounds at once (the force falls: the peak has passed) with its peak's velocity;
 * a slow press within 5.4 ms; the pad's rebound after a hard hit is no second note; a roll is every note; a light
 * hold that wavers around the release threshold is one note; the velocity of one hit does not depend on where the
 * 1.8 ms readings fall on it; a pad left alone never fires. Built -O2 (the IRQ lock's asm is left out). */
#define BOARD_SMK37 1
#include "smk_input.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

static int fails;
static void check(int ok, const char *what) { printf("smk pad: %-86s %s\n", what, ok ? "ok" : "FAIL"); fails += !ok; }

enum { PAD = 9, BASE = 70 };
#define DT 1.8                                     /* ms between two readings of a pad (smk_sched) */
#define N(ms) ((int)((ms) / DT + 0.5))          /* readings in ms */                       /* a bottom-row pad: white key 1 (G3) */
static uint32_t bit_of(void) { return 1u << SMK_WHITE_NOTE[SMK_WHITE_OF_PAD(PAD)]; }
static int notes, offs, last_vel, first_at;        /* note-ons, note-offs, the last velocity, the reading of the first */
static uint32_t now, was;
static unsigned rnd = 1;
static int noise(int a) { rnd = rnd * 1103515245u + 12345u; return (int)((rnd >> 16) % (2u * (unsigned)a + 1u)) - a; }

static void reset(void)
{
    uint32_t i;
    smk_trim_defaults();
    fm1_in.pad_base[PAD] = 100u;                   /* (as fm1_input_init) */
    fm1_in.pad_st[PAD] = 0;
    fm1_in.pad_rt[PAD] = 0;
    for (i = 0; i < 400u; i++)                     /* settle the baseline on the idle reading */
        smk__pad(PAD, (uint32_t)(BASE + noise(3)));
    fm1_in.pnotes = fm1_in.notes = 0;
    notes = offs = last_vel = 0;
    first_at = -1;
    now = was = 0;
}
static void read1(int v)                           /* one reading; counts what it started or ended */
{
    uint32_t b;
    if (v < 0)
        v = 0;
    if (v > 1023)
        v = 1023;
    smk__pad(PAD, (uint32_t)v);
    b = fm1_in.pnotes & bit_of();
    if (b && !was) {
        notes++;
        last_vel = fm1_in.key_vel[SMK_KEY_F3 + SMK_WHITE_NOTE[SMK_WHITE_OF_PAD(PAD)]];
        if (first_at < 0)
            first_at = (int)now;
    }
    if (!b && was)
        offs++;
    was = b;
    now++;
}
static void idle(int n)
{
    while (n-- > 0)
        read1(BASE + noise(3));
}
/* a hit as a continuous force (a rubber pad over a force sensor: a rounded pulse): up to A above the base, tp ms to
 * its top, then down over td ms (the hold, if any, comes after); sampled every DT ms from phase ph (ms) */
static double hitf(double t, double a, double tp, double td)
{
    if (t <= 0)
        return 0;
    if (t < tp)
        return a * sin(1.5707963 * t / tp) * sin(1.5707963 * t / tp);
    if (t < tp + td)
        return a * cos(1.5707963 * (t - tp) / td) * cos(1.5707963 * (t - tp) / td);
    return 0;
}
static int hit_at_phase(double a, double tp, double td, double ph)   /* the velocity it gets (0: none) */
{
    int k;
    reset();
    for (k = 0; k < 60; k++)
        read1(BASE + (int)hitf(k * 1.8 + ph, a, tp, td) + noise(2));
    return notes == 1 ? last_vel : 0;
}

int main(void)
{
    int k, ok, vmin, vmax, crossing;

    reset();
    idle(5000);
    check(notes == 0 && offs == 0, "a pad left alone (noise +-3) never fires");

    /* a hard hit: up in 2 ms, down over 6 ms */
    reset();
    crossing = -1;
    for (k = 0; k < 40; k++) {
        int v = BASE + (int)hitf(k * DT + 1.0, 940.0, 2.0, 6.0) + noise(2);
        if (crossing < 0 && v > BASE + (int)SMK_PAD_ON)
            crossing = k;
        read1(v);
    }
    check(notes == 1 && offs == 1, "a hard hit: one note, one release");
    check(first_at - crossing <= 2, "a hard hit sounds the reading after its highest one (within 3.6 ms of the crossing)");
    check(last_vel >= 100, "a hard hit: a loud velocity (its peak, through the factory curve)");

    /* the rebound: a hard hit, then the pad comes back up to 40 % of it 12 ms and 30 ms after */
    reset();
    for (k = 0; k < N(19); k++)
        read1(BASE + (int)hitf(k * DT + 1.0, 830.0, 2.0, 4.0));
    idle(N(5));                                    /* (released by now) */
    for (k = 0; k < 4; k++)
        read1(BASE + 300 - k * 70);
    idle(N(10));
    for (k = 0; k < 3; k++)
        read1(BASE + 220 - k * 70);
    idle(50);
    check(notes == 1, "the pad's rebound after a hard hit (36 % and 26 %, 12 ms and 30 ms after the hit) is no second note");

    /* a roll: four equal hits 60 ms apart (16 a second); a softer hit (2/3) 50 ms after a hard one */
    reset();
    for (k = 0; k < 4; k++) {
        int j;
        for (j = 0; j < N(60); j++)
            read1(BASE + (int)hitf(j * DT + 1.0, 700.0, 2.0, 5.0) + noise(2));
    }
    check(notes == 4 && offs == 4, "a roll, four equal hits 60 ms apart: four notes");
    reset();
    for (k = 0; k < N(50); k++)
        read1(BASE + (int)hitf(k * DT + 1.0, 800.0, 2.5, 5.0) + noise(2));
    for (k = 0; k < N(50); k++)
        read1(BASE + (int)hitf(k * DT + 1.0, 540.0, 2.5, 5.0) + noise(2));
    check(notes == 2, "a softer hit (2/3) 50 ms after a hard one: a second note");

    /* a slow press, held 300 ms */
    reset();
    for (k = 0; k < N(29); k++)                    /* up over 29 ms */
        read1(BASE + 460 * k / N(29) + noise(4));
    for (k = 0; k < N(300); k++)
        read1(BASE + 460 + noise(8));
    crossing = notes;
    for (k = 0; k < 6; k++)
        read1(BASE + 460 - 80 * k);
    idle(20);
    check(crossing == 1 && notes == 1 && offs == 1, "a slow press held 300 ms: one note, released when let go");
    check(first_at <= 2 + (int)SMK_PAD_PEAKMAX, "a slow press sounds within the window (4 readings, 5.4 ms) of the crossing");

    /* a light hold that wavers around the release threshold: one note */
    reset();
    for (k = 0; k < 6; k++)
        read1(BASE + 300 - 40 * k);
    for (k = 0; k < N(400); k++)                   /* +5 .. +19 above the base */
        read1(BASE + 12 + noise(7));
    ok = notes == 1;
    idle(20);
    check(ok && notes == 1, "a light hold wavering around the release threshold: no second note");

    /* the same hit, the readings falling anywhere on it: the velocity hardly changes */
    for (ok = 1, k = 0; k < 2; k++) {
        double tp = k ? 6.0 : 3.0, ph;
        vmin = 200, vmax = 0;
        for (ph = 0; ph < DT; ph += 0.15) {
            int v = hit_at_phase(640.0, tp, 8.0, ph);
            if (!v)
                ok = 0;
            if (v < vmin)
                vmin = v;
            if (v > vmax)
                vmax = v;
        }
        printf("smk pad: (peak at %.0f ms: velocity %d .. %d over the reading's phase)\n", tp, vmin, vmax);
        if (vmax - vmin > (k ? 8 : 16))
            ok = 0;
    }
    check(ok, "one hit, wherever the 1.8 ms readings fall on it: one note, the velocity within a few steps");

    /* soft and hard: the velocity follows the force */
    {
        int soft = hit_at_phase(250.0, 3.0, 6.0, 0.7), mid = hit_at_phase(600.0, 3.0, 6.0, 0.7),
            hard = hit_at_phase(880.0, 3.0, 6.0, 0.7);
        printf("smk pad: (soft %d, medium %d, hard %d)\n", soft, mid, hard);
        check(soft > 0 && soft < mid && mid < hard, "soft < medium < hard");
    }

    puts(fails ? "SMK PAD TEST FAILED" : "smk pad test passed");
    return fails != 0;
}
