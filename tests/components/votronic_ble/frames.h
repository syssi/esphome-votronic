#pragma once
#include <vector>

namespace esphome::votronic_ble::testing {

// Solar charger frames (19 bytes each)
// Byte layout:
//   0-1:  battery_voltage * 100  (LE)
//   2-3:  pv_voltage * 100       (LE)
//   4-5:  pv_current * 10        (LE)
//   6-7:  unknown
//   8:    battery_status_bitmask
//   9-11: unknown
//   12:   pv_controller_status_bitmask
//   13-14: charged_capacity      (LE)
//   15-16: charged_energy / 10   (LE)
//   17-18: pv_power * 10         (LE)

// Active charging: bat=13.56V, pv=22.40V, I=5.0A, P=112.0W,
//   battery_status=U1 phase (0x02), controller=Active (0x08),
//   charged_capacity=100Ah, charged_energy=5000Wh
static const std::vector<uint8_t> SOLAR_CHARGER_FRAME_ACTIVE = {
    0x4C, 0x05,        // battery_voltage: 1356 * 0.01 = 13.56 V
    0xC0, 0x08,        // pv_voltage: 2240 * 0.01 = 22.40 V
    0x32, 0x00,        // pv_current: 50 * 0.1 = 5.0 A
    0x00, 0x00,        // unknown
    0x02,              // battery_status: U1 phase
    0x00, 0x00, 0x00,  // unknown
    0x08,              // pv_controller_status: Active (bit 3)
    0x64, 0x00,        // charged_capacity: 100 Ah
    0xF4, 0x01,        // charged_energy: 500 * 10 = 5000 Wh
    0x60, 0x04,        // pv_power: 1120 * 0.1 = 112.0 W
};

// AES active: bat=12.60V, pv=12.55V, I=0.3A, P=0.3W,
//   battery_status=Standby (0x00), controller=AES active (0x20)
static const std::vector<uint8_t> SOLAR_CHARGER_FRAME_AES = {
    0xEC, 0x04,        // battery_voltage: 1260 * 0.01 = 12.60 V
    0xE7, 0x04,        // pv_voltage: 1255 * 0.01 = 12.55 V
    0x03, 0x00,        // pv_current: 3 * 0.1 = 0.3 A
    0x00, 0x00,        // unknown
    0x00,              // battery_status: Standby
    0x00, 0x00, 0x00,  // unknown
    0x20,              // pv_controller_status: AES active (bit 5)
    0x32, 0x00,        // charged_capacity: 50 Ah
    0x0A, 0x00,        // charged_energy: 10 * 10 = 100 Wh
    0x03, 0x00,        // pv_power: 3 * 0.1 = 0.3 W
};

// Reduced output: pv_controller_status=Reduced (0x10)
static const std::vector<uint8_t> SOLAR_CHARGER_FRAME_REDUCED = {
    0x58, 0x05,        // battery_voltage: 1368 * 0.01 = 13.68 V
    0x48, 0x0B,        // pv_voltage: 2888 * 0.01 = 28.88 V
    0x14, 0x00,        // pv_current: 20 * 0.1 = 2.0 A
    0x00, 0x00,        // unknown
    0x04,              // battery_status: U2 phase (bit 2)
    0x00, 0x00, 0x00,  // unknown
    0x10,              // pv_controller_status: Reduced (bit 4)
    0xC8, 0x00,        // charged_capacity: 200 Ah
    0x32, 0x00,        // charged_energy: 50 * 10 = 500 Wh
    0x14, 0x00,        // pv_power: 20 * 0.1 = 2.0 W
};

// Battery computer frames (20 bytes each)
// Byte layout:
//   0-1:  battery_voltage * 100           (LE)
//   2-3:  secondary_battery_voltage * 100 (LE)
//   4-5:  battery_capacity_remaining      (LE)
//   6-7:  unknown
//   8:    state_of_charge
//   9:    unknown
//   10-12: current * 1000                 (24-bit signed LE)
//   13-14: battery_nominal_capacity * 10  (LE)
//   15-19: unknown

// Charging: bat=13.50V, sec=13.20V, cap=150Ah, soc=75%, I=+5.000A,
//   P=67.50W, nominal=200Ah
static const std::vector<uint8_t> BATTERY_COMPUTER_FRAME_CHARGING = {
    0x46, 0x05,                    // battery_voltage: 1350 * 0.01 = 13.50 V
    0x28, 0x05,                    // secondary_voltage: 1320 * 0.01 = 13.20 V
    0x96, 0x00,                    // capacity_remaining: 150 Ah
    0x00, 0x00,                    // unknown
    0x4B,                          // state_of_charge: 75 %
    0x00,                          // unknown
    0x88, 0x13, 0x00,              // current: 5000 * 0.001 = 5.000 A
    0xD0, 0x07,                    // nominal_capacity: 2000 * 0.1 = 200.0 Ah
    0x00, 0x00, 0x00, 0x00, 0x00,  // unknown
};

// Discharging: bat=12.80V, sec=12.70V, cap=120Ah, soc=60%, I=-2.500A,
//   P=-32.0W, nominal=200Ah
static const std::vector<uint8_t> BATTERY_COMPUTER_FRAME_DISCHARGING = {
    0x00, 0x05,                    // battery_voltage: 1280 * 0.01 = 12.80 V
    0xF6, 0x04,                    // secondary_voltage: 1270 * 0.01 = 12.70 V
    0x78, 0x00,                    // capacity_remaining: 120 Ah
    0x00, 0x00,                    // unknown
    0x3C,                          // state_of_charge: 60 %
    0x00,                          // unknown
    0x3C, 0xF6, 0xFF,              // current: 0xFFF63C -> -2500 * 0.001 = -2.500 A
    0xD0, 0x07,                    // nominal_capacity: 2000 * 0.1 = 200.0 Ah
    0x00, 0x00, 0x00, 0x00, 0x00,  // unknown
};

}  // namespace esphome::votronic_ble::testing
