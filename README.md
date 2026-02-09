# Arduino Motor Control — L298N + DC Motors (UNO)

A minimal project to validate **direction control** and **PWM speed control** of two DC motors using an **L298N** motor driver and **Arduino UNO**.

## Hardware
- Arduino UNO
- L298N motor driver
- 2x DC gear motors
- Battery pack (motor supply)

## Wiring / Pin Mapping

| L298N | Arduino |
|------|---------|
| ENA  | D5 (PWM) |
| IN1  | D7 |
| IN2  | D8 |
| ENB  | D6 (PWM) |
| IN3  | D9 |
| IN4  | D10 |

> **Note:** Make sure **Arduino GND and L298N GND are common** (shared ground).  
> Motor power can be supplied from a separate battery pack.

## Run
1. Open `src/motor_test.ino` in Arduino IDE
2. Board: **Arduino Uno** → Select COM Port
3. Upload
4. The robot cycles: **forward → backward → left → right → stop**

## Project Structure
- `src/motor_test.ino` — motor control test sketch
- `images/` — optional photos / demo media

## Notes
- Adjust `basePWM` and `turnPWM` (0–255) based on motors and battery.
- If motors don’t spin, check **battery voltage**, **GND common**, and **ENA/ENB jumpers** on L298N.
