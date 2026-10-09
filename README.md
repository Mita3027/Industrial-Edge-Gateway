# Industrial Edge Gateway using ESP32, RS485 and MQTT

## 📌 Overview
The **Industrial Edge Gateway** is an IoT-based project that uses an ESP32 to connect industrial devices through RS485 serial communication and publish received data to an MQTT broker over Wi-Fi. It demonstrates the integration of industrial communication protocols with IoT-based remote monitoring.

## 🎯 Features
- ESP32-based edge gateway
- RS485 serial communication
- Modbus-style request transmission
- Raw hexadecimal response monitoring
- Wi-Fi connectivity
- MQTT messaging using HiveMQ
- Periodic data collection and remote message publishing

## 🛠️ Hardware Requirements
- ESP32 development board
- RS485 transceiver module (e.g., MAX485)
- Compatible industrial field device
- Jumper wires and power supply

## 💻 Technologies Used
- C++
- Arduino Framework
- Visual Studio Code
- PlatformIO
- RS485 and Modbus-style communication
- Wi-Fi and MQTT
- HiveMQ Public Broker

## ⚙️ Working Principle
1. The ESP32 connects to a Wi-Fi network.
2. It sends a request frame to the connected device through RS485.
3. The ESP32 waits for a response and displays the received bytes in hexadecimal format.
4. The response, or a no-response status, is published to an MQTT broker.
5. An MQTT subscriber can monitor the messages remotely.

## 📡 MQTT Configuration
- **Broker:** `broker.hivemq.com`
- **Port:** `1883`
- **Topic:** `industrial_edge_gateway_2026/esp32`

## 🚀 Getting Started
1. Clone this repository:
   ```bash
   git clone https://github.com/Mita3027/Industrial-Edge-Gateway.git
   ```
2. Open the project in Visual Studio Code with PlatformIO installed.
3. Configure your Wi-Fi credentials.
4. Build and upload the firmware to the ESP32.
5. Open the Serial Monitor at **115200 baud**.
6. Subscribe to the MQTT topic using an MQTT client to view published messages.

## 🔮 Future Improvements
- Decode Modbus registers into meaningful values.
- Add CRC validation and error handling.
- Support multiple industrial devices.
- Implement offline data buffering.
- Develop a real-time monitoring dashboard.
- Add secure MQTT communication using authentication and TLS.

## 👩‍💻 Author
**Mita Patil**

GitHub: [Mita3027](https://github.com/Mita3027)

## ⚠️ Note
This project is a prototype for educational and demonstration purposes. The current firmware publishes raw serial responses and does not yet implement complete Modbus RTU response validation. Avoid committing real Wi-Fi credentials to the repository.
