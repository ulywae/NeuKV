/**
 * @file FlashSaver.ino
 * @author Ulywae (@neufa)
 * @brief Example: How to pack multiple booleans into a single write to save Flash life.
 */

#include <Arduino.h>
#include <NeuKV.h>

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("\n--- NeuKV Flash Saver Example ---");

  // Some virtual device states
  bool isWiFiConnected = true;
  bool isMqttActive = false;
  bool isMotorRunning = true;
  bool hasError = false;

  if (NeuKV.begin()) {
    // --- STEP 1: PACK & STORE (putFlags) ---
    // We pack 4 booleans into 1 byte (uint8_t is default)
    // This only performs ONE write operation to the NVS Flash.
    Serial.println("Packing and saving system states...");
    if (NeuKV.putFlags("sys_state", {isWiFiConnected, isMqttActive, isMotorRunning, hasError})) {
      Serial.println("Success: 4 states stored as a single byte!");
    }

    // --- STEP 2: RETRIEVE & UNPACK (getFlags) ---
    bool states[4]; // Prepare an array to hold the results
    
    NeuKV.getFlags("sys_state", states, 4);

    Serial.println("\nRestored States:");
    Serial.printf(" WiFi Connected: %s\n", states[0] ? "YES" : "NO");
    Serial.printf(" MQTT Active:    %s\n", states[1] ? "YES" : "NO");
    Serial.printf(" Motor Running: %s\n", states[2] ? "YES" : "NO");
    Serial.printf(" System Error:  %s\n", states[3] ? "YES" : "NO");

    NeuKV.end();
  } else {
    Serial.println("Failed to open storage!");
  }
}

void loop() {
  // Nothing to do
}
