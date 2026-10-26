# Clipper Pro: Clipper on the M-VAVE SMK-37

Clipper Pro is Clipper 1.1, the synthesizer firmware for the FM-1, on the M-VAVE SMK-37: the SMK-37 Pro (and the
Starrykey 37 Play), the SMK-37 Elite and the MKE-P37 are one keyboard, with one package each. The board layer
(keys, pads, faders, wheels, the screen, the flash map and the update path) is genkigenki's
[fm1-to-smk37](https://github.com/genkigenki/fm1-to-smk37). Everyone who made it possible:
[CREDITS.md](CREDITS.md).

The rule of the mapping: every FM-1 control keeps a home on the SMK-37, so Clipper's own manual
([QUICKSTART.md](QUICKSTART.md)) still applies. The SMK-37's extra controls (outer keys, pads, faders, wheels, pedal,
four spare buttons) play the synthesizer and never replace an FM-1 control.

## Knobs

| SMK-37 (printed) | Clipper Pro | FM-1 |
|---|---|---|
| K1 | master volume (a virtual pot) | MASTER |
| K2 | SELECT: pages, menu sections, tempo | SELECT |
| K3 | ALGORITHM: the kind of sound (BASS, KEYS, ORGN, PAD, LEAD, PLCK, STAB, FX, USER) | ALGORITHM |
| K4 | PRESETS: the sounds (the pads keep playing; KNOB BANK flashes: the sounds are on the pads there) | PRESETS |
| K5 to K8 | the screen's four dials (KNOB 1 to 4); on HOME the sound's main controls | KNOB 1 to 4 |

## Faders

Four faders, four banks. **FADER BANK** shows the bank screen; pressed again, `<` `>` or K2 step the banks; PATCH
closes it, or it closes by itself after 4 s. Moving a fader brings the screen up with the value it set. A fader takes over only once it moves, so nothing jumps after a sound or
bank change. A fader sets its value as the MIDI CC does from a controller (0 to 127 over the whole range), on the
sound being played; with GLO > MIDI 2 > SEND CC the move goes out as that CC too, for a DAW to record.

| Bank | Fader 1 | Fader 2 | Fader 3 | Fader 4 |
|---|---|---|---|---|
| **A MACROS** (the default) | BRITE (CC 16) | TIMBR (CC 17) | MOTN (CC 18) | SPACE (CC 19) |
| **B ENV** | ATTACK (CC 73) | DECAY (CC 75) | SUSTAIN | RELEASE (CC 72) |
| **C TONE** | CUTOFF, the engine's own (CC 74) | RESONANCE (CC 71) | LFO RATE (CC 76) | GLIDE (CC 5) |
| **D MIX** | CHORUS send (CC 93) | DELAY send (CC 94) | REVERB send (CC 91) | LEVEL (CC 7) |

On HYBRID, bank A is its own four macros M1 to M4 (their names on the screen), as CC 16 to 19 are. An engine without a
resonance shows `-` for fader 2 of bank C.

## Wheels and pedal

They play the sound as a MIDI keyboard's do, through the same performance controls:

