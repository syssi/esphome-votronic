#pragma once
#include <cstdint>
#include <vector>

namespace esphome::votronic::testing {

// Quelle: docs/pdus/display-link-smartshunt-display.log
// BatV: 13.54V  SecBatV: 13.01V  CapRemain: 460Ah  SOC: 100%  Current: 0.389A  Charging
static const std::vector<uint8_t> SMARTSHUNT_INFO1_FRAME_CHARGING = {
    0xAA, 0xCA, 0x4A, 0x05, 0x15, 0x05, 0xCC, 0x01, 0x20, 0x00, 0x64, 0x00, 0x85, 0x01, 0x00, 0x98,
};

// Quelle: docs/pdus/display-link-smartshunt-display.log
// NominalCapacity: 460.0Ah  BatStatus: 0x04 (U2 phase)
static const std::vector<uint8_t> SMARTSHUNT_INFO2_FRAME = {
    0xAA, 0xDA, 0x00, 0x00, 0x00, 0x00, 0xF8, 0x11, 0x5E, 0x0A, 0x00, 0x00, 0x2F, 0x04, 0x02, 0x4E,
};

// Quelle: Kommentar in components/votronic/votronic.cpp (decode_battery_computer_info1_data_)
// BatV: 12.99V  SecBatV: 13.11V  CapRemain: 455Ah  SOC: 99%  Current: -0.389A  Discharging
static const std::vector<uint8_t> SMARTSHUNT_INFO1_FRAME_DISCHARGING = {
    0xAA, 0xCA, 0x03, 0x05, 0x0F, 0x05, 0xC7, 0x01, 0x20, 0x00, 0x63, 0x00, 0x7B, 0xFE, 0xFF, 0x39,
};

}  // namespace esphome::votronic::testing
