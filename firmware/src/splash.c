/* SPDX-License-Identifier: GPL-3.0-only */
/* SLOOP boot splash: the logo (tools/gen_logo.py -> sloop_logo.h: 16 colours, RLE runs of
 * (run - 1) << 4 | colour), drawn into the canvas in bands of at most 120 rows. */
#include "sloop_logo.h"
static void sloop_logo_draw(uint32_t y0)
{
    uint32_t r0;
    for (r0 = 0; r0 < SLOOP_SPLASH_H; r0 += 120u) {
        uint32_t rows = SLOOP_SPLASH_H - r0 > 120u ? 120u : SLOOP_SPLASH_H - r0;
        uint32_t lo = r0 * SLOOP_SPLASH_W, hi = (r0 + rows) * SLOOP_SPLASH_W, pos = 0, i;
        cv_begin(SLOOP_SPLASH_W, rows, C_BLACK);
        for (i = 0; i < sizeof SLOOP_SPLASH_RLE && pos < hi; i++) {
            uint32_t run = (SLOOP_SPLASH_RLE[i] >> 4) + 1u, c = SLOOP_SPLASH_RLE[i] & 15u, p;
            if (c && pos + run > lo)
                for (p = pos < lo ? lo : pos; p < pos + run && p < hi; p++)
                    cv_px[p - lo] = swap16(SLOOP_SPLASH_PAL[c]);
            pos += run;
        }
        cv_blit((240u - SLOOP_SPLASH_W) / 2u, y0 + r0);
    }
}

/* the boot screen: the logo, the version, where it comes from */
static void sloop_splash(void)
{
    lcd_fill(0, 0, 240, 240, C_BLACK);
    sloop_logo_draw(10);
    const char *v = FELUCCA_VERSION;
    char vb[24];
    if (v[0] == 'C' && v[1] == 'L' && v[7] == ' ')
        v += 8;                                         /* "CLIPPER 0.1" -> "0.1" under the wordmark */
#if BOARD_SMK37
    if (v[0] == 'P' && v[1] == 'R' && v[2] == 'O' && v[3] == ' ')
        v += 4;                                         /* "CLIPPER PRO 1.2" -> "1.2": PRO is the badge */
    cv_begin(56, 40, C_BLACK);                          /* the PRO badge, top right of the sail: the orange sail's colour */
    cv_rect(0, 0, 56, 2, RGB(255, 98, 26)), cv_rect(0, 38, 56, 2, RGB(255, 98, 26));
    cv_rect(0, 0, 2, 40, RGB(255, 98, 26)), cv_rect(54, 0, 2, 40, RGB(255, 98, 26));
    cv_text(4, 4, &FONT_L, "PRO", RGB(255, 98, 26));
    cv_blit(180, 14);
#endif
    {   /* (1.0) "1.0 EXP" -> "1.0 EXPERIMENTAL": the boot screen says it whole */
        uint32_t n = 0;
        while (v[n] && n < 12u)
            vb[n] = v[n], n++;
        vb[n] = 0;
        if (n >= 4u && vb[n - 4] == ' ' && vb[n - 3] == 'E' && vb[n - 2] == 'X' && vb[n - 1] == 'P' && !v[n])
            str_cpy(vb + n, "ERIMENTAL", 10);
        v = vb;
    }
    cv_begin(240, 36, C_BLACK);
    cv_text(120 - text_w(&FONT_S, v) / 2, 0, &FONT_S, v, RGB(196, 196, 204));
    cv_text(120 - text_w(&FONT_S, "based on felucca") / 2, 18, &FONT_S, "based on felucca", RGB(96, 96, 104));
    cv_blit(0, 202);
}
