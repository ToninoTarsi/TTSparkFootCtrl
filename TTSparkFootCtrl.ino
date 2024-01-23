#include <NimBLEDevice.h>
#include <Arduino.h>
#include "Spark.h"


#define MIDI_SERVICE_UUID         "03b80e5a-ede8-4b33-a751-6ce34ec4c700" // MIDI Service UUID
#define MIDI_CHARACTERISTIC_UUID  "7772e5db-3868-4112-a1a9-f2669d106bf3" // MIDI Characteristic UUID
//#define MIDI_ADDRESS "08:29:78:37:7A:AD" 
#define MIDI_ADDRESS "11:61:7A:B5:E5:2F"

NimBLEClient* pClient;
NimBLERemoteService* pMidiService;
NimBLERemoteCharacteristic* pMidiCharacteristic;
void connetctFootCtrl();
int mode = 0;

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
        


        int midi_chan, midi_cmd,midi_data; bool onoff;
        
        midi_cmd = pData[2] ;
        midi_data = pData[3] % 4;
         
        Serial.print("Received MIDI Message: ");     
        for (size_t i = 0; i < length; i++) {
            Serial.print(pData[i], HEX);
            Serial.print(" ");
        }
        Serial.println();

        if (midi_cmd == 0xB0)  {
          if (midi_data == 2) mode = 0;
          if (midi_data == 3) mode = 1;
          Serial.print("Switch to mode : ");
          Serial.println(mode);
        }
        else if (midi_cmd == 0xC0) {
          Serial.print("Switch to command : ");
          Serial.println(midi_data);
          if ( mode == 0) {
            Serial.println("Execyuting mode 0");
            switch (midi_data) {
              case 0:              change_hardware_preset(0);Serial.println("change_hardware_preset 1");break;                 
              case 1:              change_hardware_preset(1);Serial.println("change_hardware_preset 2");break;                  
              case 2:              change_hardware_preset(2);Serial.println("change_hardware_preset 3");break;                
              case 3:              change_hardware_preset(3);Serial.println("change_hardware_preset 4");break;                  
            }
          }
          if ( mode == 1) {
            switch (midi_data) {
              case 0:              change_drive_toggle();Serial.println("change_drive_toggle 1");break;                       
              case 1:              change_mod_toggle();Serial.println("change_mod_toggle 2");break;                          
              case 2:              change_delay_toggle();Serial.println("change_delay_toggle 3");break;                     
              case 3:              change_reverb_toggle();Serial.println("change_reverb_toggle 4");break;     
            }               
          }
          if ( mode == 2) {
            
          }
          if ( mode == 3) {
            
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
            ESP.restart();
        }
    } else {
        Serial.println("Failed to get MIDI service!");
    }
}

void setup() {
    //setCpuFrequencyMhz(80);
    Serial.begin(115200);
    Serial.println("Starting setup...");
    mode = 0;

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

