/*
 * This is a configuration file for the "esp8266_nqtt_lcd16x2"
 * Rename this file to "config.h"
 */

// LCD
#define SCL_PIN 14
#define SDA_PIN 12
#define I2C_ADDRESS 0x27
#define LCD_CHARS 16
#define LCD_LINES 2

// WiFi
#define CONFIG_WIFI_SSID "wifi"
#define CONFIG_WIFI_PASS "pass123"

// MQTT
#define CONFIG_MQTT_HOST "192.168.0.5"
#define CONFIG_MQTT_USER ""
#define CONFIG_MQTT_PASS ""

#define CONFIG_MQTT_CLIENT_ID "ESPLCD16X2" // Must be unique on the MQTT network

// MQTT Topics
#define CONFIG_MQTT_TOPIC_SET "home/wifilcd/set"

// Enables Serial and print statements
#define CONFIG_DEBUG true
