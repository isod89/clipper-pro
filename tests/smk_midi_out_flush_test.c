/* SPDX-License-Identifier: GPL-3.0-only */
/* TRS MIDI OUT (smk_midi_out.h): a note-off dropped while a flush is already running (the ring full twice) used to be
 * merged into it, and the running pass had gone past that note (and past its channel's CC 64 / 123): the note stayed
 * on on the synth at the jack. Now the flush starts again from the top. The receiving synth is modelled from the
 * bytes on the jack. Exit status: notes left on. */
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
static uint32_t ut_busy;
static uint32_t rx_st, rx_n, rx_b[3], rx_on[16][128];
static void rx_byte(uint32_t b)
{
    if (b >= 0xF8u) return;
    if (b & 0x80u) { rx_st = b, rx_n = 0; return; }
    if (!rx_st) return;
    rx_b[rx_n++] = b;
    if (rx_n < 2u) return;
    if ((rx_st & 0xF0u) == 0x90u) rx_on[rx_st & 15u][rx_b[0]] = rx_b[1] != 0;
    else if ((rx_st & 0xF0u) == 0x80u) rx_on[rx_st & 15u][rx_b[0]] = 0;
    else if ((rx_st & 0xF0u) == 0xB0u && rx_b[0] == 123u) memset(rx_on[rx_st & 15u], 0, sizeof rx_on[0]);
    rx_n = 0, rx_st = 0;
}
static uint32_t ut_done(void) { return !ut_busy; }
static void ut_put(uint32_t b) { rx_byte(b); ut_busy = 4; }
#define SMO_TX_DONE() ut_done()
#define SMO_TX_PUT(b) ut_put(b)
#include "smk_midi_out.h"
#define PKT(cin, a, b, c) ((uint32_t)(cin) | (uint32_t)(a) << 8 | (uint32_t)(b) << 16 | (uint32_t)(c) << 24)
static void tick(uint32_t n) { while (n--) { if (ut_busy) ut_busy--; smo_tick(); } }
static void fill(void) { uint32_t d = smo.drops; while (smo.drops == d) smo_put(PKT(11, 0xBF, 1, 0)); }   /* the ring full */
int main(void)
{
    smo_put(PKT(9, 0x92, 60, 100));               /* ch 3 note 60 on, sent */
    tick(100);
    fill();
    smo_put(PKT(8, 0x82, 60, 0));                 /* its note-off finds the ring full: a flush */
    while (!(smo.fn && smo.fpos > 2049u))         /* the flush running, past channel 1's notes and its CC 64 / 123 */
        tick(1);
    smo_put(PKT(9, 0x90, 64, 100));               /* ch 1 note 64 on: goes out */
    fill();
    smo_put(PKT(8, 0x80, 64, 0));                 /* its note-off finds the ring full: flush asked again */
    tick(200000);                                 /* everything sent, the flush over */
    printf("smo_repro: ch 3 note 60 %s, ch 1 note 64 %s on the other synth (flush %u, ch 1 bit %u)\n",
           rx_on[2][60] ? "STUCK" : "off", rx_on[0][64] ? "STUCK" : "off", smo.flush, smo.on[0][2] >> 0 & 1u);
    printf("smo_flush: %s\n", rx_on[0][64] || rx_on[2][60] ? "FAIL: a note left on" : "no note left on after two dropped note-offs");
    return rx_on[0][64] || rx_on[2][60];
}
