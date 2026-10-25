/* SPDX-License-Identifier: GPL-3.0-only
 * SMK-37 Pro FADER BANK screen: Copyright (C) 2026 M. Geisler (genkigenki/makkuro)
 * Clipper Pro: Copyright (C) 2026 isod89 */
/* Clipper Pro: the four faders play the sound, in four banks (FADER BANK steps them, as < > and SELECT do while the
 * screen is up; PATCH closes it, or it closes by itself):
 *   A MACROS  BRITE, TIMBR, MOTN, SPACE (HYBRID: its M1..M4), the CCs 16..19
 *   B ENV     ATTACK, DECAY, SUSTAIN, RELEASE (the CCs 73, 75, -, 72)
 *   C TONE    CUTOFF, RESONANCE (the engine's own, CC 74 / 71), LFO RATE (CC 76), GLIDE (CC 5)
 *   D MIX     CHORUS, DELAY, REVERB sends (CC 93, 94, 91), LEVEL (CC 7)
 * A fader sets its value as the CC from a MIDI controller does (seq.c midi_cc: 0..127 over the range), on the sound being
 * played; with GLO > MIDI 2 > SEND CC the move goes out as that CC too (seq.c cc_out_knob). A fader takes over only
 * once it moves (also after a bank change): nothing jumps. After genkigenki's FADER BANK screen (SLOOP's SMK-37 layer) */
#define FB_N 4u
static uint8_t smk_fbank, smk_fbank_new;           /* the bank (main.c smk_faders) */
static const char *const FB_NAME[FB_N] = {"MACRO", "ENV", "TONE", "MIX"};
static const char *const FB_LAB[FB_N][4] = {
    {"BRITE", "TIMBR", "MOTN", "SPACE"},
    {"ATTACK", "DECAY", "SUSTAIN", "RELEASE"},
    {"CUTOFF", "RESONANCE", "LFO RATE", "GLIDE"},
    {"CHORUS", "DELAY", "REVERB", "LEVEL"}};
#define FB_SUS 0xFFu                               /* (no CC: SUSTAIN set directly) */
static const uint8_t FB_CC[FB_N][4] = {{16, 17, 18, 19}, {73, 75, FB_SUS, 72}, {74, 71, 76, 5}, {93, 94, 91, 7}};
static const uint8_t FB_RGB[4][3] = {{255, 190, 60}, {40, 220, 80}, {0, 200, 255}, {170, 60, 255}};
#define FB_SHOW_MS 4000u                           /* up this long after the last change */
static uint8_t fb_open;
static uint32_t fb_t, fb_frame;                    /* last action (ms); the frame drawn last */

static int fb_shown(void)
{
    return fb_open && !ui.menu && ui.layer == LY_PLAY && !ui.hold_kind;
}
static void fb_set(uint32_t b)                     /* bank b, the screen up */
{
    smk_fbank = (uint8_t)(b % FB_N);
    smk_fbank_new = 1;
    fb_open = 1;
    fb_t = fm1_ms;
    ui.force = 1;
}
static void fb_close(void) { fb_open = 0; }
static void fb_expire(void)                        /* up FB_SHOW_MS after the last change */
{
    if (fb_open && fm1_ms - fb_t > FB_SHOW_MS)
        fb_open = 0, ui.force = 1;
}
static int fb_left(void)                           /* drawn last frame, not now: the page needs a redraw */
{
    int l = fb_frame && fb_frame + 1u == ui.frame;
    if (l)
        fb_frame = 0;
    return l;
}

/* what fader i sets in bank b on the sound played (track 1): its range, *vp its value; 0: nothing (an engine without a
 * resonance) */
