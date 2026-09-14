# Changelog

All notable changes to the **AgriFlow-EdgeAI** project are documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

---

## [1.0.0] - 2026-09-14

### Initial SIH & Minor Project Release

#### Architecture & Firmware
- **Modular Embedded Architecture**: Decomposed monolithic loop into 8 discrete C++ subsystem modules (`SensorManager`, `PumpController`, `DecisionEngine`, `BlynkManager`, `WiFiManager`, `Calibration`, `LEDManager`, `main`).
- **Cooperative Scheduling**: Implemented non-blocking `millis()` task dispatcher replacing blocking `delay()` calls.
- **Signal Processing**: Added 10-sample sliding window Moving Average Filter on ADC lines to suppress electrical noise.
- **Hysteresis Logic**: Added deadband control window (`SOIL_DRY_THRESHOLD_PERCENT` vs `SOIL_WET_THRESHOLD_PERCENT`) to eliminate relay chatter.
- **Hardware Protection**: Enforced low-water dry-run pump interlocking and software-defined max run duration timeouts (300 seconds).
- **Edge Resilience**: Added offline fallback mode with auto-reconnect backoff so local autonomous irrigation continues even when Wi-Fi/Cloud is unreachable.
- **NVS Calibration**: Built-in two-point linear calibration model mapping raw 12-bit ADC values to accurate soil volumetric water content (VWC).

#### Documentation
- Created IEEE-structured technical documentation suite in `docs/` (`project-overview`, `hardware`, `software`, `architecture`, `installation`, `calibration`, `troubleshooting`, `future-work`, `references`).
- Authored full Bill of Materials (`hardware/bom.csv` and `hardware/bom.md`) with cost, tolerance, and electrical justifications.
- Comprehensive hardware wiring and isolation safety schematics in `hardware/circuit-schematic.md`.

#### Visuals & Schematics
- Generated high-resolution, dark-mode SVG vector diagrams:
  - System Architecture (`diagrams/system-architecture.svg`)
  - Hardware Block Diagram (`diagrams/hardware-block-diagram.svg`)
  - Software Flowchart (`diagrams/software-flowchart.svg`)
  - Multi-Stage Decision Flow (`diagrams/decision-flow.svg`)
  - Component Sequence Diagram (`diagrams/sequence-diagram.svg`)
  - Finite State Machine (`diagrams/state-machine.svg`)
  - Physical Pin Mapping (`diagrams/pin-mapping.svg`)
  - End-to-End Data Flow Pipeline (`diagrams/data-flow.svg`)
- Included SIH presentation assets and Edge-AI / TinyML integration roadmap.
