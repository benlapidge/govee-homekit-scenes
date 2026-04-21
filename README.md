# Govee Floor Lamp → HomeKit Bridge

ESP32 firmware that exposes a Govee H6076 RGBICW Floor Lamp as a native HomeKit accessory using [HomeSpan](https://github.com/HomeSpan/HomeSpan). HomeKit characteristics (on/off, brightness, hue, saturation, colour temperature) are translated into Govee Developer API calls, plus one Switch per curated scene that behaves as a radio button.

Built because Govee's native Matter integration ships a poor colour picker and no scene support.

## Features

- HomeKit LightBulb with full colour wheel, brightness, and colour temperature
- Curated scenes exposed as switches (Cyberpunk, Sunset, Aurora, Warm Sun, Meditation, Sleep, Movie)
- Radio-button scenes: activating one clears the others; direct lamp control clears the active scene
- Debounced API dispatch (~300 ms) so dragging a slider produces a single Govee API call, not dozens
- Config (API key, device id, SKU) persisted in NVS — set once over serial, survives reboots

## Hardware

- Any ESP32 dev board (tested layout: `esp32dev`)
- Wi-Fi network the ESP32 and your Home Hub share

## Dependencies

PlatformIO pulls these automatically:

- [HomeSpan](https://registry.platformio.org/libraries/homespan/HomeSpan)
- [ArduinoJson](https://registry.platformio.org/libraries/bblanchon/ArduinoJson) v7

## Build & flash

```bash
pio run -t upload
pio device monitor -b 115200
```

## One-time setup

1. Get a Govee Developer API key from the Govee Home app: *Profile → Settings → Apply for API Key*. Wait for the email.
2. Open the serial monitor at 115200 baud.
3. Set the API key (the device id and SKU default to this project's hardcoded H6076):

   ```
   @K your-govee-api-key-here
   ```

4. (Optional) override device id or SKU:

   ```
   @D 1F:09:DE:B6:40:86:50:58
   @S H6076
   ```

5. Verify with `@P` — the API key is printed masked.
6. Provision Wi-Fi if not already done. HomeSpan boots an open AP `Govee-Bridge-Setup`; connect a phone, follow the captive portal. From the serial monitor you can also use HomeSpan's built-in `W` command.

## Pair to HomeKit

1. In Apple Home: *+ → Add Accessory → More options → Govee Floor Lamp*.
2. Enter the HomeSpan default setup code: `466-37-726`.
3. Eight tiles will appear: the lamp plus one switch per scene.

## Adding a scene

1. Open the Govee app, find the scene, and capture its `paramId` and `id` (you can sniff this from the `/router/api/v1/device/control` request the app makes, or pull from `/router/api/v1/device/scenes`).
2. Append one line to [`src/SceneRegistry.h`](src/SceneRegistry.h):

   ```cpp
   {"My New Scene", 12345, 6789},
   ```

3. Rebuild and flash. The new switch appears after re-pairing the accessory (Home app: tap the accessory tile → Settings → Remove Accessory, then re-add).

## Troubleshooting

- **HTTP 429 / rate limit messages in serial log:** Govee allows ~10 req/min/device. The debounce window already coalesces slider drags; if you're still hitting it, raise `Debouncer{300}` in `GoveeLight.h` to e.g. 500 ms.
- **HTTP 401:** API key wrong. Re-run `@K`.
- **HTTP 400 / "device not found":** check `@P` output; re-run `@D` and `@S`.
- **Lamp out of sync after using Govee app:** known v1 limitation. Toggle the lamp in HomeKit to resync.
- **Scene switch turns itself off:** expected — selecting a scene only fires the API call; turn the lamp off via the LightBulb tile.

## Out of scope (v1)

LAN UDP API, segmented colour, music/DIY/DreamView modes, multi-device, state polling. Extension points are marked `// TODO:` in the code.

## License

MIT.