static const param_desc_t *fb_param(uint32_t b, uint32_t i, int16_t **vp)
{
    track_t *t = &trk[0];
    uint32_t cc = FB_CC[b % FB_N][i & 3u];
    if (cc == FB_SUS) {
        *vp = &t->p[P_SUS];
        return &TP[P_SUS];
    }
    if (cc >= 16u && cc <= 19u) {                  /* the macros (seq.c part_ctl_cc) */
        if (t->eng_req == ENGI_HYBRID) {
            *vp = &t->p[P_E4 + cc - 16u];
            return &ENGINES[ENGI_HYBRID]->edit[4u + cc - 16u];
        }
        *vp = &t->p[P_MAC1 + cc - 16u];
        return &TP[P_MAC1 + cc - 16u];
    }
    return cc_target(t, cc, vp);
}
/* fader i of bank b moved to v (0..127): as the CC (seq.c midi_cc), then SEND CC (cc_out_knob) */
static void fb_apply(uint32_t b, uint32_t i, uint32_t v)
{
    track_t *t = &trk[0];
    uint32_t cc = FB_CC[b % FB_N][i & 3u];
    int16_t *vp, old;
    const param_desc_t *d = fb_param(b, i, &vp);
    if (!d || d->max <= d->min)
        return;
    old = *vp;
    if (cc == FB_SUS)
        *vp = (int16_t)(d->min + ((int32_t)v * (d->max - d->min) + 63) / 127);
    else
        midi_cc(t, cc, v);
    if (*vp != old) {
        cc_out_knob(vp, old);                      /* (GLO > MIDI 2 > SEND CC) */
        ui.force = 1;
    }
}

static void fb_draw(void)
{
    static uint32_t shown;
    uint32_t b = smk_fbank % FB_N, i, half, sig = b * 7919u + trk[0].eng_req * 104729u;
    int16_t *vp;
    const uint8_t *rgb = FB_RGB[b];
    uint16_t col = RGB(rgb[0], rgb[1], rgb[2]);
    if (fb_frame + 1u != ui.frame) {               /* another screen was up: all of it */
        lcd_fill(0, 0, 240, 240, C_BLACK);
        ui.force = 1;
    }
    fb_frame = ui.frame;
    for (i = 0; i < 4u; i++)
        if (fb_param(b, i, &vp))
            sig = sig * 31u + (uint32_t)(*vp + 200);
    if (!ui.force && sig == shown)
        return;
    shown = sig;
    for (half = 0; half < 2u; half++) {
        cv_begin(240, 120, C_BLACK);
        if (!half) {
            cv_text(4, 2, &FONT_L, "FADER BANK", C_WHITE);
            for (i = 0; i < FB_N; i++) {           /* the banks (four, 60 px each) */
                int32_t x = (int32_t)i * 60 + 1, on = i == b;
                char l[2] = {(char)('A' + i), 0};
                cv_rect(x, 42, 58, 74, on ? C_WHITE : TE_G2);
                if (!on)
                    cv_rect(x + 1, 43, 56, 72, TE_G1);
                cv_text(x + 29 - text_w(&FONT_L, l) / 2, 48, &FONT_L, l, on ? C_BLACK : TE_G3);
                cv_text(x + 29 - text_w(&FONT_S, FB_NAME[i]) / 2, 90, &FONT_S, FB_NAME[i], on ? C_BLACK : TE_G3);
            }
        } else {
            for (i = 0; i < 4u; i++) {             /* the faders: F1 BRITE ... value, a bar */
                int32_t y = 4 + (int32_t)i * 24, w;
                char val[12], v2[20], f[3] = {'F', (char)('1' + i), 0};
                const char *unit = "", *lab = FB_LAB[b][i];
                const param_desc_t *d = fb_param(b, i, &vp);
                if (b == 0u && trk[0].eng_req == ENGI_HYBRID && d && d->label)
                    lab = d->label;                /* (HYBRID: its own macros' names) */
                cv_text(4, y, &FONT_S, f, col);
                cv_text(32, y, &FONT_S, lab, col);
                if (!d || d->max <= d->min) {
                    cv_text(236 - text_w(&FONT_S, "-"), y, &FONT_S, "-", TE_G3);
                    continue;
                }
                param_format(d, *vp, val, &unit);
                str_cpy(v2, val, sizeof v2);
                str_cpy(v2 + str_len(v2), unit, sizeof v2 - str_len(v2));
                cv_text(236 - text_w(&FONT_S, v2), y, &FONT_S, v2, C_WHITE);
                w = RATIO(d, *vp) * 204 / 1000;
                cv_rect(32, y + 17, 204, 3, TE_G1);
                cv_rect(32, y + 17, w < 1 ? 1 : w, 3, col);
            }
            cv_text((240 - text_w(&FONT_S, "< >  K2  FADER BANK")) / 2, 100, &FONT_S, "< >  K2  FADER BANK", TE_G3);
        }
        cv_blit(0, half * 120u);
    }
}
