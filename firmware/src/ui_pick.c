/* SPDX-License-Identifier: GPL-3.0-only
 * SMK-37 Pro pad picker: Copyright (C) 2026 M. Geisler (genkigenki/makkuro)
 * Clipper Pro: Copyright (C) 2026 isod89 */
/* Clipper Pro: the SOUNDS on the pads. The 16 pads (and the screen, 8 x 2) are the sounds of one kind, in the PRESETS
 * list's order (ui.c: the favourites, then BASS, KEYS, ORGN, PAD, LEAD, PLCK, STAB, FX, then the user presets), 16 a
 * page. A pad loads its sound (as PRESETS does) and plays a short preview; < > step the pages. KNOB BANK opens and
 * closes it (its LED lit while it is up), PATCH closes it. Pad hits never reach the notes while it is up (smk_input.h
 * smk_pad_pick); it is never up unless asked for: a PRESETS (K4) or ALGORITHM (K3) turn browses the sounds and the pads
 * keep playing, KNOB BANK flashes on each detent (the sounds are there), and an open SOUNDS follows the sound to its
 * page. After genkigenki's pad picker in SLOOP's SMK-37 layer. */
enum { PK_USER = 8, PK_FAV = 9 };
static const char *const PICK_KIND[10] = {"BASS", "KEYS", "ORGN", "PAD", "LEAD", "PLCK", "STAB", "FX", "USER", "FAV"};
static const uint8_t PICK_RGB[10][3] = {           /* a colour a kind (pads and screen) */
    {255, 40, 40},  {255, 140, 0},  {255, 220, 0}, {40, 220, 80},  {0, 200, 255},
    {60, 80, 255},  {170, 60, 255}, {255, 60, 170}, {220, 210, 170}, {255, 190, 60}};
static uint8_t pick_pin;
static uint16_t pick_page;
static uint32_t pick_frame;                        /* the frame drawn last */
static uint16_t pick_seen = 0xFFFFu;               /* the sound the page was last set for */
static uint32_t pick_k4_until;                     /* KNOB BANK lit until (ms): a PRESETS detent */
typedef struct { uint16_t first, len, sub, nsub, kind; } pick_pg_t;

static uint32_t pick_total(void)
{
    uint32_t total;
    preset_pos(&total);
    return total;
}
static uint32_t pick_cur(void)
{
    uint32_t total;
    return preset_pos(&total);
}
static uint32_t pick_kind_of(uint32_t n)
{
    uint32_t k = kind_at(n);
    return k == 0xFEu ? PK_FAV : k == 0xFFu || k >= PK_USER ? PK_USER : k;
}
static void pick_name(uint32_t n, char *b)        /* 13 bytes */
{
    uint32_t k, e = preset_at(n, &k);
    if (e == NENGINES)
        up_name(k, b);
    else
        str_cpy(b, ENGINES[e % NENGINES]->presets[k].name, 13);
}
/* a cell's two short lines: the first two words' first 3 letters ("POLY PAD": POL / PAD), one word: 3 + 3 */
static void pick_abbr(const char *s, char *l1, char *l2)
{
    const char *w2 = s, *r;
    uint32_t j;
    while (*w2 && *w2 != ' ')
        w2++;
    while (*w2 == ' ')
        w2++;
    for (j = 0; s[j] && s[j] != ' ' && j < 3u; j++)
        l1[j] = s[j];
    l1[j] = 0;
    r = *w2 ? w2 : s + j;
    for (j = 0; r[j] && r[j] != ' ' && j < 3u; j++)
        l2[j] = r[j];
    l2[j] = 0;
}

/* the pages: each kind's run of entries, 16 a page. Fills *pg for page want, *of = the page of entry n;
 * returns the number of pages */
static uint32_t pick_walk(uint32_t want, pick_pg_t *pg, uint32_t n_of, uint32_t *of)
{
    uint32_t total = pick_total(), n = 0, p = 0;
    while (n < total) {
        uint32_t k = pick_kind_of(n), end = n, nsub, i;
        while (end < total && pick_kind_of(end) == k)
            end++;
        nsub = (end - n + 15u) / 16u;
        for (i = 0; i < nsub; i++, p++) {
            uint32_t f = n + i * 16u, l = end - f > 16u ? 16u : end - f;
            if (p == want && pg) {
                pg->first = (uint16_t)f;
                pg->len = (uint16_t)l;
                pg->sub = (uint16_t)i;
                pg->nsub = (uint16_t)nsub;
                pg->kind = (uint16_t)k;
            }
            if (of && n_of >= f && n_of < f + l)
                *of = p;
        }
        n = end;
    }
    return p;
}
static void pick_home(void)                        /* to the page of the current sound */
{
    uint32_t p = 0;
    pick_walk(0xFFFFu, 0, pick_cur(), &p);
    pick_page = (uint16_t)p;
}
static uint32_t pick_info(pick_pg_t *pg)          /* the page shown */
{
    uint32_t np;
    pg->first = pg->len = pg->sub = pg->nsub = pg->kind = 0;
    np = pick_walk(pick_page, pg, 0xFFFFu, 0);
    if (np && pick_page >= np) {
        pick_page = 0;
        pick_walk(0, pg, 0xFFFFu, 0);
    }
    return np;
}

