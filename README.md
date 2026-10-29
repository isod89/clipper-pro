<p align="center"><img src="assets/logo/clipper-logo.png" alt="Clipper" width="420"></p>

<p align="center"><b>Clipper Pro — the Clipper synthesizer on the M-VAVE SMK-37.</b><br>
Free and open source (GPL-3.0). Clipper by isod89, grown out of <a href="https://github.com/isod89/sloop-fm1">SLOOP</a> and based on
<a href="https://github.com/hugelton/Felucca">Felucca</a> by Leo Kuroshita / Hügelton Instruments; the SMK-37 board layer by
<a href="https://github.com/genkigenki/fm1-to-smk37">genkigenki</a>. Everyone who made it possible: <a href="CREDITS.md">CREDITS.md</a>.</p>

<p align="center">
<b><a href="https://isod89.github.io/clipper-pro/">Install it from the browser</a></b> ·
<a href="CLIPPER-PRO.md"><b>Clipper Pro: controls, install</b></a> ·
<a href="DEMARRAGE-RAPIDE-PRO-FR.md">Démarrage rapide (français)</a> ·
<a href="TEST-CLIPPER-PRO.md">Essais sur le SMK-37</a> ·
<a href="QUICKSTART.md">Clipper's quick start</a> ·
<a href="BUILDING.md">Building</a> ·
<a href="../../releases">Releases</a> ·
<a href="../../issues">Report a bug</a>
</p>

---

**Clipper is a synthesizer**: 23 engines and 407 sounds, HYBRID (three oscillators, two filters, a modulation matrix),
FMX, FM6, WAVE, VECTOR, JUNO, ACID, MACRO and more, four macros on every sound, effects, an arpeggiator, a step
sequencer, microtuning, MPE, USB audio and MIDI. **Clipper Pro** is that firmware on the M-VAVE SMK-37, with
what the bigger keyboard brings, made for playing a synthesizer:

| | |
|---|---|
| **37 velocity keys** | the 27 middle keys as Clipper's, velocity from your unit's factory curves; the outer 10 play more notes |
| **16 RGB pads** | **NOTES**: the sound on the pads, in its kind's colour (QNT WHITE: the scale); **SOUNDS** (KNOB BANK): the sounds of a kind on the pads, a tap loads one; the steps and tiles when a layer is held |
| **4 faders, 4 banks** | A **MACROS** (BRITE, TIMBR, MOTN, SPACE; HYBRID's M1..M4), B **ENV** (ADSR), C **TONE** (cutoff, resonance, LFO rate, glide), D **MIX** (chorus, delay, reverb, level) |
| **8 knobs** | MASTER, SELECT, ALGORITHM (the kind of sound), PRESETS, and the screen's four dials, as the FM-1's |
| **Wheels and pedal** | the pitch bend (Clipper's BEND range), the mod wheel (the sound's MW destination, the matrix), a sustain pedal |
| **MIDI** | USB MIDI, and the **TRS MIDI OUT** jack: the keys, pads, wheels, pedal, arpeggiator and sequencer go out on both |
| **Models** | SMK-37 Pro (and Starrykey 37 Play), SMK-37 Elite, MKE-P37: one package each, the installer picks the right one |

> **Status:** Clipper Pro 1.3, a bug-fix release (no more distortion at the end of notes, 3.5 mm and USB; UNISON no longer stacks its voices in phase; PHYS strings in tune; NZ ARCADE, SAMPLE and GRAIN follow the top keys; a firm press on a layer button opens its pages; on REC READY a knob changes only its own value; the faders follow a real volume curve; the sustain pedal held as long as you like; no hanging note on TRS MIDI OUT; the lights no longer freeze after 25 days; the installer no longer freezes Chrome on Windows; VECTOR's filter MODEL changes without a click; SLOOP projects keep Clipper's values; backups keep your wavetables; the way back accepts M-VAVE's V16), built on Clipper 1.3, tested on the SMK-37 as 1.2. Install at your own risk (read the
> [safety notes](CLIPPER-PRO.md#safety)); your unit's calibration is never written, and M-VAVE's own updater takes you
> back at any time.

## Install

1. Back up the keyboard's flash once (see [Safety](CLIPPER-PRO.md#safety)). The keyboard must run M-VAVE's **V016**:
   update an older one with M-VAVE's updater (M-UPGRADE) first.
2. Open **[isod89.github.io/clipper-pro](https://isod89.github.io/clipper-pro/)** in Chrome or Edge (or build it
   yourself on Windows: `INSTALL-CLIPPER-PRO.bat` builds the three packages in WSL and opens the same installer).
3. Plug the SMK-37 straight into the computer (a USB data cable), close other MIDI apps, tick the box, press
   **Install Clipper Pro**. The page reads the model and writes its package, then checks the keyboard after the restart.

On Windows, if the page stops while the keyboard restarts into its update mode: close Chrome completely, open the page
again and press Install (it goes on where it stopped), or run `INSTALLER-USB-PRO.bat` (no browser). The command line
does the same: `python tools/fm1_install.py build/release/`.

## Editor

Clipper's web editor works with Clipper Pro: every parameter live, the HYBRID and FMX patches, the MOD SEQ, the tuning,
DX7 banks, wavetables, the sequencer, backups. Open it from the installer page or with `OPEN-EDITOR-PRO.bat`.

## For FM-1 owners

This repository still builds Clipper for the FM-1 (`./build.sh`, unchanged from Clipper 1.1: the FM-1 build compiles to
the same code). Clipper for the FM-1 lives at [isod89/clipper-fm1](https://github.com/isod89/clipper-fm1); its README is
kept here as [README-CLIPPER.md](README-CLIPPER.md).

## Credits

[CREDITS.md](CREDITS.md) names each of them and what they did.

- **Felucca:** Leo Kuroshita ([@kurogedelic](https://github.com/kurogedelic)), Hügelton Instruments, the open FM-1
  firmware Clipper is built on.
- **SLOOP and Clipper:** isod89.
- **The SMK-37 port:** M. Geisler ([@genkigenki](https://github.com/genkigenki)):
  [fm1-to-smk37](https://github.com/genkigenki/fm1-to-smk37), the board layer and its patch series, the pad picker and
  the FADER BANK screen, the installer library that works on Windows.
- **[@ChanceTheMaker](https://github.com/ChanceTheMaker)**, [sloop-smk](https://github.com/ChanceTheMaker/sloop-smk):
  the ideas behind the pedal, the TRS note-off flush and the installer's disposable MIDI session.
- **The second core:** [@andrerezemini](https://github.com/andrerezemini) (sloop-lab),
  [Charles Vestal](https://github.com/charlesvestal) (X0X), [@keremimo](https://github.com/keremimo) (Melodee).
- **Also:** [@juanjiant-bit](https://github.com/juanjiant-bit) (JIANT FM), [AL-255](https://github.com/AL-255/FM-1-RE)
  (FM-1-RE), [kagaimiq](https://github.com/kagaimiq) (JieLi tools), and the Felucca and SLOOP communities.

M-VAVE, SMK-37, MKE-P37, Starrykey and FM-1 are trademarks of their respective owners; Clipper Pro is not affiliated
with them. Licence: GPL-3.0 ([LICENSE](LICENSE), [LICENSING.md](LICENSING.md)).
