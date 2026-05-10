/**
 * @file FlashSaver.ino
 * @author Ulywae (@neufa)
 * @brief Example: Efficiently manage multiple booleans (flags) in a single NVS key.
 * 
 * This example demonstrates:
 * 1. Bulk operations (putFlags/getFlags) - Most efficient for Flash longevity.
 * 2. Single bit operations (putFlag/getFlag) - Flexible for granular updates.
 */

#include <Arduino.h>
#include <NeuKV.h>

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("\n--- NeuKV Flash Saver Engine ---");

  if (NeuKV.begin()) {
    // =========================================================================
    // SCENARIO 1: BULK OPERATIONS (The "Nasgor" Efficiency)
    // Use this to store all your settings at once with only ONE write cycle.
    // =========================================================================
    
    Serial.println("\n[BULK] Saving 4 states at once...");
    // We pack 4 bools into 1 byte (uint8_t is default)
    NeuKV.putFlags("sys_cfg", {true, false, true, true}); 

    // Retrieve all back into an array
    bool myStates[4];
    NeuKV.getFlags("sys_cfg", myStates, 4);
    
    Serial.print("Restored: ");
    for(int i=0; i<4; i++) Serial.printf("%d ", myStates[i]);
    Serial.println();


    // =========================================================================
    // SCENARIO 2: SINGLE BIT OPERATIONS (Granular Control)
    // Use this to update or check only ONE specific flag without affecting others.
    // =========================================================================
    
    // Update only bit at index 1 to 'true' (was 'false' in Scenario 1)
    Serial.println("\n[SINGLE] Updating bit at index 1...");
    NeuKV.putFlag("sys_cfg", 1, true);

    // Read only bit at index 1
    bool isFeatureEnabled = NeuKV.getFlag("sys_cfg", 1);
    Serial.printf("Bit 1 state: %s\n", isFeatureEnabled ? "TRUE" : "FALSE");


    // =========================================================================
    // SCENARIO 3: USING LARGER TYPES (Up to 16 or 32 flags)
    // = : Use <uint16_t> or <uint32_t>
    // =========================================================================
    
    Serial.println("\n[ADVANCED] Using uint16_t for more flags...");
    NeuKV.putFlags<uint16_t>("ext_cfg", {true, false, true, false, true, true, false, true, true});
    
    bool bit8 = NeuKV.getFlag<uint16_t>("ext_cfg", 8);
    Serial.printf("Bit 8 of extended config: %d\n", bit8);

    NeuKV.end();
    Serial.println("\nAll tests complete!");
  }
}

void loop() {}
