# Sensor Calibration & Signal Conditioning Guide

This document details the mathematical models, laboratory procedures, and firmware Non-Volatile Storage (NVS) persistence techniques used to calibrate transducers on **AgriFlow-EdgeAI**.

---

## 1. ESP32 SAR ADC Non-Linearity Compensation

The ESP32 integrated 12-bit SAR ADC possesses known non-linear characteristics:
- **Deadband Zone Near 0V**: Voltages below ~100 mV read as 0.
- **Saturation Zone Near 3.3V**: Voltages above ~3.15V saturate at 4095.
- **Intermediate Curvature**: Mild sigmoid non-linearity between 0.5V and 2.5V.

Because the Capacitive Soil Moisture Sensor v1.2 outputs voltages between **1.2V and 3.0V**, its signal swing falls entirely within the reliable linear range of the ESP32 ADC when configured with **11 dB attenuation** (`analogSetAttenuation(ADC_11db)`).

---

## 2. Two-Point Soil Moisture Calibration Procedure

Soil moisture is quantified as **Volumetric Water Content (VWC)** percentage ($0.0\%$ for bone-dry soil to $100.0\%$ for full saturation).

### Calibration Protocol
1. **Dry Air Calibration Point ($ADC_{\text{dry}}$)**:
   - Suspend the clean, dry capacitive probe in open air at ambient room temperature.
   - Observe the raw ADC reading over 30 seconds.
   - Record the averaged reading (typically **$3100 \le ADC_{\text{dry}} \le 3400$**).
2. **Saturated Water Calibration Point ($ADC_{\text{wet}}$)**:
   - Submerge the probe blade into a glass of tap water up to the maximum immersion line (do not submerge the electronics or cable junction).
   - Record the averaged reading (typically **$1400 \le ADC_{\text{wet}} \le 1650$**).

### Volumetric Water Content Equation
Because capacitive output voltage is inversely proportional to soil water content, the percentage mapping is:

$$VWC(\%) = \text{constrain}\left( \frac{ADC_{\text{dry}} - ADC_{\text{raw}}}{ADC_{\text{dry}} - ADC_{\text{wet}}} \times 100.0, \; 0.0, \; 100.0 \right)$$

---

## 3. Water Reservoir Level Calibration

The immersion level sensor utilizes an open conductive ladder. Its raw voltage output scales linearly with submersion depth:

$$Level(\%) = \text{constrain}\left( \frac{ADC_{\text{raw}} - ADC_{\text{empty}}}{ADC_{\text{full}} - ADC_{\text{empty}}} \times 100.0, \; 0.0, \; 100.0 \right)$$

- **$ADC_{\text{empty}}$ (Dry sensor in air)**: $\approx 150 – 250$
- **$ADC_{\text{full}}$ (Immersed to top 40mm mark)**: $\approx 2600 – 2900$

---

## 4. Non-Volatile Storage (NVS) Persistence

Calibration parameters are saved to the ESP32 on-chip Flash NVS partition using the `Preferences.h` library. This guarantees that field calibration values are retained permanently across reboots, power cuts, or firmware updates without modifying source code:

```cpp
// Example NVS write executed during field commissioning
Calibration cal;
cal.begin();
cal.setSoilDryAdc(3250);  // Stored in 'agri_cal' namespace in Flash NVS
cal.setSoilWetAdc(1480);
```

---

## 5. Soil Texture Adjustment Factors

Different soil structures hold water at varying tensions (capillary vs gravitational water). The hysteresis thresholds in [`Config.h`](file:///Users/kuldeep/Documents/GitHub/AgriFlow-EdgeAI/firmware/config/Config.h) can be tuned according to crop and soil taxonomy:

| Soil Texture | Field Capacity VWC (%) | Permanent Wilting Point VWC (%) | Recommended `SOIL_DRY` | Recommended `SOIL_WET` |
| :--- | :---: | :---: | :---: | :---: |
| **Sandy Loam** | 18% – 24% | 6% – 10% | **22%** | **45%** |
| **Loam (Ideal Garden)** | 28% – 36% | 12% – 16% | **35%** | **65%** *(Default)* |
| **Clay / Silty Clay** | 38% – 46% | 20% – 24% | **42%** | **75%** |
| **Peat / Coco Coir** | 50% – 65% | 24% – 30% | **48%** | **80%** |
