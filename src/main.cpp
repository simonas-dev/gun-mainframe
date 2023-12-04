#include <Arduino.h>

/*
  LED Control

  This example scans for Bluetooth® Low Energy peripherals until one with the advertised service
  "19b10000-e8f2-537e-4f6c-d104768a1214" UUID is found. Once discovered and connected,
  it will remotely control the Bluetooth® Low Energy peripheral's LED, when the button is pressed or released.

  The circuit:
  - Arduino MKR WiFi 1010, Arduino Uno WiFi Rev2 board, Arduino Nano 33 IoT,
    Arduino Nano 33 BLE, or Arduino Nano 33 BLE Sense board.
  - Button with pull-up resistor connected to pin 2.

  You can use it with another board that is compatible with this library and the
  Peripherals -> LED example.

  This example code is in the public domain.
*/
#include <ArduinoBLE.h>

void search();
void connect();
void readData();

void setup() {
  Serial.begin(9600);
  while (!Serial);

  // initialize the Bluetooth® Low Energy hardware
  BLE.begin();

  Serial.println("Bluetooth® Low Energy Central - LED control");
}

bool isConntected = false;
bool isFound = false;
BLEDevice device;
int connectionAttempts = 0;
bool localDataWritten = false;
int payload1 = 50;

void loop() {
    BLE.setDeviceName("Ballistic Mainframe");

    if (!isFound) {
        search();
    } else if (!isConntected) {
        connect();
    } else if (isConntected) {
        readData();
    }
    isConntected = device.connected();
}

void readData() {
    BLECharacteristic data1 = device.characteristic("0609d529-b3a9-4d18-ac96-e09a02d14cdf");
    if (!localDataWritten) {
        Serial.print("Write: ");
        Serial.print(payload1);
        Serial.println();
        data1.writeValue(payload1);    
        localDataWritten = true;
    }
    data1.readValue(payload1);
    Serial.print("Read: ");
    Serial.print(payload1);
    Serial.println();
    sleep(1);
}

void connect() {
    Serial.println("Connecting ...");
    if (device.connect()) {
        Serial.println("Connected");
    } else {
        isFound = false;
        Serial.println("Failed to connect!");
        return;
    }

    // discover device attributes
    Serial.println("Discovering attributes ...");
    if (device.discoverAttributes()) {
        Serial.println("Attributes discovered");
    } else {
        Serial.println("Attribute discovery failed!");
        device.disconnect();
        return;
    }
    localDataWritten = false;
}

void search() {
    BLE.scanForUuid("5732d41c-e40b-4ec9-8e17-bc61ba185486");
    BLEDevice peripheral = BLE.available();
    if (peripheral) {
        // discovered a peripheral, print out address, local name, and advertised service
        Serial.println("Found:");
        Serial.println(peripheral.address());
        Serial.println(peripheral.deviceName());
        Serial.println(peripheral.advertisedServiceUuidCount());
        int serviceCnt = peripheral.advertisedServiceUuidCount();
        Serial.println(peripheral.advertisedServiceUuid(0));
        Serial.println();
        Serial.println();

        // discover peripheral attributes

        device = peripheral;
        BLE.stopScan();
        isFound = true;
    } else {
        Serial.println("Not found!");
    }
    sleep(1);
}