static int pick_shown(void)
{
    return pick_pin && !ui.menu && ui.layer == LY_PLAY && !ui.hold_kind;
}
static void pick_close(void)
{
    pick_pin = 0;
}
static void pick_pin_toggle(void)                  /* KNOB BANK */
{
    pick_pin = !pick_pin && !ui.menu;
    if (pick_pin) {
        pick_home();
        pick_seen = (uint16_t)pick_cur();
    }
}
static void pick_hint(void)                        /* a PRESETS / ALGORITHM detent: KNOB BANK flashes */
{
    pick_k4_until = fm1_ms + 150u;
}
static void pick_follow(void)                      /* (once a frame) the sound changed (K3, K4, MIDI, the editor): an
                                                    * open SOUNDS goes to its page */
{
    static uint32_t fr;
    uint32_t cur;
    if (!pick_pin || fr == ui.frame)
        return;
    fr = ui.frame;
    cur = pick_cur();
    if (cur == pick_seen)
        return;
    pick_seen = (uint16_t)cur;
    pick_home();
    ui.force = 1;
}
static void pick_turn(int32_t d)                   /* < > */
{
    pick_pg_t pg;
    uint32_t np = pick_info(&pg);
    if (np)
        pick_page = (uint16_t)((pick_page + np + (uint32_t)(d > 0 ? 1 : (int32_t)np - 1)) % np);
    ui.force = 1;
}
static void pick_select(uint32_t w)                /* cell w (0..15) of the page: hardware pad w, reading order */
{
    pick_pg_t pg;
    uint32_t n;
    if (!pick_info(&pg) || w >= pg.len)
        return;
    n = pg.first + w;
    preset_go(n);
    pick_aud_req = (uint8_t)((pg.kind == 0u ? 48u : 60u) + 1u);   /* the preview: basses an octave down */
    ui.force = 1;
}

static uint16_t pick_col(uint32_t kind, uint32_t sh)   /* the kind's colour, >> sh */
{
    const uint8_t *c = PICK_RGB[kind % 10u];
    return RGB(c[0] >> sh, c[1] >> sh, c[2] >> sh);
}
/* the colour of the current sound's own kind (the NOTES pads, the tiles): the same sound, the same colour, however it
 * was loaded (never FAV's gold for a favourite: its kind's); a user preset USER's; a sound no list entry names (an
 * engine switched on the EDIT page, a program change to an unlisted preset) keeps the colour it had */
static const uint8_t *pick_rgb_cur(void)
{
    static uint8_t kind = PK_USER;
    uint32_t i;
    track_t *t = TSEL;
    if (!bank_ready)
        bank_resolve();
    if (user_of(t) < UP_SLOTS) {
        kind = PK_USER;
    } else {
        for (i = 0; i < NBANK; i++)
            if (BANK[i].e == t->eng_req && bank_pi[i] == t->preset) {
                kind = (uint8_t)(BANK[i].kind < PK_USER ? BANK[i].kind : PK_USER);
                break;
            }
    }
    return PICK_RGB[kind % 10u];
}
static void pick_leds(void)                         /* the current sound white, the others their kind's colour dim */
{
    pick_pg_t pg;
    uint32_t w, cur = pick_cur();
    const uint8_t *c;
    if (!pick_info(&pg))
        pg.len = 0;
    c = PICK_RGB[pg.kind % 10u];
    for (w = 0; w < 16u; w++) {                    /* reading order: pads 0..7 the top row */
        if (w >= pg.len)
            padled_set(w, 0, 0, 0);
        else if (pg.first + w == cur)
            padled_set(w, 150, 150, 150);
        else
            padled_set(w, (uint8_t)(c[0] >> 2), (uint8_t)(c[1] >> 2), (uint8_t)(c[2] >> 2));
    }
}

/* the screen mirrors the pads: the current sound's name, 8 x 2 cells, < BASS 1/2 > */
static void pick_draw(void)
{
    static uint32_t shown;
    pick_pg_t pg;
    uint32_t np = pick_info(&pg), cur = pick_cur(), half, w, sig;
    uint16_t kc = pick_col(pg.kind, 0);
    char nm[13], l1[4], l2[4], foot[16];
    if (pick_frame + 1u != ui.frame) {             /* another screen was up: all of it */
        lcd_fill(0, 0, 240, 240, C_BLACK);
        ui.force = 1;
    }
    pick_frame = ui.frame;
    sig = (np ? pg.first * 977u + pg.kind * 31u : 0u) + cur * 7919u + pick_total() * 3u;
    if (!ui.force && sig == shown)
        return;
    shown = sig;
    pick_name(cur, nm);
    str_cpy(foot, PICK_KIND[pg.kind % 10u], sizeof foot);
    if (pg.nsub > 1u) {
        uint32_t n = str_len(foot);
        foot[n++] = ' ';
        fmt_int(foot + n, pg.sub + 1);
        n = str_len(foot);
        foot[n++] = '/';
        fmt_int(foot + n, pg.nsub);
    }
    for (half = 0; half < 2u; half++) {
        int32_t y0 = half ? 2 : 38;
        cv_begin(240, 120, C_BLACK);
        if (!half)
            cv_text(4, 2, &FONT_L, nm, C_WHITE);
        for (w = half * 8u; np && w < half * 8u + 8u && w < pg.len; w++) {
            int32_t x = (int32_t)(w & 7u) * 30, sel = pg.first + w == cur;
            pick_name(pg.first + w, nm);
            pick_abbr(nm, l1, l2);
            cv_rect(x + 1, y0, 28, 78, sel ? C_WHITE : kc);   /* filled like a button: the kind's colour, the current one white */
            cv_text(x + 15 - text_w(&FONT_S, l1) / 2, y0 + 20, &FONT_S, l1, C_BLACK);
            cv_text(x + 15 - text_w(&FONT_S, l2) / 2, y0 + 42, &FONT_S, l2, C_BLACK);
        }
        if (half) {
            cv_text(4, 86, &FONT_L, "<", C_WHITE);
            cv_text(236 - text_w(&FONT_L, ">"), 86, &FONT_L, ">", C_WHITE);
            cv_text((240 - text_w(&FONT_L, foot)) / 2, 86, &FONT_L, foot, kc);
        }
        cv_blit(0, half * 120u);
    }
}