| Control | Clipper Pro |
|---|---|
| PITCH wheel | the pitch bend; its range is GLO > MIDI > BEND (AUTO: the sound's own bend), RPN 0 too |
| MOD wheel | the mod wheel (CC 1): the MOD page's MW destination (vibrato, cutoff, ...) and the matrix's MW source |
| PEDAL jack | the sustain pedal (CC 64). A footswitch of either polarity, plugged in **before** power-on: the reading at power-on is the pedal up |

They go out over USB MIDI and the TRS MIDI OUT jack on Clipper's channel out, as the keys do. With GLO > MIDI > LOCAL
OFF they only go out (a DAW sends the track back). The wheels calibrate themselves: the first movements to the ends
of their travel set the range; the pitch wheel's rest is its centre.

## Keys and pads

| SMK-37 | Clipper Pro |
|---|---|
| the middle 27 keys (F3 to G5) | the FM-1's 27 keys, with velocity (the factory curves of your unit) |
| the 5 lowest (C3 to E3) and 5 highest (G#5 to C6) keys | more notes of the sound, with velocity (chromatic, OCT and TRANS apply) |
| 16 RGB pads, **NOTES** (the default) | the 16 white keys F3 to G5 (bottom row F3 to F4, top row G4 to G5), with velocity. They play as the keys do: SEL's QNT WHITE makes the white keys walk the scale from C4, chords, the arpeggiator and recording included. Lit in the colour of the sound's kind: bright while the note sounds, the root's pads half, the others dim, a silent pad dark |
| 16 RGB pads, **SOUNDS** | **KNOB BANK** opens it (lit; pressed again: closed). Only KNOB BANK does: K3 and K4 browse the sounds while the pads keep playing, and an open SOUNDS follows them to the sound's page. The pads and the screen show the sounds of one kind, 16 a page (the favourites, then BASS, KEYS, ORGN, PAD, LEAD, PLCK, STAB, FX, then your user presets), each kind in its colour, the current sound white. A tap loads the sound and plays a short preview; `<` `>` step the pages; PATCH closes it |
| pads, a layer held | as on the FM-1's keys: SEQ held, the 16 steps (read like text: top row 1 to 8); SEL held, a little piano (top row C4 to C5, bottom row the black keys): a pad sets the key; FX held, the 16 punch-in effects; GLO held, FILL (9), FILL NEXT BAR (10), TAP tempo (16); SAVE held, the song's sections (1 to 4 play, 5 to 8 store, 13 LOOP / SONG, 14 SONG REC, 16 the SONG page). Every tile that does something glows, the active one bright |

## Buttons

| SMK-37 (printed) | Clipper Pro | FM-1 |
|---|---|---|
| PATCH | HOME (tap: the sound; hold: MENU) | HOME |
| PARA | EDIT (the engine's pages; held: erase) | EDIT |
| FX | FX | FX |
| SCALE | SEL (the key, the scale, the chords) | SEL |
| CHORD | ENV (the envelope pages) | ENV |
| NOTE REPEAT | LFO (the LFO pages) | LFO |
| ARP | ARP (held: note repeat); its LED is lit while the arpeggiator runs | ARP |
| SEQ | SEQ (tapped: NOTES; held: the steps) | SEQ |
| GLOBE | GLO | GLO |
| BT | SAVE | SAVE |
| SEQ PLAY / SEQ REC | PLAY / REC | PLAY / REC |
| OCTAVE- / OCTAVE+ | OCT- / OCT+ | OCT- / OCT+ |
| KNOB BANK | the SOUNDS on the pads (lit while pinned) | none |
| FADER BANK | the fader banks (lit while the screen is up) | none |
| `<` `>` | the pages (as SELECT, never the tempo); SEQ held: the step pages; SOUNDS: its pages; FADER BANK: the banks | none |

Power-on: **OCTAVE-** held: CLIPPER PRO USB RESCUE (install again from the installer page). **OCTAVE- + OCTAVE+**
held: the hardware calibration (it teaches which button and knob does what). OCTAVE- + OCTAVE+ held 5 s while stopped:
the chip's mask-ROM bootloader (WL82 UBOOT1.00), for a full flash backup.

## Menu

Clipper's HOME menu (PATCH held), with on the SMK-37 a **PADS** section (the FM-1's LIGHTS): **PAD LIGHTS**, the pads' brightness (HIGH, MID, LOW;
kept in the LIGHTS setting), and **PAD THRESHOLD**, how firm a tap starts a note (LOW, MID, HIGH, MAX; kept in
the FM-1's KEYS setting: the SMK-37's keys have no lights). Both are shared with SLOOP Pro.
**PAD SCOPE** (OCT+) is the pads' trigger monitor, as a drum module's: the force of the pad hit last over 430 ms
(white), PAD THRESHOLD (yellow), the guard a new hit must clear (red), its notes (green: on, blue: off). Two notes of
one pad within 250 ms stop the picture: DOUBLE, how far apart, both velocities. OCT+ records again, OCT- goes back.
Take a photo of a double and send it with your report.

A pad hit flashes white for 80 ms, whatever it shows. The pads trigger as a drum module's: the threshold, the velocity at the peak of the hit, a 40 ms mask after each note
and a retrigger cancel that follows the pad's decay: one tap is one note, also a soft one whose finger stays on the
pad; rolls up to 16 hits a second.

## MIDI

Everything of Clipper's [docs/midi.md](docs/midi.md) holds. The SMK-37 adds the **TRS MIDI OUT** jack (3.5 mm, with an
adapter to DIN): the same stream as USB MIDI out (the keys, the pads, the wheels, the pedal, the arpeggiator and the
sequencer on the channel out, SEND CC), with or without a computer. The SMK-37 has no MIDI input jack (its pin is a
key-matrix row): MIDI in is over USB.

## What is kept, and where

Clipper Pro stores what Clipper stores (the settings, the working sound, the user presets, the FM6 bank) in the
SMK-37's store window, and shares it with SLOOP Pro, as Clipper and SLOOP share it on the FM-1 (the same records):
going from one to the other keeps your user presets and settings. Your own wavetables (WAVE's USR slots) have **two**
slots on the SMK-37 (USR1, USR2) instead of four: the other two are the SMK-37's factory data.

The factory calibration (USRTRIM), the Bluetooth pairing, the stock resources and key_mac are never written. Before
the first wavetable is stored, the stock firmware's mapping marker is released, so M-VAVE's updater takes the keyboard
back to its factory mapping (genkigenki's protection, as in SLOOP Pro).

## Install, build, the way back

- **Install:** [the installer page](https://isod89.github.io/clipper-pro/) in Chrome or Edge (or `INSTALL-CLIPPER-PRO.bat`,
  which builds and opens it from your computer). The keyboard must run M-VAVE's **V016** first.
- **Windows, if the page stops while the keyboard restarts into its update mode:** close Chrome completely, open the
  page again and press Install (it goes on where it stopped), or run `INSTALLER-USB-PRO.bat` (no browser).
- **Build:** `INSTALL-CLIPPER-PRO.bat` (WSL MinUI-Build and the JieLi toolchain, as Clipper's own build); elsewhere
  `BOARD=smk37 MODEL=pro|elite|mke ./build.sh` ([BUILDING.md](BUILDING.md)).
- **The way back:** back up with the editor (Backup tab), then M-VAVE's own updater (M-UPGRADE) with your model's
  official firmware.

## Safety

- Back up the whole flash once, before the first install: OCTAVE- + OCTAVE+ held 5 s (stopped), then
  [jl-uboot-tool](https://github.com/kagaimiq/jl-uboot-tool): `read 0 0x100000 smk37-dump.bin`, twice, compare.
- Never unplug during the write. If it stops: replug, press Install again; the update mode resumes.
- Unofficial firmware, no warranty. M-VAVE, SMK-37, MKE-P37 and Starrykey are trademarks of their owners; Clipper Pro
  is not affiliated with them.
