#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <EEPROM.h>

// See the following for generating UUIDs:
// https://www.uuidgenerator.net/

#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

#define EEPROM_SIZE 1


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

          uint8_t PublicKeyData[1];

          std::copy(sValue.begin(), sValue.end(), PublicKeyData);

          uint8_t* d = PublicKeyData;
          
          EEPROM.write(0, *d);
          EEPROM.commit();
          addLog("saved");
        }

        if (sValue == "read") {
          uint8_t s = EEPROM.read(0);
          addLog(String((char *)s));
        }
        
      }

      // pCharacteristic->setValue("ys:" + value);
    }
};



void setup() {
  Serial.begin(115200);

  pinMode (2, OUTPUT);

  Serial.println("1- Download and install an BLE scanner app in your phone");
  Serial.println("2- Scan for BLE devices in the app");
  Serial.println("3- Connect to MyESP32");
  Serial.println("4- Go to CUSTOM CHARACTERISTIC in CUSTOM SERVICE and write something");
  Serial.println("5- See the magic =)");

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

void loop() {
  // put your main code here, to run repeatedly:
  delay(2000);
}
