#pragma once

#include <Arduino.h>

class Config {
public:
    void load();

    bool isConfigured() const { return _apiKey.length() > 0; }

    const String& apiKey() const { return _apiKey; }
    const String& device() const { return _device; }
    const String& sku() const { return _sku; }

    void setApiKey(const String& v);
    void setDevice(const String& v);
    void setSku(const String& v);

    void printMasked() const;

private:
    String _apiKey;
    String _device;
    String _sku;
};
