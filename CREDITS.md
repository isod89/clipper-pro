# Clipper Pro: credits

Clipper Pro is Clipper on the M-VAVE SMK-37. It exists because many people shared their code, their research and
their time in the open. This page names everyone whose work is in it, what they did, and where to find it. Every part
is GPL-3.0-only unless [LICENSING.md](LICENSING.md) says otherwise.

## Clipper Pro

- **isod89 (Sodwave, 3dSam)**, [isod89/clipper-pro](https://github.com/isod89/clipper-pro): Clipper, and its port to
  the SMK-37. The port includes:
  - the controls made for a synthesizer: the fader banks (MACROS, ENV, TONE, MIX), the wheels and the pedal as a
    MIDI keyboard's, the NOTES and SOUNDS pads;
  - the installer page and its tests.

## The foundations

- **Leo Kuroshita (@kurogedelic), Hügelton Instruments**, [Felucca](https://github.com/hugelton/Felucca): the open
  FM-1 firmware Clipper is built on. It provides the platform, the engines, the sequencer, the web editor, and the
  M-UPGRADE-compatible installer and update loader. Most of the code carries his copyright line.
- **SLOOP** by isod89, [isod89/sloop-fm1](https://github.com/isod89/sloop-fm1): everything Clipper starts from.
  SLOOP's contributors are credited in its README.

## The SMK-37 port

- **M. Geisler (genkigenki / makkuro)**, [fm1-to-smk37](https://github.com/genkigenki/fm1-to-smk37),
  [Snoop37](https://github.com/genkigenki/Snoop37) and [genkigenki.github.io/smk37](https://genkigenki.github.io/smk37/).
  Clipper Pro's history contains his commits. His work in Clipper Pro:
  - **The board layer:** his SLOOP 2.5 patch series (the SMK-37 target, keys and velocity, pads and their LEDs, the
    screen, faders, wheels, encoders, the flash map and its guards, the update path, the store owner tag), and the
    later layer for SLOOP 2.7 (pad rows, the LED table, the pitch wheel's centre, the Elite and MKE-P37 builds).
  - **Safety:** refusing another board's package before anything is erased; releasing the stock mapping's marker,
    which keeps the way back to M-VAVE's firmware.
  - **Controls and screens:** the pad sound picker and the FADER BANK screen, which Clipper Pro's SOUNDS pads and
    fader screen are made from. The TRS MIDI OUT driver.
  - **The installer:** the update library of the installer page (`web/smkota.js`), the version of his SMK-37 site that
    installs from Chrome on Windows.
  - **Documentation:** the reverse engineering and documentation of the SMK-37.
- **ChanceTheMaker**, [sloop-smk](https://github.com/ChanceTheMaker/sloop-smk): ideas Clipper Pro and SLOOP Pro rebuilt:
  - the sustain pedal's measured levels;
  - a flush so that no note hangs on the TRS output;
  - the installer's MIDI session in a disposable document (`MidiSession` in `web/smkota.js`, after his
    `web/midi-session.js`): Chrome no longer holds the keyboard while it restarts into its loader and back (the
    browser freezes of Windows USB installs he reported, [sloop-pro #3](https://github.com/isod89/sloop-pro/issues/3)).

  He also studied Windows USB installs and the MIDI session.

## In the firmware Clipper Pro runs (through Clipper and SLOOP)

- **Andre Rezemini (andrerezemini)**, [sloop-lab](https://github.com/andrerezemini/sloop-lab): the sound on the second
  core (`firmware/src/cpu1.c`).
- **Charles Vestal**, [X0X](https://github.com/charlesvestal/fm1-x0x): the dual-core HAL (`firmware/hal/fm1_cpu1.h`,
  `fm1_cpu1.S`), and the supply levels for two cores.
- **keremimo**, [Melodee](https://github.com/keremimo/melodee): the dual-core start, with X0X.
- **juanjiant-bit**, [JIANT FM](https://github.com/juanjiant-bit/JIANT): the knob acceleration and the LEVEL / PAN glide.
- **renebohne**, **@ordenador** (Mario Faúndez Vidal) and the SLOOP and Felucca communities: through SLOOP.

## Research and tools

- **AL-255**, [FM-1-RE](https://github.com/AL-255/FM-1-RE): the FM-1's reverse engineering.
- **kagaimiq**, [jl-uboot-tool](https://github.com/kagaimiq/jl-uboot-tool) and the JieLi research: the full flash backup.
- **JieLi**, the [AC79 SDK](https://gitee.com/Jieli-Tech/fw-AC79_AIoT_SDK) (Apache-2.0): three of its files are read from
  your checkout at build time. None of them is in this repository.

## Ideas, libraries, fonts

As in Clipper ([README-CLIPPER.md](README-CLIPPER.md#credits)): the JUNO, VECTOR, FMX, WSEQ, MACRO, the filter models,
TURNG and SPICE / DICE are Clipper's own code after classic instruments; the supersaw follows Adam Szabo's analysis of
the JP-8000. Three engines and the screen font come from other projects:

- FM6 is msfa, from Dexed (Apache-2.0).
- PHYS comes from DaisySP and Mutable Instruments Rings (MIT).
- The screen font is Terminus (SIL OFL 1.1).

See [LICENSING.md](LICENSING.md).

If your work is in Clipper Pro and missing here, or credited wrongly, please open an issue: it will be fixed.

M-VAVE, SMK-37, MKE-P37, Starrykey and FM-1 are trademarks of their respective owners. Clipper Pro is independent and
not affiliated with them.
