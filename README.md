# 🔐 Home-Security-System

A multi-stage home security and access control system built for **Arduino Mega 2560**. It combines night-time motion detection with a 4-step authentication sequence before opening a servo-driven portal.

## ✨ Features & Security Stages

1. **Initialization**: Red status LED turns on while the servo performs a `0°` to `180°` loading sweep, then switches to green once ready.
2. **Night & Motion Detection**: Monitors ambient light via a photoresistor (`A0`). When dark (`luminosity <= 80`), a white LED turns on; if the PIR sensor detects motion, the buzzer alarm and flashing white LED trigger until an RFID tag is presented.
3. **Stage 1 — RFID Authentication**: Scans an MFRC522 badge or card and checks its UID against authorized tags.
4. **Stage 2 — Membrane Keypad**: Prompts for a 4-digit PIN on a 4×4 matrix keypad.
5. **Stage 3 — IR Remote & MAX7219 Matrix**: Requires a 4-key sequence on an infrared remote; upon success, alternates a checkmark logo and a smiley on an 8×8 MAX7219 LED matrix.
6. **Stage 4 — Push-Button Sequence & Portal**: Requires pressing a sequence of physical push buttons, then opens the servo "portal" (`90°` → `0°`) and plays an RGB LED color transition.

All prompts, errors, and progress updates are displayed on a **16×2 LCD**, accompanied by per-stage indicator LEDs (red, yellow, green, blue) and confirmation/refusal LEDs.

## 🧩 Hardware & Pinout

Designed for **Arduino Mega 2560** (uses pins up to `53`):

| Component | Pins |
| :--- | :--- |
| Photoresistor (Photocell) | `A0` |
| PIR Motion Sensor | `48` |
| White Alert LED | `25` |
| Active Buzzer | `44` |
| MFRC522 RFID Reader | `SDA: 20`, `RST: 49` (+ hardware SPI) |
| IR Receiver | `47` |
| MAX7219 8×8 LED Matrix | `DIN: 22`, `CLK: 23`, `CS: 24` |
| 16×2 LCD Display | `30, 31, 32, 33, 34, 35` |
| 4×4 Membrane Keypad | Rows: `46, 10, 8, 7` / Cols: `41, 42, 43, 45` |
| 5 Push Buttons (`INPUT_PULLUP`) | `36, 37, 38, 39, 40` |
| Servo Motor (Portal) | `11` |
| Stage LEDs (Red, Yellow, Green, Blue) | `26, 28, 27, 29` |
| Confirmation / Refusal LEDs | `53` / `12` |
| RGB LEDs | `3, 4, 5` and `13, 6, 9` |

## 📚 Required Libraries

- `LedControl` (MAX7219)
- `IRremote` (IR receiver)
- `LiquidCrystal` (built-in)
- `Keypad`
- `Servo` (built-in)
- `SPI` (built-in)
- `MFRC522` (RFID)

## ⚠️ Notes & Limitations

- **Default Credentials**: Example RFID UIDs (`UIDbadge`, `UIDcarte`) and default passcodes (`1234` for keypad/remote, `12345` for buttons) are hardcoded in `home_security.ino` and should be customized before deployment.
- **Sequential Execution**: Actions run sequentially without multitasking; as noted in the sketch, the Active Buzzer tone library conflicts with `IRremote`.
- **Stage 4 Check**: The button-sequence verification loop checks the first 4 entries (`i < 4`) of the 5-button array.

## 🛠️ Details

The project has been created the April 1st of 2023.
