/**
 * @file NeuKV.cpp
 * @author Ulywae (@neufa)
 * @brief Implementation of the NeuKV NVS wrapper.
 */

#include "NeuKV.h"

namespace Neu
{

    /**
     * @brief Constructor stores the namespace name for later use in begin().
     */
    KV::KV(const char *ns) : _ns(ns), _is_open(false) {}

    /**
     * @brief Handles low-level flash initialization and error recovery.
     * This is idempotent and will only run once.
     */
    esp_err_t KV::init()
    {
        static bool is_init = false;
        if (is_init)
            return ESP_OK;

        esp_err_t ret = nvs_flash_init();
        if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
        {
            // Erase partition and retry if flash is full or format is incompatible
            nvs_flash_erase();
            ret = nvs_flash_init();
        }

        if (ret == ESP_OK)
            is_init = true;
        return ret;
    }

    /**
     * @brief Ensures NVS hardware is ready and opens the specific namespace.
     */
    bool KV::begin()
    {
        if (init() != ESP_OK)
            return false;
        return (_is_open = (nvs_open(_ns, NVS_READWRITE, &_handle) == ESP_OK));
    }

    /**
     * @brief Checks key existence by attempting to get blob size.
     */
    bool KV::isExists(const char *key)
    {
        if (!_is_open)
            return false;
        size_t s;
        return (nvs_get_blob(_handle, key, NULL, &s) != ESP_ERR_NVS_NOT_FOUND);
    }

    /**
     * @brief Store raw binary data (buffers, arrays, etc).
     */
    bool KV::putRaw(const char *key, const void *data, size_t len)
    {
        if (!_is_open)
            return false;
        return (nvs_set_blob(_handle, key, data, len) == ESP_OK);
    }

    /**
     * @brief Retrieve raw binary data into a provided buffer.
     */
    bool KV::getRaw(const char *key, void *data, size_t len)
    {
        if (!_is_open)
            return false;
        size_t actual_size = len;
        return (nvs_get_blob(_handle, key, data, &actual_size) == ESP_OK);
    }

    /**
     * @brief Removes a single key-value pair.
     */
    bool KV::remove(const char *key)
    {
        if (!_is_open)
            return false;
        return (nvs_erase_key(_handle, key) == ESP_OK);
    }

    /**
     * @brief Formats the entire NVS partition.
     * @warning This is a destructive operation. Use with caution for Factory Reset.
     */
    bool KV::format()
    {
        if (init() != ESP_OK)
            return false;
        // Erase the default NVS partition
        esp_err_t ret = nvs_flash_erase();
        if (ret != ESP_OK)
            return false;

        // Re-initialize after erase
        return (nvs_flash_init() == ESP_OK);
    }

    /**
     * @brief Erases the entire namespace.
     */
    bool KV::clear()
    {
        if (!_is_open)
            return false;
        return (nvs_erase_all(_handle) == ESP_OK);
    }

    /**
     * @brief Commits pending changes to flash and releases the NVS handle.
     */
    void KV::end()
    {
        if (_is_open)
        {
            nvs_commit(_handle);
            nvs_close(_handle);
            _is_open = false;
        }
    }

} // namespace Neu

/**
 * @brief Pre-instantiated global object for the default 'neu' namespace.
 */
Neu::KV NeuKV("neu");
