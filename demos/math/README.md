# Math Dash (Tophat Demo)

An arcade-style, fast-paced speed arithmetic game built with the [Tophat 2D game engine](https://tophat2d.dev) in [Umka](https://github.com/vtereshkov/umka-lang).

## Features

- **4 Game Modes**:
  1. **Classic Survival**: 3 Lives, progressive levels from basic addition/subtraction to multiplication, division, and missing-variable algebra (`? + 8 = 25`, `? x 6 = 42`). Ramping speed and question timers.
  2. **Time Rush (60s Blitz)**: 60 seconds on the clock. Fast continuous problems. Correct answers award **+2.5s** bonus time, mistakes penalize **-3.5s**.
  3. **True or False Blitz**: Rapid-fire equations (`14 + 19 = 33` -> TRUE, `7 x 8 = 54` -> FALSE). Test your lightning reflexes!
  4. **Zen Practice**: Untimed, no lives lost, relaxed practice and warm-up.

- **Combo & Streak System**:
  - Build consecutive correct streaks to activate multipliers (`x2`, `x3`, `x4`, up to `x5 MAX`)!
  - Celebratory combo milestones: *GOOD!*, *GREAT!*, *AMAZING!*, *UNSTOPPABLE!*, *MATH WIZARD!*.
  - Floating score popups and star confetti particle bursts.

- **Audio & Visual Juice**:
  - Retro arcade aesthetic with drifting translucent math operators in the background.
  - Interactive cards with hover and press effects.
  - Screen shake and red flash on mistakes; celebratory green pulse on correct answers.
  - High score tracking per mode.
  - Full sound effects with in-game mute toggle (`[M]`).

## Controls

| Action | Control |
|---|---|
| **Select Answer (Multiple Choice)** | Click cards with **Mouse**, or press keys **[1]**, **[2]**, **[3]**, **[4]** |
| **Direct Numeric Typing** | Type digits `0-9`, `Backspace` to delete, press `[ENTER]` to submit |
| **True / False Blitz** | Press **[T]** or **[Right Arrow]** for TRUE; **[F]** or **[Left Arrow]** for FALSE (or click buttons) |
| **Pause / Resume** | Press **[ESC]** |
| **Restart / Play Again** | Press **[SPACE]** or **[ENTER]** on Game Over screen |
| **Toggle Sound** | Press **[M]** |

## How to Run

From the Tophat repository root:
```bash
./tophat -dir demos/math -main demos/math/main.um
```
Or from inside `demos/math/`:
```bash
../../tophat
```
