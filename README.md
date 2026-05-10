# NeuKV

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![Platform: Arduino](https://img.shields.io/badge/Platform-Arduino-00878F?logo=arduino&logoColor=white)](https://arduino.cc)
[![Platform: ESP32](https://img.shields.io/badge/Platform-ESP32-blue?logo=espressif&logoColor=white)](https://espressif.com)

**NeuKV** is a high-performance, lightweight NVS (Non-Volatile Storage) wrapper for ESP32. It is designed to be simple for beginners while providing advanced power for professionals by utilizing C++ templates and avoiding the overhead of the `String` class.

---

## Features

- **No String Overhead**: Completely avoids `String` class to prevent memory fragmentation.
- **Zero Dynamic Allocation**: Uses binary blobs for maximum speed and stability.
- **Type Safety**: Includes `static_assert` to prevent saving incompatible data types (like pointers).
- **Multi-Namespace Support**: Easily isolate different types of data (e.g., WiFi, Sensors, System) by creating multiple instances.
- **Raw Data Support**: Save and load raw buffers, arrays, or binary data directly.
- **Flash Saver (Flag Packing)**: Pack up to 32 booleans into a single numeric key to reduce NVS write cycles and extend Flash life.
- **Powerfull Templates**: Save and load entire `structs` or arrays in a single line.
- **Beginner Friendly**: Provides simple `putInt`, `putFloat`, and `putBool` shortcuts.
- **Auto-Initialization**: Automatically handles NVS flash initialization and error recovery.

---

## Installation

### Arduino Library Manager (Recommended)

1. In the Arduino IDE, go to **Sketch > Include Library > Manage Libraries...**
2. Search for **NeuKV**
3. Click **Install**

### PlatformIO

```ini
lib_deps = ulywae/NeuKV
```

### Manual Installation

1. Download the repository as `.zip`
2. Open Arduino IDE
3. Go to **Sketch > Include Library > Add .ZIP Library**
4. Select the downloaded file

---

## Quick Start

### Basic Usage (Safe Initialization)

Always check if a key exists before overwriting it with default values to save flash life.

```cpp
#include <NeuKV.h>

void setup() {
    NeuKV.begin();

    // Save data
    NeuKV.putInt("boot_count", 10);

    // Get data with default value fallback
    int boots = NeuKV.getInt("boot_count", 0);

    NeuKV.end(); // Always call end() to commit changes
}
```

---

### Storing Raw Arrays/Buffers

Use `putRaw` and `getRaw` for arrays or raw binary buffers.

```cpp
int myHistory[5] = {10, 20, 30, 40, 50};

// Save array
NeuKV.putRaw("history", myHistory, sizeof(myHistory));

// Load array
int restoredHistory[5];
NeuKV.getRaw("history", restoredHistory, sizeof(restoredHistory));
```

---

### Flash Saver (Storing Multiple Booleans)

Stop wasting Flash write cycles! Pack multiple booleans into a single 1-byte or 2-byte storage. Perfect for system states or user preferences.

```cpp
// 1. [BULK] Pack 4 booleans into 1 byte (Saves Flash Life!)
NeuKV.putFlags("settings", {true, false, true, true});

// 2. [SINGLE] Update just one specific bit (Read-Modify-Write)
NeuKV.putFlag("settings", 1, true); // Changes bit at index 1 to true

// 3. [SINGLE] Retrieve only one specific bit
bool isMqttActive = NeuKV.getFlag("settings", 2);

// 4. [BULK] Retrieve all back into an array
bool myFlags[4];
NeuKV.getFlags("settings", myFlags, 4);
```

> [!IMPORTANT]
>
> **Why use Flag Packing?**
> Every write to Flash memory has a cost. Storing 8 booleans as 1 packed byte is **8x more efficient** than storing them separately. **NeuKV** helps you extend your ESP32's life by reducing unnecessary write cycles.

> [!TIP]
>
> **Flash Longevity Tip**: Use `putFlags()` (Bulk) whenever you need to store multiple status booleans at once.
> It performs only ONE write operation to the hardware.

---

### Multi-Namespace

UsageYou can create multiple instances to separate your data into different namespaces (e.g., one for system settings and one for user profiles). This keeps your data organized and safe during `clear()` operations.

```cpp
#include <NeuKV.h>

// Create custom instances for different namespaces
Neu::KV SystemStore("system");
Neu::KV UserStore("user");

void setup() {
  // Use the global instance (namespace: "neu")
  NeuKV.begin();
  NeuKV.putInt("boot_count", 1);
  NeuKV.end();

  // Use your custom "system" namespace
  if (SystemStore.begin()) {
    SystemStore.putFloat("version", 1.0f);
    SystemStore.end();
  }

  // Use your custom "user" namespace
  if (UserStore.begin()) {
    UserStore.putBool("dark_mode", true);
    UserStore.end();
  }
}
```

---

### Advanced Usage (Storing Structs)

```cpp
struct Config {
    float threshold;
    bool alarmEnabled;
};

void saveConfig() {
    NeuKV.begin();

    Config myCfg = {25.5f, true};
    NeuKV.put("sys_cfg", myCfg); // Saves the whole struct

    NeuKV.end();
}
```

---

## API Reference

| Method                     | Description                                                                 |
| :------------------------- | :-------------------------------------------------------------------------- |
| `begin()`                  | Opens the default namespace and initializes flash hardware.                 |
| `isExists(key)`            | Checks if a specific key exists in the current namespace.                   |
| `put<T>(key, val)`         | Stores any trivially copyable data (struct, int, float, etc).               |
| `get<T>(key, var)`         | Retrieves data. Returns `true` if found, variable remains unchanged if not. |
| `get<T>(key, var, def)`    | Retrieves data with an automatic default value fallback.                    |
| `putInt(key, val)`         | Shortcut to store a 32-bit integer.                                         |
| `putRaw(key, ptr, len)`    | Stores raw binary data/arrays.                                              |
| `getRaw(key, ptr, len)`    | Retrieves raw binary data into a buffer.                                    |
| `getInt(key, def)`         | Shortcut to get an integer with a default value.                            |
| `putFloat(key, val)`       | Shortcut to store a float.                                                  |
| `getFloat(key, def)`       | Shortcut to get a float with a default value.                               |
| `putBool(key, val)`        | Shortcut to store a boolean.                                                |
| `getBool(key, def)`        | Shortcut to get a boolean with a default value.                             |
| `putFlags<T>(key, {..})`   | [BULK] Packs multiple bools into one numeric key (saves Flash life).        |
| `getFlags<T>(key, arr, n)` | [BULK] Unpacks a numeric key back into a boolean array.                     |
| `putFlag<T>(key, idx, v)`  | [SINGLE] Updates one specific bit (Read-Modify-Write).                      |
| `getFlag<T>(key, idx, d)`  | [SINGLE] Retrieves one specific bit from a packed key.                      |
| `remove(key)`              | Deletes a specific key-value pair.                                          |
| `clear()`                  | Erases all keys within the current namespace.                               |
| `format()`                 | [DANGER] Wipes the entire NVS memory (all namespaces).                      |
| `end()`                    | Commits pending changes and closes the NVS handle.                          |

---

## Limitations

To keep the library "pure" and high-performance, please note the following:

1. **No Pointers in Structs**: You cannot store structs that contain pointers (e.g., `char*`, `int*`) or dynamic containers (e.g., `std::vector`, `std::string`). Only "Trivially Copyable" data is allowed.
2. **Key/Namespace Length**: Limited to a maximum of **15 characters**.
3. **No Strings**: This library intentionally does not support the Arduino `String` class to ensure memory stability. Use fixed-size char arrays (`char[32]`) inside your structs instead.
4. **Flash Wear**: Like all NVS operations, frequent writing in a tight loop can wear out your ESP32's flash memory. Always manage your write cycles responsibly.

---

## Notes

This library is primarily a personal toolkit. It was built to solve specific problems with simplicity and performance in mind.

- If it fits your use case — great.
- If not — that’s fine too.

It does exactly what it was designed to do, without the fluff.

---

## Author

Created by **Ulywae (@neufa)**
Part of the **NEU Ecosystem**

---

## License

This library is licensed under the MIT License.

---
