#include <NimBLEDevice.h>
#include "Spark.h"

//#define MIDI_SERVICE_UUID         "00001803-0000-1000-8000-00805f9b34fb" // MIDI Service UUID
//#define MIDI_CHARACTERISTIC_UUID  "00002A06-0000-1000-8000-00805f9b34fb" // MIDI Characteristic UUID

#define MIDI_SERVICE_UUID         "03b80e5a-ede8-4b33-a751-6ce34ec4c700" // MIDI Service UUID
#define MIDI_CHARACTERISTIC_UUID  "7772e5db-3868-4112-a1a9-f2669d106bf3" // MIDI Characteristic UUID
#define MIDI_ADDRESS "08:29:78:37:7A:AD" 


NimBLEClient* pClient;
NimBLERemoteService* pMidiService;
NimBLERemoteCharacteristic* pMidiCharacteristic;
void connetctFootCtrl();

class MyCallbacks : public NimBLEClientCallbacks {
    void onConnect(NimBLEClient* pClient) {
        Serial.println("Connected to BLE MIDI device");
    }

    void onDisconnect(NimBLEClient* pClient) {
        Serial.println("Disconnected from BLE MIDI device");

        ESP.restart();
       
        Serial.println("Setup complete.");
        }
};

void notifyCallback(NimBLERemoteCharacteristic* pBLERemoteCharacteristic, uint8_t* pData, size_t length, bool isNotify) {
    if (isNotify) {
        Serial.print("Received MIDI Message: ");


        int midi_chan, midi_cmd,midi_data; bool onoff;
        midi_chan = (pData[3] & 0x0f) + 1;
        midi_cmd = pData[2] & 0xf0;
        midi_data = pData[3] & 0xf0;
        onoff = (pData[2] == 127 ? true : false);        
        for (size_t i = 0; i < length; i++) {
            Serial.print(pData[i], HEX);
            Serial.print(" ");
        }
        Serial.println();

    if (midi_cmd == 0xc0)  {
      switch (pData[3]) {
        case 0:              change_hardware_preset(0);                 break; // MIDI Commander BIFX A
        case 1:              change_hardware_preset(1);                 break; // MIDI Commander BIFX B
        case 2:              change_hardware_preset(2);                 break; // MIDI Commander BIFX C
        case 3:              change_hardware_preset(3);                 break; // MIDI Commander BIFX D
      }
    
    }
    
  }
        
}

void connetctFootCtrl() {

  NimBLEDevice::init("ESP32 MIDI Client");
    pClient = NimBLEDevice::createClient();

    MyCallbacks* pCallbacks = new MyCallbacks();
    pClient->setClientCallbacks(pCallbacks);

    Serial.println("Connecting to BLE MIDI device...");
    NimBLEAddress serverAddress(MIDI_ADDRESS); // Replace with the address of your BLE MIDI device
    pClient->connect(serverAddress, true);

    Serial.println("Getting MIDI service...");
    pMidiService = pClient->getService(NimBLEUUID(MIDI_SERVICE_UUID));

    if (pMidiService) {
        Serial.println("Getting MIDI characteristic...");
        pMidiCharacteristic = pMidiService->getCharacteristic(NimBLEUUID(MIDI_CHARACTERISTIC_UUID));

        if (pMidiCharacteristic) {
            Serial.println("Registering notify callback...");
            pMidiCharacteristic->registerForNotify(notifyCallback);
        } else {
            Serial.println("Failed to get MIDI characteristic!");
        }
    } else {
        Serial.println("Failed to get MIDI service!");
    }
}

void setup() {
    Serial.begin(115200);
    Serial.println("Starting setup...");


    DEBUG("Spark connecting ...");
    while (!spark_state_tracker_start()) {  // set up data to track Spark and app state, if it fails to find the Spark it will return false
      DEBUG("No Spark found - perhaps sleep?");// think about a deep sleep here if needed
    }
    DEBUG("Spark found and connected - starting");

    connetctFootCtrl();


    Serial.println("Setup complete.");
}

void loop() {
    // Nothing to do in the loop for this basic example.
}

