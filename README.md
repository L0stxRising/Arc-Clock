# Arc-Clock

Arc Clock Is a 4 key Custom Iron man ARC reactor Themed Clock made by me (L0stsky)!!! Its my first project with a pcb.

---

## What is this??
Arc Clock is a Custom alarm clock That is based on the Theme of Iron Man. It Uses a PCB, a 3D Printed case and Various Other sensors (battery if you want!)
The clock has 4 Buttons for Various Funcs like setting alarms, changing time, playing games etc!

---
## Materials Used
Buzzer
Esp32 Wroom
2.25" dispay module
DHT11 Sensor
4* Mx cherry switches and keycaps
Mini 360 Buck
3D printed Case

---

## Operating Modes (Cycle with Button 3)
Arc Clock has 3 Modes:

1. **Mode 0: Iron Man Arc Clock HUD**
   - Animated Arc Reactor graphic with rotating spokes.
   - Large digital clock (`HH:MM:SS`).
   - Live DHT11 temperature and humidity.
   - WiFi connection & alarm status.
   - Tap Touch Sensor for Repulsor blast sound.

2. **Mode 1: Telemetry Dashboard**
   - Visual bar for Temperature and Humidity
   - WiFi network status.
   - Touch sensor state, uptime, and available RAM.

3. **Mode 2: "Arc Dodger" Mini-Game**
   - Fly the Iron Man armor dodging incoming missiles and drones.
   - **B1**: Thrust Up
   - **B2**: Dive Down
   - **Touch Sensor**: Fire Repulsor Beam laser
   - **B4**: Reboot suit / restart game on crash

4. **Mode 3: Alarm Protocol & SFX Test**
   - **B1**: Increment Hour
   - **B2**: Increment Minute
   - **B4**: Toggle Alarm ON/OFF
   - **Touch Sensor**: Trigger Repulsor sound effect test

---

## A web Control!!
1. Has auto Connect to wifi (set the creds in the Code) and Auto Updates Time!!
2. Automatically goes to sleep after inactivity (can be changed)
3. Has a website to control it to make it ring or change the Background and more!

---

## AI Usage
1. Used AI to help me with the PCB making as i didn't understand a single thing about them.
2. Used a Lil AI help in the Code

---

## How to use
1. Download the Repo
2. Update the Code with you wifi Credentials
3. Flash to your esp32!

---

## Creator
Made by L0stsky ;D with KiCad
