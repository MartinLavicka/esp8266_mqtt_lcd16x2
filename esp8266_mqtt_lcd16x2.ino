/*
  WiFi display 16x2
*/

// Set configuration options for pins, WiFi, and MQTT in the following file:
#include "config.h"

#include <ArduinoJson.h>
#include <Wire.h>
#include <LiquidCrystal_PCF8574.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

const bool debug_mode = CONFIG_DEBUG;

// lcd
const int scl = SCL_PIN;
const int sda = SDA_PIN;
const long i2c_adr = I2C_ADDRESS;
const int lcd_chars = LCD_CHARS;
const int lcd_lines = LCD_LINES;

// wifi
const char* ssid = CONFIG_WIFI_SSID;
const char* password = CONFIG_WIFI_PASS;
// mqtt
const char* mqtt_server = CONFIG_MQTT_HOST;
const char* mqtt_username = CONFIG_MQTT_USER;
const char* mqtt_password = CONFIG_MQTT_PASS;
const char* client_id = CONFIG_MQTT_CLIENT_ID;
// topics
const char* set_topic = CONFIG_MQTT_TOPIC_SET;
// json
const int BUFFER_SIZE = JSON_OBJECT_SIZE(15);
// globals
String lcd_line1 = "                ";
String lcd_line2 = "                ";
bool content_change = true;

WiFiClient espClient;
PubSubClient client(espClient);
LiquidCrystal_PCF8574 lcd(0x27);

void setup() {
  if (debug_mode) {
    Serial.begin(115200);
  }
  delay(1000);
  Serial.println("Starting ...");

  Serial.println("TEST: Check LCD");
  Wire.begin();
  Wire.beginTransmission(0x27);
  int error = Wire.endTransmission();
  Serial.print("Error: ");
  Serial.print(error);
  if (error == 0) {
    Serial.println(": LCD found.");
  } else {
    Serial.println(": LCD not found.");
  }
  lcd.begin(lcd_chars, lcd_lines);
  lcd.setBacklight(255);
  lcd.clear();
  lcd.noBlink();
  lcd.noCursor();

  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to: ");
  set_lcd_line1("Connecting to: ");
  Serial.println(ssid);
  set_lcd_line2(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  bool r = true;
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
    if (r) {
      lcd.setCursor(15, 1);
      lcd.print(".");
      r = false;
    }
    else {
      lcd.setCursor(15, 1);
      lcd.print(" ");
      r = true;
    }

  }

  Serial.println("");
  set_lcd_line1("WiFi connected");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
  set_lcd_line2("IP:" + WiFi.localIP());  
}

/*
  SAMPLE PAYLOAD:
  {
    "line1": "ahoj",
    "line2": "martine"
  }
*/
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");

  char message[length + 1];
  for (int i = 0; i < length; i++) {
    message[i] = (char)payload[i];
  }
  message[length] = '\0';
  Serial.println(message);

  if (!processJson(message)) {
    return;
  }
}

bool processJson(char* message) {
  StaticJsonBuffer<BUFFER_SIZE> jsonBuffer;

  JsonObject& root = jsonBuffer.parseObject(message);

  if (!root.success()) {
    Serial.println("parseObject() failed");
    return false;
  }

  if (root.containsKey("line1")) {
    lcd_line1 = root["line1"].as<String>();
    content_change = true;
  }
  else {
    lcd_line1 = "";
    content_change = true;
  }

  if (root.containsKey("line2")) {
    lcd_line2 = root["line2"].as<String>();
    content_change = true;
  }
  else {
    lcd_line2 = "";
    content_change = true;
  }
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("MQTT connection: ");    
    set_lcd_line1("MQTT connection:");
    if (client.connect(client_id, mqtt_username, mqtt_password)) {
      Serial.println("connected");
      set_lcd_line2("connected");
      client.subscribe(set_topic);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      delay(2000);
      set_lcd_line2(" try again in 5 seconds");
      // Wait 3 seconds before retrying
      delay(3000);
    }
  }
}


void loop() {

  if (!client.connected()) {
    reconnect();
    content_change = true;
  }
  client.loop();
  //Serial.println("2");

  //  if (lcd_line1.length() < lcd_chars); {
  //    do
  //    {
  //      //lcd_line1 += " ";
  //      lcd_line1 += ".";
  //      Serial.println("3");
  //      Serial.println(lcd_line1);
  //    } while (lcd_line1.length() < lcd_chars);
  //  }

  if (content_change) {
    set_lcd_content();
    Serial.println("New LCD content");
  }
}

void set_lcd_content() {
  lcd.setCursor(0, 0);
  lcd.print(lcd_line1);
  lcd.print("                ");
  Serial.println("Line1: " + lcd_line1);
  lcd.setCursor(0, 1);
  lcd.print(lcd_line2);
  lcd.print("                ");
  Serial.println("Line2: " + lcd_line2);
  content_change = false;
}

void set_lcd_line1(String line1) {
  lcd.setCursor(0, 0);
  lcd.print(line1 + "                ");
  content_change = false;
}

void set_lcd_line2(String line2) {
  lcd.setCursor(0, 1);
  lcd.print(line2 + "                ");
  content_change = false;
}
