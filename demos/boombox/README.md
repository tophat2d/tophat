# Tophat Boombox Demo

A retro 1980s portable cassette deck, FM stereo radio station, and dynamic audio visualizer built in [Umka](https://github.com/vtereshkov/umka-lang) for the [Tophat 2D Engine](https://github.com/tophat2d/tophat).

![Tophat Boombox](https://tophat2d.dev/logo.svg)

---

## Features

- **Interactive Cassette Deck**:
  - Mechanical piano-key transport controls (`REW`, `PLAY`, `PAUSE`, `STOP`, `FF`, `EJECT`).
  - Animated dual tape spools with spinning reel hubs and spoked teeth.
  - Dynamic tape ribbon winding simulation (tape ribbon physically transfers between spools as playback progresses).
  - High-speed winding animation on Rewind and Fast-Forward.

- **Multi-Track Mixtape Audio**:
  - **Track 1: "NEON NIGHTS"** — Driving 120 BPM Synthwave / Outrun track with punchy drums, rolling 16th-note bassline, and neon arpeggios.
  - **Track 2: "CYBER GROOVE"** — 128 BPM Chiptune Funk groove with syncopated slap bass, claps, and energetic chip lead melody.
  - **Track 3: "MIDNIGHT CASSETTE"** — 86 BPM relaxed Lo-Fi tape chillhop with warm Rhodes jazz chords and sub-bass.
  - Authentic mechanical sound effects (deck latch clicks, eject kachunk, high-speed tape whirr, and FM tuner static).

- **Audio Subsystem Showcase**:
  - **Master Volume**: Smooth logarithmic volume fader controlling `sound.setVol()`.
  - **Stereo Balance**: `L <---> R` panning slider directly controlling `sound.setPan()`.
  - **Tape Speed / Pitch**: Variable speed knob (`0.5x` to `2.0x`) controlling `sound.setPitch()` in real-time.
  - **FM Stereo Radio Mode**: Toggle between Cassette Tape and FM Radio tuner with a frequency dial slider (88–108 MHz). Hear clear stations when tuned or warm static hiss in between.

- **Dynamic Visualizer & Reactive Graphics**:
  - **Bass-Reactive Speakers**: Dual stereo woofers physically throb, flex, and bounce with the kick drum and bassline pulses.
  - **12-Band Spectrum Analyzer**: Segmented LED graphic equalizer with realistic peak-hold decay.
  - **Dual Stereo VU Meters**: Left & right channel level bars responsive to master volume, stereo panning, and bass boost.
  - **Digital LCD / VFD Display**: Vintage cyan/green phosphor display with track title ticker, elapsed time counter (`MM:SS`), and status badges.
  - **Mega Bass Toggle**: Boosts bass pulsation, low-frequency equalizer response, and speaker cone travel with glowing red LED status.

---

## Controls

### Mouse
- **Click & Drag**:
  - **VOL Slider**: Adjust master volume.
  - **PAN Slider**: Pan audio left / right.
  - **SPEED Slider**: Adjust playback speed & pitch.
  - **SEEK Bar**: Scrub through the tape timeline.
  - **TUNER Bar** (Radio Mode): Tune FM radio frequency.
- **Click**:
  - Piano Keys: `REW`, `PLAY`, `PAUSE`, `STOP`, `FF`, `EJECT`.
  - Toggles: `MEGA BASS`, `TAPE / FM`, `AUTO LOOP`.

### Keyboard Shortcuts
| Key | Action |
| :--- | :--- |
| **Space** | Play / Pause |
| **S** | Stop |
| **Left Arrow** | Tape Rewind (`REW`) |
| **Right Arrow** | Tape Fast-Forward (`FF`) |
| **Up / Down** | Volume Up / Down |
| **B** | Toggle Mega Bass |
| **E** | Eject / Next Mixtape |
| **M** | Toggle Tape / FM Radio Mode |
| **1, 2, 3** | Direct Track Selection |

---

## Running the Demo

From the root of the Tophat repository:

```bash
tophat -main demos/boombox/main.um
```

Or from within the `demos/boombox` directory:

```bash
cd demos/boombox && tophat
```
