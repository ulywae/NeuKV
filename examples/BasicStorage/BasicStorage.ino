/**
 * @file BasicStorage.ino
 * @author Ulywae (@neufa)
 * @brief Example usage of NeuKV library for ESP32.
 *
 * This example demonstrates basic types, custom structs,
 * and the difference between getting data with or without defaults.
 */

#include <Arduino.h>
#include <NeuKV.h>

// Define a custom struct for advanced storage
// Must be trivially copyable (no Strings or pointers)
struct DeviceConfig
{
    int id;
    float threshold;
    bool status;
};

void setup()
{
    Serial.begin(115200);
    delay(1000);
    Serial.println("\n--- NeuKV Example ---");

    // 1. Initialize and open storage
    // NeuKV is a global instance using the "neu" namespace
    if (NeuKV.begin())
    {
        Serial.println("Storage opened successfully!");

        // --- PART 1: Basic Types (Beginner Friendly Shortcuts) ---

        // Update boot count: read current, add 1, then save
        int currentBoots = NeuKV.getInt("boot_count", 0);
        NeuKV.putInt("boot_count", currentBoots + 1);

        NeuKV.putFloat("temp_limit", 38.5f);
        NeuKV.putBool("led_state", true);

        // Read them back
        int boots = NeuKV.getInt("boot_count");
        float limit = NeuKV.getFloat("temp_limit");
        bool led = NeuKV.getBool("led_state");

        Serial.printf(">>> Basic Data -> Boots: %d, Temp Limit: %.1f, LED: %s\n",
                      boots, limit, led ? "ON" : "OFF");

        // --- PART 2: Custom Struct (Advanced Power) ---

        DeviceConfig myCfg = {101, 25.4f, true};

        // Save the whole struct as a binary blob
        NeuKV.put("dev_cfg", myCfg);

        // Read struct back using the "Default Value" method
        DeviceConfig savedCfg;
        NeuKV.get("dev_cfg", savedCfg, {0, 0.0f, false});

        Serial.printf(">>> Struct Data -> ID: %d, Threshold: %.2f, Status: %d\n",
                      savedCfg.id, savedCfg.threshold, savedCfg.status);

        // --- PART 3: Pure Get (Without Defaults) ---

        float pressure = 1013.25f; // Initial/Current value

        // Check if key exists before doing something
        if (NeuKV.isExists("pressure"))
        {
            // This will only change 'pressure' if the key is found in NVS
            NeuKV.get("pressure", pressure);
            Serial.printf(">>> Pressure loaded: %.2f\n", pressure);
        }
        else
        {
            Serial.println(">>> Pressure key not found, using initial value.");
        }

        // --- PART 4: Maintenance ---

        // NeuKV.remove("temp_limit"); // How to delete a specific key
        // NeuKV.clear();              // How to wipe the entire namespace

        // 2. Commit and Close
        // Mandatory to call end() to save changes to flash memory
        NeuKV.end();
        Serial.println("------------------------------------------");
        Serial.println("Storage closed and changes saved safely.");
    }
    else
    {
        Serial.println("Failed to open storage!");
    }
}

void loop()
{
    // Nothing here
}
