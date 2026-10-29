/* SPDX-License-Identifier: GPL-3.0-only */
/* Clipper Pro FADER BANK D MIX, fader 4 LEVEL (SLOOP Pro issue #7, the same mapping here): the sound's LEVEL over the whole
 * travel: silent at the bottom, LEVEL's top at the top, every position above the bottom heard, louder all the way up,
 * half the travel the power-on LEVEL (-4 dB). LEVEL is in 1/2 dB steps (0..127): the travel read as LEVEL steps (as CC 7
 * sets it) put half of it at -24 dB and under, heard as muted. The other faders as before (CC 74: as the CC).
 * firmware/src/ui_fbank.c fb_apply.
 *   cc -O2 -w -DCLIPPER_SYNTH=1 -Ibuild/gen -Ifirmware/src -Ifirmware/hal tests/fader_level_test.c -lm */
#define UI_PAGES_HARNESS_ONLY 1
#include "ui_pages_test.c"
static int pick_shown(void) { return 0; }           /* (ui_pick.c, SMK build: SOUNDS not up) */
#include "../firmware/src/ui_fbank.c"

#define FB_MIX 3u                                    /* bank D */
#define FB_LVL 3u                                    /* its fader 4 */
static int32_t db10(uint32_t f)                      /* the sound's LEVEL with the fader at f, dB x 10 (-999: silent) */
{
    fb_apply(FB_MIX, FB_LVL, f);
    return LEVEL_DB_X10[trk[0].p[P_LEVEL] & 127];
}
static int32_t peak(uint32_t f)                      /* a held chord through the mix, the fader at f */
{
    static int32_t o[2 * CTL];
    uint32_t b, i;
    int32_t pk = 0;
    fb_apply(FB_MIX, FB_LVL, f);
    for (b = 0; b < 40u; b++) {                      /* (the LEVEL glide settles) */
        mix_block(o, CTL);
        if (b >= 20u)
            for (i = 0; i < 2u * CTL; i++)
                pk = abs(o[i]) > pk ? abs(o[i]) : pk;
    }
    return pk;
}

int main(int argc, char **argv)
{
    uint32_t f, mono = 1, heard = 1;
    int32_t top, prev = -1000;
    int16_t *vp;
    outdir = argc > 1 ? argv[1] : "build/host";
    panel = PANEL_DEFAULT;
    layers_init();
    host_tracks_init();
    song.master_q12 = 4096;
    for (f = 0; f < NPART; f++) { set_engine_of(&trk[f], TRK_DEF[f][0]); apply_preset_to(&trk[f], TRK_DEF[f][1]); trk[f].engine = trk[f].eng_req; }
    go_home(); ui.force = 1; frame();
    check(fb_param(FB_MIX, FB_LVL, &vp) == &TP[P_LEVEL] && vp == &trk[0].p[P_LEVEL], "bank D, fader 4: the sound's LEVEL");

    fb_apply(FB_MIX, FB_LVL, 64);                    /* (a move: the fader then goes to the bottom) */
    fb_apply(FB_MIX, FB_LVL, 0);
    check(trk[0].p[P_LEVEL] == 0, "the fader at the bottom: silent (LEVEL 0)");
    fb_apply(FB_MIX, FB_LVL, 127);
    check(trk[0].p[P_LEVEL] == TP[P_LEVEL].max, "the fader at the top: LEVEL's top");
    top = db10(127);
    for (f = 1; f <= 127u; f++) {
        int32_t d = db10(f);
        mono &= d >= prev;
        heard &= trk[0].p[P_LEVEL] > 0;
        prev = d;
    }
    check(mono && heard, "every position above the bottom: heard (LEVEL > 0), louder all the way up");
    printf("fader_level: the fader at 1/4, 1/2, 3/4: %+.1f, %+.1f, %+.1f dB (LEVEL %d at 1/2; the top %+.1f dB)\n",
           db10(32) / 10.0, db10(64) / 10.0, db10(95) / 10.0, (fb_apply(FB_MIX, FB_LVL, 64), trk[0].p[P_LEVEL]), top / 10.0);
    check(db10(64) >= top - 120 && db10(64) <= top - 60, "half the travel: 6..12 dB under the top (GM's volume law)");
    check(trk[0].p[P_LEVEL] >= 102 && trk[0].p[P_LEVEL] <= 106, "half the travel: about LEVEL's power-on 104 (-4 dB)");
    check(db10(32) >= top - 300 && db10(95) >= top - 60, "a quarter: within 30 dB of the top, three quarters within 6 dB");
    fb_apply(FB_MIX, 2, 100);
    check(trk[0].p[P_REV] == 100, "fader 3 (REVERB, CC 91): as the CC, as before");
    {   /* heard: a held chord through the whole mix (MASTER at the top) */
        int32_t p0, p1, ph, pt;
        trk_note_on(&trk[0], 60, 100); trk_note_on(&trk[0], 64, 100); trk_note_on(&trk[0], 67, 100);
        p0 = peak(0); p1 = peak(8); ph = peak(64); pt = peak(127);
        printf("fader_level: the sound through the mix, the fader at 0, 8, 64, 127: peak %d, %d, %d, %d\n", p0, p1, ph, pt);
        check(p0 == 0 && p1 > 0 && ph > p1 && pt >= ph && ph * 4 >= pt,
              "through the mix: silent at the bottom, heard low down, half the travel within 12 dB of the top");
    }
    printf("fader_level: %s\n", fails ? "FAIL" : "ok");
    return fails != 0;
}
