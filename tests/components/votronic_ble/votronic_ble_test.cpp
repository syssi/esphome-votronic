#include <gtest/gtest.h>
#include "common.h"
#include "frames.h"

namespace esphome::votronic_ble::testing {

// Solar charger

TEST(VotronicBleSolarTest, BatteryVoltage) {
  TestableVotronicBle ble;
  sensor::Sensor battery_voltage;
  ble.set_battery_voltage_sensor(&battery_voltage);
  ble.decode_solar_charger_data_(SOLAR_CHARGER_FRAME_ACTIVE);
  EXPECT_NEAR(battery_voltage.state, 13.56f, 0.01f);
}

TEST(VotronicBleSolarTest, PvVoltageCurrentPower) {
  TestableVotronicBle ble;
  sensor::Sensor pv_voltage, pv_current, pv_power;
  ble.set_pv_voltage_sensor(&pv_voltage);
  ble.set_pv_current_sensor(&pv_current);
  ble.set_pv_power_sensor(&pv_power);
  ble.decode_solar_charger_data_(SOLAR_CHARGER_FRAME_ACTIVE);
  EXPECT_NEAR(pv_voltage.state, 22.40f, 0.01f);
  EXPECT_NEAR(pv_current.state, 5.0f, 0.01f);
  EXPECT_NEAR(pv_power.state, 112.0f, 0.1f);
}

TEST(VotronicBleSolarTest, ChargedCapacityAndEnergy) {
  TestableVotronicBle ble;
  sensor::Sensor charged_capacity, charged_energy;
  ble.set_charged_capacity_sensor(&charged_capacity);
  ble.set_charged_energy_sensor(&charged_energy);
  ble.decode_solar_charger_data_(SOLAR_CHARGER_FRAME_ACTIVE);
  EXPECT_FLOAT_EQ(charged_capacity.state, 100.0f);
  EXPECT_FLOAT_EQ(charged_energy.state, 5000.0f);
}

TEST(VotronicBleSolarTest, BatteryStatusU1Phase) {
  TestableVotronicBle ble;
  sensor::Sensor bitmask;
  text_sensor::TextSensor status_text;
  ble.set_battery_status_bitmask_sensor(&bitmask);
  ble.set_battery_status_text_sensor(&status_text);
  ble.decode_solar_charger_data_(SOLAR_CHARGER_FRAME_ACTIVE);
  EXPECT_FLOAT_EQ(bitmask.state, 2.0f);
  EXPECT_EQ(status_text.state, "U1 phase");
}

TEST(VotronicBleSolarTest, PvControllerActive) {
  TestableVotronicBle ble;
  sensor::Sensor pv_bitmask;
  text_sensor::TextSensor pv_status;
  binary_sensor::BinarySensor controller_active, current_reduction, aes_active;
  ble.set_pv_controller_status_bitmask_sensor(&pv_bitmask);
  ble.set_pv_controller_status_text_sensor(&pv_status);
  ble.set_controller_active_binary_sensor(&controller_active);
  ble.set_current_reduction_binary_sensor(&current_reduction);
  ble.set_aes_active_binary_sensor(&aes_active);
  ble.decode_solar_charger_data_(SOLAR_CHARGER_FRAME_ACTIVE);
  EXPECT_FLOAT_EQ(pv_bitmask.state, 8.0f);
  EXPECT_EQ(pv_status.state, "Active");
  EXPECT_TRUE(controller_active.state);
  EXPECT_FALSE(current_reduction.state);
  EXPECT_FALSE(aes_active.state);
}

TEST(VotronicBleSolarTest, PvControllerAesActive) {
  TestableVotronicBle ble;
  text_sensor::TextSensor pv_status, battery_status;
  binary_sensor::BinarySensor controller_active, current_reduction, aes_active;
  ble.set_pv_controller_status_text_sensor(&pv_status);
  ble.set_battery_status_text_sensor(&battery_status);
  ble.set_controller_active_binary_sensor(&controller_active);
  ble.set_current_reduction_binary_sensor(&current_reduction);
  ble.set_aes_active_binary_sensor(&aes_active);
  ble.decode_solar_charger_data_(SOLAR_CHARGER_FRAME_AES);
  EXPECT_EQ(pv_status.state, "AES active");
  EXPECT_EQ(battery_status.state, "Standby");
  EXPECT_FALSE(controller_active.state);
  EXPECT_FALSE(current_reduction.state);
  EXPECT_TRUE(aes_active.state);
}

TEST(VotronicBleSolarTest, PvControllerReduced) {
  TestableVotronicBle ble;
  text_sensor::TextSensor pv_status, battery_status;
  binary_sensor::BinarySensor controller_active, current_reduction;
  ble.set_pv_controller_status_text_sensor(&pv_status);
  ble.set_battery_status_text_sensor(&battery_status);
  ble.set_controller_active_binary_sensor(&controller_active);
  ble.set_current_reduction_binary_sensor(&current_reduction);
  ble.decode_solar_charger_data_(SOLAR_CHARGER_FRAME_REDUCED);
  EXPECT_EQ(pv_status.state, "Reduced");
  EXPECT_EQ(battery_status.state, "U2 phase");
  EXPECT_FALSE(controller_active.state);
  EXPECT_TRUE(current_reduction.state);
}

TEST(VotronicBleSolarTest, InvalidFrameSizeIgnored) {
  TestableVotronicBle ble;
  sensor::Sensor pv_voltage;
  pv_voltage.state = -1.0f;
  ble.set_pv_voltage_sensor(&pv_voltage);
  ble.decode_solar_charger_data_({0xAA, 0xBB, 0xCC});
  EXPECT_FLOAT_EQ(pv_voltage.state, -1.0f);
}

TEST(VotronicBleSolarTest, NullSensorsDoNotCrash) {
  TestableVotronicBle ble;
  ble.decode_solar_charger_data_(SOLAR_CHARGER_FRAME_ACTIVE);
}

// Battery computer

TEST(VotronicBleBatteryComputerTest, ChargingVoltagesAndCapacity) {
  TestableVotronicBle ble;
  sensor::Sensor battery_voltage, secondary_voltage, capacity_remaining, soc, nominal_capacity;
  ble.set_battery_voltage_sensor(&battery_voltage);
  ble.set_secondary_battery_voltage_sensor(&secondary_voltage);
  ble.set_battery_capacity_remaining_sensor(&capacity_remaining);
  ble.set_state_of_charge_sensor(&soc);
  ble.set_battery_nominal_capacity_sensor(&nominal_capacity);
  ble.decode_battery_computer_data_(BATTERY_COMPUTER_FRAME_CHARGING);
  EXPECT_NEAR(battery_voltage.state, 13.50f, 0.01f);
  EXPECT_NEAR(secondary_voltage.state, 13.20f, 0.01f);
  EXPECT_FLOAT_EQ(capacity_remaining.state, 150.0f);
  EXPECT_FLOAT_EQ(soc.state, 75.0f);
  EXPECT_NEAR(nominal_capacity.state, 200.0f, 0.1f);
}

TEST(VotronicBleBatteryComputerTest, ChargingCurrentPowerState) {
  TestableVotronicBle ble;
  sensor::Sensor battery_voltage, current, power;
  binary_sensor::BinarySensor charging, discharging;
  ble.set_battery_voltage_sensor(&battery_voltage);
  ble.set_current_sensor(&current);
  ble.set_power_sensor(&power);
  ble.set_charging_binary_sensor(&charging);
  ble.set_discharging_binary_sensor(&discharging);
  ble.decode_battery_computer_data_(BATTERY_COMPUTER_FRAME_CHARGING);
  EXPECT_NEAR(battery_voltage.state, 13.50f, 0.01f);
  EXPECT_NEAR(current.state, 5.000f, 0.001f);
  EXPECT_NEAR(power.state, 13.50f * 5.000f, 0.1f);
  EXPECT_TRUE(charging.state);
  EXPECT_FALSE(discharging.state);
}

TEST(VotronicBleBatteryComputerTest, DischargingCurrentState) {
  TestableVotronicBle ble;
  sensor::Sensor current;
  binary_sensor::BinarySensor charging, discharging;
  ble.set_current_sensor(&current);
  ble.set_charging_binary_sensor(&charging);
  ble.set_discharging_binary_sensor(&discharging);
  ble.decode_battery_computer_data_(BATTERY_COMPUTER_FRAME_DISCHARGING);
  EXPECT_NEAR(current.state, -2.500f, 0.001f);
  EXPECT_FALSE(charging.state);
  EXPECT_TRUE(discharging.state);
}

TEST(VotronicBleBatteryComputerTest, InvalidFrameSizeIgnored) {
  TestableVotronicBle ble;
  sensor::Sensor battery_voltage;
  battery_voltage.state = -1.0f;
  ble.set_battery_voltage_sensor(&battery_voltage);
  ble.decode_battery_computer_data_({0xAA, 0xBB});
  EXPECT_FLOAT_EQ(battery_voltage.state, -1.0f);
}

TEST(VotronicBleBatteryComputerTest, NullSensorsDoNotCrash) {
  TestableVotronicBle ble;
  ble.decode_battery_computer_data_(BATTERY_COMPUTER_FRAME_CHARGING);
}

// String converters

TEST(VotronicBleBitmaskTest, BatteryStatusAllValues) {
  TestableVotronicBle ble;
  EXPECT_EQ(ble.battery_status_bitmask_to_string_(0x00), "Standby");
  EXPECT_EQ(ble.battery_status_bitmask_to_string_(0x01), "I phase");
  EXPECT_EQ(ble.battery_status_bitmask_to_string_(0x02), "U1 phase");
  EXPECT_EQ(ble.battery_status_bitmask_to_string_(0x04), "U2 phase");
  EXPECT_EQ(ble.battery_status_bitmask_to_string_(0x08), "U3 phase");
}

TEST(VotronicBleBitmaskTest, SolarChargerStatusAllValues) {
  TestableVotronicBle ble;
  EXPECT_EQ(ble.solar_charger_status_bitmask_to_string_(0x00), "Standby");
  EXPECT_EQ(ble.solar_charger_status_bitmask_to_string_(0x08), "Active");
  EXPECT_EQ(ble.solar_charger_status_bitmask_to_string_(0x10), "Reduced");
  EXPECT_EQ(ble.solar_charger_status_bitmask_to_string_(0x20), "AES active");
}

TEST(VotronicBleBitmaskTest, UnknownBitmaskFormatted) {
  TestableVotronicBle ble;
  EXPECT_EQ(ble.battery_status_bitmask_to_string_(0x40), "Unknown (0x40)");
  EXPECT_EQ(ble.solar_charger_status_bitmask_to_string_(0x01), "Unknown (0x01)");
}

}  // namespace esphome::votronic_ble::testing
