# PCB Design Specifications & Carrier Board Guidelines

This directory contains design specifications for moving the **AgriFlow-EdgeAI** system from breadboard prototyping to a dedicated double-sided FR4 Printed Circuit Board (PCB).

---

## Recommended PCB Stackup & Layer Specifications

- **Layers**: 2-Layer FR4 (1.6mm thickness)
- **Copper Weight**: 1.0 oz (35µm) standard; 2.0 oz recommended for high-current pump traces
- **Solder Mask**: Matte Black or Dark Green with White Silkscreen
- **Minimum Trace Width**:
  - Logic Signals: 10 mil (0.254 mm)
  - 3.3V / 5V Power Rails: 25 mil (0.635 mm)
  - 12V High-Current Pump Traces: 60 mil (1.524 mm)
- **Clearance**:
  - Low-voltage logic-to-logic: 10 mil (0.254 mm)
  - AC/DC Mains-to-Logic Creepage (under relay): $\ge 3.0\text{ mm}$ with physical milled isolation slot

---

## Design Guidelines & EMI Mitigation

1. **Physical Isolation Slot**:
   - Route a 1.5mm wide milled air gap (isolation slot) directly beneath the relay contacts to prevent high-voltage creepage across the PCB surface under humid agricultural conditions.
2. **Ground Plane**:
   - Implement a solid uninterrupted Ground Plane on the bottom layer.
   - Avoid routing signal tracks across ground splits.
3. **Decoupling Placement**:
   - Place the `100nF` high-frequency ceramic capacitor within 5mm of the ESP32 `VIN` and `3V3` pins.
4. **Screw Terminal Blocks**:
   - Utilize 5.08mm pitch pluggable screw terminals (Phoenix Contact style) for field sensor connections (Soil, Water, DHT22, Pump, 12V Power).
