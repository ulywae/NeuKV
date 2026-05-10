/**
 * @file NeuKV.h
 * @author Ulywae (@neufa)
 * @brief Simple and powerful NVS wrapper for ESP32 without String overhead.
 * @version 1.0.0
 *
 * Features:
 * - Simple and easy to use.
 * - Fast and efficient NVS operations.
 * - No dynamic memory allocation.
 * - No String overhead.
 * - Easy to use templates.
 */

#ifndef NEU_KEY_VALUE_H
#define NEU_KEY_VALUE_H

#include <type_traits>
#include "nvs_flash.h"
#include "nvs.h"

namespace Neu
{

    /**
     * @brief Main class for handling NVS operations.
     *
     * Designed to be lightweight and fast by using templates and avoiding dynamic memory.
     */
    class KV
    {
    private:
        nvs_handle_t _handle; ///< NVS partition handle
        const char *_ns;      ///< Namespace name
        bool _is_open;        ///< Status flag for open/close state

        /**
         * @brief Initialize the NVS flash hardware.
         * @internal This is called automatically by begin(). It handles flash
         * initialization, erasing, and retrying if necessary.
         * @return esp_err_t ESP_OK if successful, otherwise error code.
         */
        static esp_err_t init();

    public:
        /**
         * @brief Construct a new KV object.
         * @param ns The NVS namespace name (max 15 characters).
         */
        KV(const char *ns);

        /**
         * @brief Open the storage namespace and initialize flash if needed.
         * @return true if successful, false otherwise.
         */
        bool begin();

        /**
         * @brief Check if a specific key exists in the storage.
         * @param key The key to look for.
         * @return true if found, false otherwise.
         */
        bool isExists(const char *key);

        /**
         * @brief Store any trivially copyable data (struct, int, float, etc).
         * @tparam T The data type.
         * @param key Storage key name.
         * @param value The value to be stored.
         * @return true if successful.
         */
        template <typename T>
        bool put(const char *key, const T &value)
        {
            static_assert(std::is_trivially_copyable<T>::value, "Data type must be trivially copyable!");
            if (!_is_open)
                return false;
            return (nvs_set_blob(_handle, key, &value, sizeof(T)) == ESP_OK);
        }

        /**
         * @brief Retrieve data from storage.
         * If the key is not found, the variable remains unchanged.
         * @tparam T The data type.
         * @param key Storage key name.
         * @param value Reference to the variable where data will be stored.
         * @return true if successful, false if key not found or error.
         */
        template <typename T>
        bool get(const char *key, T &value)
        {
            static_assert(std::is_trivially_copyable<T>::value, "Data type must be trivially copyable!");
            if (!_is_open)
                return false;

            size_t size = sizeof(T);
            return (nvs_get_blob(_handle, key, &value, &size) == ESP_OK);
        }

        /**
         * @brief Retrieve data from storage with a default value.
         * If the key is not found, the value is set to defaultValue.
         * @tparam T The data type.
         * @param key Storage key name.
         * @param value Reference to the variable where data will be stored.
         * @param defaultValue Value to return if the key is not found.
         */
        template <typename T>
        void get(const char *key, T &value, const T &defaultValue)
        {
            if (!get(key, value))
                value = defaultValue;
        }

        /** @name Basic Helpers
         * Simple shortcuts for basic data types.
         */
        ///@{

        /**
         * @brief Store a 32-bit integer.
         * @param key Storage key name.
         * @param val The integer value to store.
         * @return true if successful.
         */
        bool putInt(const char *key, int32_t val) { return put(key, val); }

        /**
         * @brief Get a 32-bit integer.
         * @param key Storage key name.
         * @param def Default value if key is not found.
         * @return The stored value or default.
         */
        int32_t getInt(const char *key, int32_t def = 0)
        {
            int32_t v;
            get(key, v, def);
            return v;
        }

        /**
         * @brief Store a float value.
         * @param key Storage key name.
         * @param val The float value to store.
         * @return true if successful.
         */
        bool putFloat(const char *key, float val) { return put(key, val); }

        /**
         * @brief Get a float value.
         * @param key Storage key name.
         * @param def Default value if key is not found.
         * @return The stored value or default.
         */
        float getFloat(const char *key, float def = 0.0f)
        {
            float v;
            get(key, v, def);
            return v;
        }

        /**
         * @brief Store a boolean value.
         * @param key Storage key name.
         * @param val The boolean value to store.
         * @return true if successful.
         */
        bool putBool(const char *key, bool val) { return put(key, val); }

        /**
         * @brief Get a boolean value.
         * @param key Storage key name.
         * @param def Default value if key is not found.
         * @return The stored value or default.
         */
        bool getBool(const char *key, bool def = false)
        {
            bool v;
            get(key, v, def);
            return v;
        }

        /**
         * @brief Packs multiple booleans into a single numeric key to save NVS write cycles.
         * @tparam T The storage type (uint8_t for 8 flags, uint16_t for 16, uint32_t for 32).
         * @param key Storage key name.
         * @param flags List of booleans in braces, e.g., {true, false, true}.
         * @return true if the packed byte was successfully stored.
         */
        template <typename T = uint8_t>
        bool putFlags(const char *key, std::initializer_list<bool> flags)
        {
            T packed = 0;
            uint8_t i = 0;
            for (bool f : flags)
            {
                if (f)
                    packed |= ((T)1 << i);
                if (++i >= (sizeof(T) * 8))
                    break;
            }
            return put(key, packed);
        }

        /**
         * @brief Unpacks a stored numeric key back into multiple boolean variables.
         * @tparam T The storage type used during putFlags.
         * @param key Storage key name.
         * @param flagsArray Pointer to a boolean array to store the results.
         * @param count Number of flags to extract.
         */
        template <typename T = uint8_t>
        void getFlags(const char *key, bool *flagsArray, uint8_t count)
        {
            T packed;
            if (get(key, packed))
            {
                for (uint8_t i = 0; i < count; i++)
                {
                    if (i < (sizeof(T) * 8))
                        flagsArray[i] = (packed >> i) & 1;
                }
            }
        }

        /**
         * @brief Store raw binary data (arrays, buffers, etc).
         * @param key Storage key name.
         * @param data Pointer to the data buffer.
         * @param len Length of the data in bytes.
         * @return true if successful.
         */
        bool putRaw(const char *key, const void *data, size_t len);

        /**
         * @brief Retrieve raw binary data into a provided buffer.
         * @param key Storage key name.
         * @param data Pointer to the destination buffer.
         * @param len Size of the destination buffer in bytes.
         * @return true if successful.
         */
        bool getRaw(const char *key, void *data, size_t len);

        ///@}

        /**
         * @brief Remove a specific key from the current namespace.
         * @param key Key name to remove.
         * @return true if successful.
         */
        bool remove(const char *key);

        /**
         * @brief Erase all keys in the current namespace.
         * @return true if successful.
         */
        bool clear();

        /**
         * @brief Completely erase the entire NVS partition (all namespaces).
         * @warning DANGER: This will wipe EVERYTHING in NVS, including other namespaces
         *          and system data (like WiFi credentials stored by ESP-IDF).
         * @return true if successful.
         */
        static bool format();

        /**
         * @brief Commit changes and close the current namespace.
         */
        void end();
    };

} // namespace Neu

/**
 * @brief Global instance of Neu::KV with default namespace "neu".
 */
extern Neu::KV NeuKV;

#endif
