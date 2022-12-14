#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <EEPROM.h>
#include <WiFi.h>

#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

#define EEPROM_SIZE 32

const char* ssid = "yourNetworkName";
const char* password =  "yourNetworkPass";
const uint16_t port = 8090;
const char* host = "192.168.1.83";

void addLog(String sValue) 
{
  Serial.println("*********");
  Serial.println(sValue);
  Serial.println("*********");
}

class MyCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
      std::string value = pCharacteristic->getValue();

      String sValue = "";

      if (value.length() > 0) {

         for (int i = 0; i < value.length(); i++)
           sValue = sValue + value[i];

        addLog("query: " + sValue);

        if (sValue == "on") {
          digitalWrite(2, HIGH);
          addLog("Pin 2 enabled");
        }

        if (sValue == "off") {
          digitalWrite(2, LOW);
          addLog("Pin 2 disabled");
        }

        if (sValue == "set") {
          setPassword(sValue);
          addLog("saved");
        }

        if (sValue == "read") {
          String s = getPassword();
          addLog(s);
        }
        
      }

      // pCharacteristic->setValue("ys:" + value);
    }
};

String getPassword() {
  String password;

  for (int i = 0; i < EEPROM_SIZE; i++) {
      byte readValue = EEPROM.read(i);

      if (readValue == 0) {
          break;
      }
      password = password + char(readValue);
  }

  return password;
}

void setPassword(String password) {
    int n = password.length();
    char char_array[n + 1];
    strcpy(char_array, password.c_str());
    int addr = 0;
    for (int i = 0; i < EEPROM_SIZE; i++) {
        EEPROM.write(addr, char_array[i]);
        addr += 1;
    }
}

void initBt() {
  BLEDevice::init("MyESP32");
  BLEServer *pServer = BLEDevice::createServer();

  BLEService *pService = pServer->createService(SERVICE_UUID);

  BLECharacteristic *pCharacteristic = pService->createCharacteristic(
                                         CHARACTERISTIC_UUID,
                                         BLECharacteristic::PROPERTY_READ |
                                         BLECharacteristic::PROPERTY_WRITE
                                       );

  pCharacteristic->setCallbacks(new MyCallbacks());

  pCharacteristic->setValue("Hello World");
  pService->start();

  BLEAdvertising *pAdvertising = pServer->getAdvertising();
  pAdvertising->start();
}

void initSocket() {
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    
  }

  if (WiFi.status() != WL_CONNECTED) {
    addLog("Wifi connected");
    Serial.println(WiFi.localIP());
  } else {
    addLog("Connect failed");
  }
}

void setup() {
  Serial.begin(115200);

  pinMode (2, OUTPUT);

  if (!EEPROM.begin(EEPROM_SIZE)) {

        delay(1000000);
    }

  initBt();
  initSocket();
}

void loop() {

    WiFiClient client; 

  if (client.connect(host, port)) {
    client.print("Hello from ESP32!");
    addLog("Message sent to client");
    client.stop();
  }

  delay(2000);
}
