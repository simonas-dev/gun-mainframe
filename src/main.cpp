#include <Arduino.h>
#include <ArduinoBLE.h>

#define IS_DEBUG false

#define SERVER_UUID "5732d41c-e40b-4ec9-8e17-bc61ba185486"
#define DATA_1_UUID "0609d529-b3a9-4d18-ac96-e09a02d14cdf"
#define DATA_2_UUID "315fb3e2-9c5a-4784-8d30-8dddca5b9625"
#define DATA_3_UUID "fd3d3f21-ab45-4f0d-b0c7-de1d392df061"

#define DEFAULT_DATA_1 49
#define DEFAULT_DATA_2 40
#define DEFAULT_DATA_3 30

void search();
void connect(int attemptNum);
void readData();

void setup() {
    if (IS_DEBUG) {
        Serial.begin(9600);
        while (!Serial);
    }

  // initialize the Bluetooth® Low Energy hardware
  BLE.begin();

  Serial.println("Setup done.");
}

bool isConntected = false;
bool isFound = false;
BLEDevice device;
int connectionAttempts = 0;
bool localDataWritten = false;

int payload1 = DEFAULT_DATA_1;
int payload2 = DEFAULT_DATA_2;
int payload3 = DEFAULT_DATA_3;

void loop() {
    BLE.setDeviceName("Ballistic Mainframe");

    if (!isFound) {
        search();
    } else if (!isConntected) {
        connect(1);
        isConntected = device.connected();
    } else if (isConntected) {
        readData();
        isConntected = device.connected();
        if (!isConntected) {
            isFound = false;
        }
    }
}

void readData() {
    BLECharacteristic data1 = device.characteristic(DATA_1_UUID);
    BLECharacteristic data2 = device.characteristic(DATA_2_UUID);
    BLECharacteristic data3 = device.characteristic(DATA_3_UUID);
    if (!localDataWritten) {
        Serial.println("Write local: ");
        Serial.println(payload1);
        data1.writeValue(payload1);
        data2.writeValue(payload2);
        data3.writeValue(payload3);
        localDataWritten = true;
    }
    int temp;
    data1.readValue(temp);
    if (temp != 0) {
        payload1 = temp;
    }
    data2.readValue(temp);
    if (temp != 0) {
        payload2 = temp;
    }
    data3.readValue(temp);
    if (temp != 0) {
        payload3 = temp;
    }
    Serial.print("Read remote: ");
    Serial.print(payload1);
    Serial.print(' ');
    Serial.print(payload2);
    Serial.print(' ');
    Serial.print(payload3);
    Serial.println();
    delay(32);
}

void connect(int attemptNum) {
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
        if (attemptNum > 5) {
            device.disconnect();
            return;
        } else {
            connect(attemptNum + 1);
        }
    }
    localDataWritten = false;
}

void search() {
    BLE.scanForUuid(SERVER_UUID);
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
    delay(32);
}
