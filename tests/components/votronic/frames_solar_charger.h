#pragma once
#include <cstdint>
#include <vector>

namespace esphome::votronic::testing {

// Quelle: docs/pdus/display-link-solar-charger.txt, Zeile 1
// Battery: 12.63V  PV: 12.61V  PV-I: 3.2A  PV-Power: 40.4W  Temp: 10°C
// Mode-ID: 1  BatStatus: 0x20  PVStatus: 0x08 (Active)
static const std::vector<uint8_t> SOLAR_CHARGER_FRAME_LOW_PV = {
    0xAA, 0x1A, 0xEF, 0x04, 0xED, 0x04, 0x20, 0x00, 0x00, 0x00, 0x9B, 0x0A, 0x01, 0x20, 0x08, 0x80,
};

// Quelle: docs/pdus/display-link-solar-charger.txt, Zeile 3
// Battery: 13.50V  PV: 35.58V  PV-I: 3.7A  PV-Power: 131.6W  Temp: 48°C
// Mode: AGM 14.7-14.8V (47)  BatStatus: 0x02 (U1 phase)  PVStatus: 0x19 (Reduced)
static const std::vector<uint8_t> SOLAR_CHARGER_FRAME_HIGH_PV = {
    0xAA, 0x1A, 0x46, 0x05, 0xE6, 0x0D, 0x25, 0x00, 0x00, 0x00, 0x00, 0x30, 0x2F, 0x02, 0x19, 0x93,
};

}  // namespace esphome::votronic::testing
