# arduino-motor-control-l298n
Arduino UNO + L298N DC motor control (PWM speed + direction) with wiring notes and test sketch.
# Arduino Motor Control — L298N + DC Motors (UNO)

A minimal project to validate **direction + PWM speed control** of two DC motors using an **L298N** motor driver and **Arduino UNO**.

## Hardware
- Arduino UNO
- L298N motor driver
- 2x DC gear motors
- Battery pack (motor supply)

## Pin Mapping
- ENA = 5, ENB = 6
- IN1 = 7, IN2 = 8
- IN3 = 9, IN4 = 10

## Run
1. Open `src/motor_test.ino` in Arduino IDE
2. Board: Arduino Uno, select COM port
3. Upload
4. The robot cycles: forward → backward → left → right → stop

## Notes
Adjust `basePWM` and `turnPWM` (0–255) based on your motors and battery.
