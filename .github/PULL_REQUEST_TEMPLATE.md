## Description

Provide a summary of the changes made and the motivation behind them.

Fixes #(issue number)

## Type of Change
- [ ] Bug fix (non-breaking change which fixes an issue)
- [ ] New feature (non-breaking change which adds functionality)
- [ ] Breaking change (fix or feature that would cause existing hardware/firmware setup to not work as expected)
- [ ] Documentation update
- [ ] Hardware / Schematic revision

## Embedded Hardware Checklist
- [ ] All GPIO pin allocations documented in `firmware/config/PinDefinitions.h`
- [ ] All configurable constants defined in `firmware/config/Config.h`
- [ ] No blocking `delay()` calls introduced in execution loop
- [ ] Verified non-volatile storage (NVS) backward compatibility
- [ ] Fail-safe state verified (pump remains safely OFF on failure)

## Verification & Testing
- [ ] Compiled cleanly using PlatformIO CLI (`pio run`)
- [ ] Tested on physical ESP32 hardware
- [ ] Serial monitor logs checked at 115200 baud
- [ ] Wi-Fi disconnection / reconnection verified
