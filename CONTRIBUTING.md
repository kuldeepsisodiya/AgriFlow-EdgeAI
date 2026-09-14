# Contributing to AgriFlow-EdgeAI

Thank you for your interest in contributing to **AgriFlow-EdgeAI**! This project was engineered for the Smart India Hackathon (SIH) and Academic Engineering Minor Projects, adhering to rigorous software engineering and embedded development standards.

We welcome contributions from students, researchers, agronomists, and embedded software engineers.

---

## Code of Conduct

All contributors and maintainers are expected to abide by our [Code of Conduct](file:///Users/kuldeep/Documents/GitHub/AgriFlow-EdgeAI/CODE_OF_CONDUCT.md). Please report any unacceptable behavior to the project maintainers.

---

## How to Contribute

### 1. Reporting Bugs
- Check the [GitHub Issues](https://github.com/kuldeepsisodiya/AgriFlow-EdgeAI/issues) to ensure the issue hasn't already been reported.
- Use the **Bug Report** template.
- Include hardware revisions (e.g., ESP32 DevKit V1 vs ESP32-S3), power source details, sensor versions (capacitive v1.2 vs resistive), and full serial monitor logs at `115200` baud.

### 2. Suggesting Enhancements
- Check existing issues and feature roadmaps in [`docs/future-work.md`](file:///Users/kuldeep/Documents/GitHub/AgriFlow-EdgeAI/docs/future-work.md).
- Open an issue using the **Feature Request** template explaining the proposed improvement, agricultural benefits, and resource impact on the ESP32 (RAM/Flash).

### 3. Submitting Pull Requests
1. Fork the repository.
2. Create a topic branch:
   ```bash
   git checkout -b feature/tinyml-vpd-predictor
   ```
3. Ensure your firmware follows the **Modular C++ Embedded Standards**:
   - **No blocking `delay()` calls** in the main execution loop; utilize `millis()` cooperative task scheduling.
   - Separate concerns into designated manager classes (`SensorManager`, `PumpController`, `DecisionEngine`, etc.).
   - Document any changed GPIO pin assignments in `firmware/config/PinDefinitions.h` with electrical justifications.
   - Keep configuration parameters in `firmware/config/Config.h`.
4. Run static validation and ensure the firmware builds cleanly with PlatformIO:
   ```bash
   pio run
   ```
5. Commit with clear semantic commit messages:
   - `feat: add moving average filter for capacitive soil moisture`
   - `fix: prevent inductive relay coil brownout with software delay stagger`
   - `docs: update calibration polynomial equations in calibration.md`
6. Push to your branch and open a Pull Request targeting `main`.

---

## Embedded Coding Standards

- **Language Standard**: C++17 / Arduino Core for ESP32.
- **Header Guards**: Use `#ifndef CLASS_NAME_H` `#define CLASS_NAME_H` `#endif`.
- **Memory Management**: Avoid dynamic heap allocations (`malloc`, `new`, large dynamic `String`) inside recurrent loop routines to prevent heap fragmentation. Prefer stack allocation, statically allocated buffers, and fixed-size arrays.
- **Fail-Safe Operation**: All actuation logic MUST fail closed (pump powered OFF) in the event of sensor disconnection, out-of-range ADC readings, or watchdog timeouts.

