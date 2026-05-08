#pragma once
#include <cstdint>
#include <vector>

namespace esphome::votronic::testing {

// Quelle: docs/pdus/display-link-triple-charger-doing-pv-charging-plus-ac-charging.txt, Zeile 1
// Solar: Battery: 13.64V  PV: 21.79V  PV-I: 0A  Temp: 50°C  Mode: LiFePo4 14.2V (82)
// PVStatus: 0x01  BatStatus: 0x20
static const std::vector<uint8_t> TRIPLE_SOLAR_CHARGER_FRAME = {
    0xAA, 0x1A, 0x54, 0x05, 0x83, 0x08, 0x00, 0x00, 0x00, 0xE0, 0x97, 0x32, 0x52, 0x20, 0x01, 0xF6,
};

// Quelle: docs/pdus/display-link-triple-charger-doing-pv-charging-plus-ac-charging.txt, Zeile 2
// Charging converter: Battery: 13.64V  Secondary: 13.54V  Current: 29.9A  Load: 99%
// Temp: 15.1°C  Mode: LiFePo4 14.2V (82)  ControllerStatus: 0x0C
static const std::vector<uint8_t> TRIPLE_CHARGING_CONVERTER_FRAME = {
    0xAA, 0x7A, 0x54, 0x05, 0x4A, 0x05, 0x2B, 0x01, 0x00, 0x00, 0x63, 0x97, 0x52, 0x28, 0x0C, 0xCC,
};

// Quelle: docs/pdus/display-link-triple-charger-doing-pv-charging-plus-ac-charging.txt, Zeile 4
// Charger: Battery: 13.64V  Secondary: 13.53V  Current: 0A  Load: 0%
// Temp: 15.1°C  Mode: LiFePo4 14.2V (82)  ControllerStatus: 0x00 (Standby)
static const std::vector<uint8_t> TRIPLE_CHARGER_FRAME_STANDBY = {
    0xAA, 0x3A, 0x54, 0x05, 0x49, 0x05, 0x00, 0x00, 0x00, 0x01, 0x00, 0x97, 0x52, 0x28, 0x00, 0xCB,
};

// Quelle: docs/pdus/display-link-triple-charger-doing-b2b-charging-plus-ac-charging.txt, Zeile 17
// Charger: Battery: 13.37V  Secondary: 13.18V  Current: 21.2A  Load: 97%
// Temp: 15.0°C  Mode: LiFePo4 14.2V (82)  ControllerStatus: 0x0C (Active)
static const std::vector<uint8_t> TRIPLE_CHARGER_FRAME_CHARGING = {
    0xAA, 0x3A, 0x39, 0x05, 0x26, 0x05, 0xD4, 0x00, 0x00, 0x01, 0x61, 0x96, 0x52, 0x28, 0x0C, 0x71,
};

}  // namespace esphome::votronic::testing
