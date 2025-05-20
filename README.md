# ESP Home Monitoring Project 🌐📡

This project is a full-stack IoT system that uses an ESP microcontroller to monitor environmental conditions and simulate a flood detection event. The system includes:

- 🌡️ Data collection from a **DHT11 temperature and humidity sensor**
- 💡 Remote control of an LED via a web interface
- 🚨 Flood simulation via a physical button on the ESP
- ⚙️ A **.NET backend** handling WebSocket communication and data persistence
- 🖥️ A **Blazor frontend** providing real-time updates and controls

---

## 🔧 Project Structure

This repository is organized using separate branches for each part of the system:

| Branch        | Description                                |
|---------------|--------------------------------------------|
| `esp-code`    | Firmware for the ESP (DHT11, LED, Button)  |
| `backend`     | .NET backend handling WebSockets and logic |
| `frontend`    | Blazor-based web frontend UI               |

---

## 📡 Features

- **ESP8266/ESP01-based firmware**
  - Reads temperature and humidity from a DHT11 sensor
  - Sends data to backend via WebSockets
  - Listens for LED toggle commands
  - Sends flood event when button is pressed

- **.NET Backend**
  - Receives and processes WebSocket messages from the ESP
  - Sends commands to ESP (e.g., toggle LED)
  - Stores or routes sensor data as needed

- **Blazor Frontend**
  - Real-time display of temperature and humidity
  - Button to turn the LED on/off remotely
  - Indicator for flood event (simulated via button press on ESP)

---

## 🚀 Getting Started

### Prerequisites

- ESP8266 or ESP01 microcontroller
- DHT11 sensor
- Push button
- LED + resistor
- .NET 6.0+ SDK
- Node.js (optional if using frontend tools)
- Visual Studio or VS Code

---
