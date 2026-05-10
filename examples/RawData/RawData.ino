/**
 * @file RawData.ino
 * @author Ulywae (@neufa)
 * @brief Example: How to store and retrieve raw arrays/buffers using NeuKV.
 */

#include <Arduino.h>
#include <NeuKV.h>

void setup()
{
    Serial.begin(115200);
    delay(1000);
    Serial.println("\n--- NeuKV Raw Data Example ---");

    // Sample array to store (e.g., last 5 sensor readings)
    int sensorHistory[5] = {102, 405, 301, 880, 550};

    if (NeuKV.begin())
    {
        // --- STEP 1: STORE RAW ARRAY (putRaw) ---
        Serial.println("Saving sensor history array...");
        if (NeuKV.putRaw("history", sensorHistory, sizeof(sensorHistory)))
        {
            Serial.println("Array saved successfully!");
        }

        // --- STEP 2: RETRIEVE RAW ARRAY (getRaw) ---
        // Prepare a buffer/variable with the same size to hold the data
        int restoredHistory[5];

        if (NeuKV.getRaw("history", restoredHistory, sizeof(restoredHistory)))
        {
            Serial.println("Array retrieved successfully. Values:");
            for (int i = 0; i < 5; i++)
            {
                Serial.printf(" Index %d: %d\n", i, restoredHistory[i]);
            }
        }
        else
        {
            Serial.println("Failed to retrieve array (Key not found).");
        }

        // Always call end() to finalize and save changes to Flash memory
        NeuKV.end();
    }
    else
    {
        Serial.println("Failed to open storage!");
    }
}

void loop()
{
    // Nothing to do
}
