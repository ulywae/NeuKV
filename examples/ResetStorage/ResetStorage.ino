/**
 * @file ResetStorage.ino
 * @author Ulywae (@neufa)
 * @brief Maintenance example: How to remove specific keys, wipe namespaces, or format flash.
 *
 * USE WITH CAUTION: This example demonstrates data deletion.
 */

#include <Arduino.h>
#include <NeuKV.h>

void setup()
{
    Serial.begin(115200);
    delay(1000);
    Serial.println("\n--- NeuKV Maintenance Mode ---");

    // --- SCENARIO 1: REMOVE A SPECIFIC KEY ---
    if (NeuKV.begin())
    {
        const char *targetKey = "old_setting";
        if (NeuKV.isExists(targetKey))
        {
            if (NeuKV.remove(targetKey))
            {
                Serial.printf("Success: Key '%s' has been removed.\n", targetKey);
            }
        }
        else
        {
            Serial.printf("Notice: Key '%s' does not exist.\n", targetKey);
        }
        NeuKV.end();
    }

    // --- SCENARIO 2: CLEAR CURRENT NAMESPACE ---
    // This only wipes the "neu" namespace.
    if (NeuKV.begin())
    {
        Serial.println("Clearing current namespace...");
        if (NeuKV.clear())
        {
            Serial.println("SUCCESS: Namespace 'neu' is now empty.");
        }
        NeuKV.end();
    }

    // --- SCENARIO 3: FULL NVS FORMAT (DANGER) ---
    // This wipes EVERY namespace in the NVS partition.
    // Note: format() is a static method, no need to call begin() first.
    Serial.println("\n[WARNING] Performing FULL NVS FORMAT in 5 seconds...");
    Serial.println("This will erase ALL namespaces and system data!");
    delay(5000);

    if (Neu::KV::format())
    {
        Serial.println("HARD RESET SUCCESS: Entire NVS partition has been formatted.");
    }
    else
    {
        Serial.println("ERROR: Failed to format NVS.");
    }

    Serial.println("\nMaintenance complete.");
}

void loop()
{
    // Nothing to do
}
