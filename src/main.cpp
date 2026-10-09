#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>

// ==============================
// WIFI
// ==============================
const char* ssid = "Wifi_Name";
const char* password = "Wifi_Password";

// ==============================
// MQTT / HIVEMQ
// ==============================
const char* mqtt_server = "broker.hivemq.com";
const int mqtt_port = 1883;

const char* mqtt_topic = "industrial_edge_gateway_2026/esp32";

// ==============================
// RS485 / MODBUS
// ==============================
#define RX_PIN 16
#define TX_PIN 17
#define RS485_DIR 4

#define MODBUS_BAUD 9600

HardwareSerial RS485Serial(2);

WiFiClient espClient;
PubSubClient mqttClient(espClient);


// ==============================
// RS485 TRANSMIT
// ==============================
void setTransmitMode()
{
    digitalWrite(RS485_DIR, HIGH);
}


// ==============================
// RS485 RECEIVE
// ==============================
void setReceiveMode()
{
    digitalWrite(RS485_DIR, LOW);
}


// ==============================
// SEND MODBUS REQUEST
// ==============================
void sendModbusFrame(const uint8_t *frame, size_t length)
{
    setTransmitMode();

    delayMicroseconds(100);

    RS485Serial.write(frame, length);

    RS485Serial.flush();

    delayMicroseconds(300);

    setReceiveMode();
}


// ==============================
// WIFI
// ==============================
void setupWiFi()
{
    Serial.println();
    Serial.println("Connecting to Wi-Fi...");

    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }

    Serial.println();
    Serial.println("Wi-Fi connected!");

    Serial.print("ESP32 IP: ");
    Serial.println(WiFi.localIP());
}


// ==============================
// MQTT CONNECTION
// ==============================
void reconnectMQTT()
{
    while (!mqttClient.connected())
    {
        Serial.print("Connecting to HiveMQ... ");

        String clientID = "ESP32_Gateway_";
        clientID += String(random(0xffff), HEX);

        if (mqttClient.connect(clientID.c_str()))
        {
            Serial.println("CONNECTED!");

            mqttClient.publish(
                mqtt_topic,
                "ESP32 Gateway Connected"
            );
        }
        else
        {
            Serial.print("FAILED, rc=");
            Serial.print(mqttClient.state());
            Serial.println();

            delay(2000);
        }
    }
}


// ==============================
// READ MODBUS
// ==============================
void readModbus()
{
    uint8_t request[] =
    {
        0xFF,
        0x01,
        0x00,
        0x00,
        0x00,
        0x08,
        0x28,
        0x12
    };

    // Clear old data
    while (RS485Serial.available())
    {
        RS485Serial.read();
    }

    Serial.println();
    Serial.println("==============================");
    Serial.println("MODBUS REQUEST");
    Serial.println("==============================");

    for (int i = 0; i < sizeof(request); i++)
    {
        if (request[i] < 0x10)
            Serial.print("0");

        Serial.print(request[i], HEX);
        Serial.print(" ");
    }

    Serial.println();

    // Send request
    sendModbusFrame(request, sizeof(request));

    Serial.println("Waiting for response...");

    // Wait for response
    unsigned long startTime = millis();

    while (millis() - startTime < 1000)
    {
        if (RS485Serial.available())
            break;
    }

    // Read response
    uint8_t response[64];

    int count = 0;

    while (RS485Serial.available() && count < 64)
    {
        response[count] = RS485Serial.read();
        count++;
    }

    // ==============================
    // NO DATA
    // ==============================

    if (count == 0)
    {
        Serial.println("NO RESPONSE");

        mqttClient.publish(
            mqtt_topic,
            "MODBUS RESPONSE: NO DATA"
        );

        return;
    }

    // ==============================
    // PRINT RAW RESPONSE
    // ==============================

    Serial.print("Received ");
    Serial.print(count);
    Serial.println(" bytes:");

    Serial.print("RAW: ");

    char mqttMessage[300];

    strcpy(mqttMessage, "MODBUS RAW:");

    for (int i = 0; i < count; i++)
    {
        if (response[i] < 0x10)
            Serial.print("0");

        Serial.print(response[i], HEX);
        Serial.print(" ");

        char temp[6];

        sprintf(temp, " %02X", response[i]);

        strcat(mqttMessage, temp);
    }

    Serial.println();

    // ==============================
    // SEND RAW DATA TO MQTT
    // ==============================

    mqttClient.publish(
        mqtt_topic,
        mqttMessage
    );

    Serial.println("Sent raw response to MQTT.");
}


// ==============================
// SETUP
// ==============================
void setup()
{
    Serial.begin(115200);

    delay(1000);

    Serial.println();
    Serial.println("==============================");
    Serial.println("INDUSTRIAL EDGE GATEWAY");
    Serial.println("==============================");
    Serial.println("ESP32 + WiFi + MQTT + Modbus");
    Serial.println("==============================");

    // RS485 direction
    pinMode(RS485_DIR, OUTPUT);

    setReceiveMode();

    // RS485 UART
    RS485Serial.begin(
        MODBUS_BAUD,
        SERIAL_8N1,
        RX_PIN,
        TX_PIN
    );

    // Wi-Fi
    setupWiFi();

    // MQTT
    mqttClient.setServer(
        mqtt_server,
        mqtt_port
    );

    delay(1000);
}


// ==============================
// LOOP
// ==============================
void loop()
{
    // MQTT connection
    if (!mqttClient.connected())
    {
        reconnectMQTT();
    }

    mqttClient.loop();

    // Read Modbus every 5 seconds
    static unsigned long lastRead = 0;

    if (millis() - lastRead >= 5000)
    {
        lastRead = millis();

        readModbus();
    }
}