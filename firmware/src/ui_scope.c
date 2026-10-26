/* SPDX-License-Identifier: GPL-3.0-only
 * SLOOP Pro, Clipper Pro: Copyright (C) 2026 isod89 */
/* PAD SCOPE (HOME menu > PADS), as a drum module's trigger monitor: what the pad hit last measures (smk_input.h
 * smk_scope), 430 ms across the screen, 1.8 ms a column. White: the force above the baseline; yellow: PAD THRESHOLD;
 * red: the guard a new hit has to clear (retrigger cancel); green: a note-on, blue: its note-off. Two notes of one pad
 * within 250 ms are taken as a double: the picture stops there (DOUBLE, how far apart, both velocities) until OCT+.
 * OCT- goes back to the menu. Meant for a photo when a pad plays twice. */
static void scope_open(void)
{
    smk_scope.pad = 0xFFu;
    smk_scope_rearm();
    smk_scope.on = 1;
}
static void scope_close(void) { smk_scope.on = 0; }

static void scope_draw(void)
{
    static uint32_t shown, at;
    uint32_t n = smk_scope.n, cnt = n < SMK_SCOPE_N ? n : SMK_SCOPE_N, i, x, sig, maxv = 96u;
    uint32_t base = smk_scope.base, on = SMK_PAD_ON, frozen = smk_scope.frozen, pass;
    const int32_t Y0 = 44, H = 160;
    char t[40];
    sig = n * 2654435761u + frozen * 7919u + smk_scope.pad * 104729u + smk_pad_level * 31u;
    if (!ui.force && (sig == shown || (!frozen && ui.frame - at < 3u)))
        return;                                    /* (live: a picture every third frame) */
    shown = sig;
    at = ui.frame;
    for (i = 0; i < cnt; i++) {                    /* the scale: the highest of the force and the guard */
        uint32_t k = (n - cnt + i) % SMK_SCOPE_N, v = smk_scope.raw[k] > base ? smk_scope.raw[k] - base : 0u;
        uint32_t g = smk_scope.rt[k] * 4u + on / 2u;
        if (v > maxv)
            maxv = v;
        if (g > maxv)
            maxv = g;
    }
    maxv += maxv / 10u;
#define SCOPE_Y(v) (Y0 + H - (int32_t)((v) > maxv ? (uint32_t)H : (v) * (uint32_t)H / maxv))
    for (pass = 0; pass < 2u; pass++) {            /* two bands of 120 rows */
        cv_begin(240, 120, C_BLACK);
        cv_oy = pass ? -120 : 0;
        cv_text(4, 2, &FONT_S, "PAD SCOPE", C_WHITE);
        if (smk_scope.pad < SMK_NPAD) {
            str_cpy(t, "PAD ", sizeof t);
            fmt_int(t + str_len(t), (int32_t)smk_scope.pad + 1);
            cv_text(236 - text_w(&FONT_S, t), 2, &FONT_S, t, C_HI);
        }
        if (!cnt) {
            str_cpy(t, "TAP A PAD", sizeof t);
        } else if (frozen) {                       /* DOUBLE 63 MS  VEL 54 > 38 */
            str_cpy(t, "DOUBLE ", sizeof t);
            fmt_int(t + str_len(t), (int32_t)(smk_scope.dt * 18u / 10u));
            str_cpy(t + str_len(t), " MS  VEL ", sizeof t - str_len(t));
            fmt_int(t + str_len(t), smk_scope.vel[0]);
            str_cpy(t + str_len(t), " > ", sizeof t - str_len(t));
            fmt_int(t + str_len(t), smk_scope.vel[1]);
        } else {
            str_cpy(t, "LIVE  NOTES ", sizeof t);
            fmt_int(t + str_len(t), smk_scope.nnotes);
            str_cpy(t + str_len(t), "  VEL ", sizeof t - str_len(t));
            fmt_int(t + str_len(t), smk_scope.vel[0]);
        }
        cv_text(4, 20, &FONT_S, t, frozen ? RGB(255, 80, 80) : C_AMB);
        cv_rect(0, Y0 + H, 240, 1, C_LINE);
        for (x = 0; x < 240u; x += 6u)             /* PAD THRESHOLD, dashed */
            cv_rect((int32_t)x, SCOPE_Y(on), 3, 1, RGB(255, 210, 0));
        for (x = 0; x < cnt; x++) {
            uint32_t k = (n - cnt + x) % SMK_SCOPE_N, kp = (n - cnt + (x ? x - 1u : 0u)) % SMK_SCOPE_N;
            uint32_t v = smk_scope.raw[k] > base ? smk_scope.raw[k] - base : 0u;
            uint32_t vp = smk_scope.raw[kp] > base ? smk_scope.raw[kp] - base : 0u;
            uint32_t g = smk_scope.rt[k] * 4u + on / 2u;
            if (smk_scope.ev[k] & SC_ON)
                cv_rect((int32_t)x, Y0, 1, H, RGB(0, 200, 60));
            if (smk_scope.ev[k] & SC_OFF)
                cv_rect((int32_t)x, Y0 + H - 24, 1, 24, RGB(60, 120, 255));
            cv_pset((int32_t)x, SCOPE_Y(g > on ? g : on), RGB(255, 60, 60));
            cv_line(x ? (int32_t)x - 1 : 0, SCOPE_Y(vp), (int32_t)x, SCOPE_Y(v), C_WHITE);
        }
        str_cpy(t, "THR ", sizeof t);
        fmt_int(t + str_len(t), (int32_t)on);
        str_cpy(t + str_len(t), "  MAX ", sizeof t - str_len(t));
        fmt_int(t + str_len(t), (int32_t)maxv);
        cv_text(4, 208, &FONT_S, t, C_GRAY);
        cv_text(236 - text_w(&FONT_S, "OCT+ AGAIN"), 208, &FONT_S, "OCT+ AGAIN", C_GRAY);
        cv_blit(0, pass * 120u);
    }
    cv_oy = 0;
#undef SCOPE_Y
}
