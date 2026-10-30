# Arc-Clock (Iron Man / JARVIS Edition)

Custom ESP32 WROOM Arc-Clock featuring an Iron Man themed HUD, live DHT11 telemetry, capacitive touch interactions, Cherry MX switches, an ST7789 display (76x284 long-strip), mini-games, and a hosted WiFi web dashboard.

---

## ⚡ GPIO Pinout (Configurable Variables)

| Component | Pin Variable | ESP32 GPIO | Description |
| :--- | :--- | :--- | :--- |
| **Buzzer** | `pin_buzz` | **4** | Active/Passive buzzer (repulsor SFX & alarms) |
| **TTP223 Touch** | `pin_touch` | **19** | Capacitive touch sensor |
| **DHT11** | `pin_dht` | **21** | Temperature & Humidity data |
| **Button 1 (MX)** | `pin_b1` | **32** | Thrust UP (Game) / Hour + (Alarm) |
| **Button 2 (MX)** | `pin_b2` | **33** | Dive DOWN (Game) / Min + (Alarm) |
| **Button 3 (MX)** | `pin_b3` | **22** | Mode Cycle (HUD / Dash / Game / Alarm) |
| **Button 4 (MX)** | `pin_b4` | **23** | Action / Alarm Dismiss / Game Restart |
| **TFT SDA (MOSI)**| `pin_mosi` | **13** | SPI MOSI |
| **TFT SCL (SCLK)**| `pin_sclk` | **14** | SPI SCLK |
| **TFT CS** | `pin_cs` | **27** | SPI Chip Select |
| **TFT DC** | `pin_dc` | **26** | Data / Command |
| **TFT BL** | `pin_bl` | **25** | Backlight (Active LOW) |

---

## 🚀 Operating Modes (Cycle with Button 3)

1. **Mode 0: Iron Man Arc Clock HUD**
   - Animated Arc Reactor graphic with rotating spokes.
   - Large digital clock (`HH:MM:SS`).
   - Live DHT11 temperature and humidity.
   - WiFi connection & alarm status.
   - Tap Touch Sensor for Repulsor blast sound.

2. **Mode 1: Stark Telemetry Dashboard**
   - Visual bar gauges for Temperature (°C) and Humidity (%).
   - WiFi IP address and network status.
   - Touch sensor state, uptime, and available RAM.

3. **Mode 2: "Arc Dodger" Mini-Game**
   - Fly the Iron Man armor dodging incoming missiles and drones.
   - **B1**: Thrust Up
   - **B2**: Dive Down
   - **Touch Sensor**: Fire Repulsor Beam laser (destroys drones in path)
   - **B4**: Reboot suit / restart game on crash

4. **Mode 3: Alarm Protocol & SFX Test**
   - **B1**: Increment Hour
   - **B2**: Increment Minute
   - **B4**: Toggle Alarm ON/OFF
   - **Touch Sensor**: Trigger Repulsor sound effect test

---

## 🌐 WiFi & Web Dashboard

1. Set your WiFi credentials at the top of [`Firmware/Code.ino`](file:///home/l0stsky/ExTraStuff/Competitions/Clock/Firmware/Code.ino):
   ```cpp
   const char* ssid = "YOUR_WIFI_NAME";
   const char* pass = "YOUR_WIFI_PASSWORD";
   ```
2. When connected, the clock syncs accurate time automatically via NTP (`pool.ntp.org`).
3. Open `http://<ESP32_IP>` in your browser or phone to access the JARVIS Web Dashboard with live sensor data and remote controls.
