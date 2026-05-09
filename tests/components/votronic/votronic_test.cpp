#include <gtest/gtest.h>
#include "common.h"
#include "frames_smartshunt.h"
#include "frames_solar_charger.h"
#include "frames_triple_charger.h"

namespace esphome::votronic::testing {

// ── Solar Charger ─────────────────────────────────────────────────────────────

TEST(VotronicSolarChargerTest, BatteryVoltage) {
  TestableVotronic votronic;
  sensor::Sensor battery_voltage;
  votronic.set_battery_voltage_sensor(&battery_voltage);
  votronic.decode_solar_charger_data_(SOLAR_CHARGER_FRAME_LOW_PV);
  EXPECT_NEAR(battery_voltage.state, 12.63f, 0.01f);
}

TEST(VotronicSolarChargerTest, PvVoltageCurrentPower) {
  TestableVotronic votronic;
  sensor::Sensor pv_voltage, pv_current, pv_power;
  votronic.set_pv_voltage_sensor(&pv_voltage);
  votronic.set_pv_current_sensor(&pv_current);
  votronic.set_pv_power_sensor(&pv_power);
  votronic.decode_solar_charger_data_(SOLAR_CHARGER_FRAME_LOW_PV);
  EXPECT_NEAR(pv_voltage.state, 12.61f, 0.01f);
  EXPECT_NEAR(pv_current.state, 3.2f, 0.01f);
  EXPECT_NEAR(pv_power.state, 40.35f, 0.1f);
}

TEST(VotronicSolarChargerTest, PvControllerTemperature) {
  TestableVotronic votronic;
  sensor::Sensor temp;
  votronic.set_pv_controller_temperature_sensor(&temp);
  votronic.decode_solar_charger_data_(SOLAR_CHARGER_FRAME_LOW_PV);
  EXPECT_FLOAT_EQ(temp.state, 10.0f);
}

TEST(VotronicSolarChargerTest, PvControllerActiveStatus) {
  TestableVotronic votronic;
  binary_sensor::BinarySensor controller_active, current_reduction, aes_active;
  votronic.set_pv_controller_active_binary_sensor(&controller_active);
  votronic.set_pv_current_reduction_binary_sensor(&current_reduction);
  votronic.set_pv_aes_active_binary_sensor(&aes_active);
  votronic.decode_solar_charger_data_(SOLAR_CHARGER_FRAME_LOW_PV);
  EXPECT_TRUE(controller_active.state);
  EXPECT_FALSE(current_reduction.state);
  EXPECT_FALSE(aes_active.state);
}

TEST(VotronicSolarChargerTest, PvControllerStatusText) {
  TestableVotronic votronic;
  text_sensor::TextSensor pv_status;
  votronic.set_pv_controller_status_text_sensor(&pv_status);
  votronic.decode_solar_charger_data_(SOLAR_CHARGER_FRAME_LOW_PV);
  EXPECT_EQ(pv_status.state, "Active");
}

TEST(VotronicSolarChargerTest, BatteryStatusBitmask) {
  TestableVotronic votronic;
  sensor::Sensor bitmask;
  votronic.set_pv_battery_status_bitmask_sensor(&bitmask);
  votronic.decode_solar_charger_data_(SOLAR_CHARGER_FRAME_LOW_PV);
  EXPECT_FLOAT_EQ(bitmask.state, 32.0f);
}

TEST(VotronicSolarChargerTest, ModeSettingAGM) {
  TestableVotronic votronic;
  sensor::Sensor mode_id;
  text_sensor::TextSensor mode_text, bat_status;
  votronic.set_pv_mode_setting_id_sensor(&mode_id);
  votronic.set_pv_mode_setting_text_sensor(&mode_text);
  votronic.set_pv_battery_status_text_sensor(&bat_status);
  votronic.decode_solar_charger_data_(SOLAR_CHARGER_FRAME_HIGH_PV);
  EXPECT_FLOAT_EQ(mode_id.state, 47.0f);
  EXPECT_EQ(mode_text.state, "AGM 14.7-14.8V");
  EXPECT_EQ(bat_status.state, "U1 phase");
}

TEST(VotronicSolarChargerTest, PvCurrentReductionHighPv) {
  TestableVotronic votronic;
  binary_sensor::BinarySensor controller_active, current_reduction, aes_active;
  sensor::Sensor pv_voltage, pv_current;
  votronic.set_pv_controller_active_binary_sensor(&controller_active);
  votronic.set_pv_current_reduction_binary_sensor(&current_reduction);
  votronic.set_pv_aes_active_binary_sensor(&aes_active);
  votronic.set_pv_voltage_sensor(&pv_voltage);
  votronic.set_pv_current_sensor(&pv_current);
  votronic.decode_solar_charger_data_(SOLAR_CHARGER_FRAME_HIGH_PV);
  EXPECT_NEAR(pv_voltage.state, 35.58f, 0.01f);
  EXPECT_NEAR(pv_current.state, 3.7f, 0.01f);
  EXPECT_TRUE(controller_active.state);
  EXPECT_TRUE(current_reduction.state);
  EXPECT_FALSE(aes_active.state);
}

TEST(VotronicSolarChargerTest, ModeSettingLiFePo4) {
  TestableVotronic votronic;
  sensor::Sensor mode_id;
  text_sensor::TextSensor mode_text;
  votronic.set_pv_mode_setting_id_sensor(&mode_id);
  votronic.set_pv_mode_setting_text_sensor(&mode_text);
  votronic.decode_solar_charger_data_(TRIPLE_SOLAR_CHARGER_FRAME);
  EXPECT_FLOAT_EQ(mode_id.state, 82.0f);
  EXPECT_EQ(mode_text.state, "LiFePo4 14.2V");
}

TEST(VotronicSolarChargerTest, NullSensorsDoNotCrash) {
  TestableVotronic votronic;
  votronic.decode_solar_charger_data_(SOLAR_CHARGER_FRAME_LOW_PV);
}

// ── Charger ───────────────────────────────────────────────────────────────────

TEST(VotronicChargerTest, StandbyState) {
  TestableVotronic votronic;
  sensor::Sensor battery_voltage, secondary_voltage, current, power, load, temp, mode_id;
  binary_sensor::BinarySensor charging, discharging;
  text_sensor::TextSensor mode_text, controller_status;
  votronic.set_battery_voltage_sensor(&battery_voltage);
  votronic.set_secondary_battery_voltage_sensor(&secondary_voltage);
  votronic.set_charger_current_sensor(&current);
  votronic.set_charger_power_sensor(&power);
  votronic.set_charger_load_sensor(&load);
  votronic.set_charger_controller_temperature_sensor(&temp);
  votronic.set_charger_mode_setting_id_sensor(&mode_id);
  votronic.set_charger_charging_binary_sensor(&charging);
  votronic.set_charger_discharging_binary_sensor(&discharging);
  votronic.set_charger_mode_setting_text_sensor(&mode_text);
  votronic.set_charger_controller_status_text_sensor(&controller_status);
  votronic.decode_charger_data_(TRIPLE_CHARGER_FRAME_STANDBY);
  EXPECT_NEAR(battery_voltage.state, 13.64f, 0.01f);
  EXPECT_NEAR(secondary_voltage.state, 13.53f, 0.01f);
  EXPECT_NEAR(current.state, 0.0f, 0.01f);
  EXPECT_NEAR(power.state, 0.0f, 0.1f);
  EXPECT_FLOAT_EQ(load.state, 0.0f);
  EXPECT_NEAR(temp.state, 15.1f, 0.1f);
  EXPECT_FLOAT_EQ(mode_id.state, 82.0f);
  EXPECT_FALSE(charging.state);
  EXPECT_FALSE(discharging.state);
  EXPECT_EQ(mode_text.state, "LiFePo4 14.2V");
  EXPECT_EQ(controller_status.state, "Standby");
}

TEST(VotronicChargerTest, ChargingState) {
  TestableVotronic votronic;
  sensor::Sensor battery_voltage, current, power, load;
  binary_sensor::BinarySensor charging, discharging, controller_active;
  text_sensor::TextSensor controller_status;
  votronic.set_battery_voltage_sensor(&battery_voltage);
  votronic.set_charger_current_sensor(&current);
  votronic.set_charger_power_sensor(&power);
  votronic.set_charger_load_sensor(&load);
  votronic.set_charger_charging_binary_sensor(&charging);
  votronic.set_charger_discharging_binary_sensor(&discharging);
  votronic.set_charger_controller_active_binary_sensor(&controller_active);
  votronic.set_charger_controller_status_text_sensor(&controller_status);
  votronic.decode_charger_data_(TRIPLE_CHARGER_FRAME_CHARGING);
  EXPECT_NEAR(battery_voltage.state, 13.37f, 0.01f);
  EXPECT_NEAR(current.state, 21.2f, 0.1f);
  EXPECT_NEAR(power.state, 283.4f, 1.0f);
  EXPECT_FLOAT_EQ(load.state, 97.0f);
  EXPECT_TRUE(charging.state);
  EXPECT_FALSE(discharging.state);
  EXPECT_TRUE(controller_active.state);
  EXPECT_EQ(controller_status.state, "Charging Battery 1;Charging Battery 2 / Reverse Charging");
}

TEST(VotronicChargerTest, NullSensorsDoNotCrash) {
  TestableVotronic votronic;
  votronic.decode_charger_data_(TRIPLE_CHARGER_FRAME_STANDBY);
}

// ── Charging Converter ────────────────────────────────────────────────────────

TEST(VotronicChargingConverterTest, VoltageCurrentPower) {
  TestableVotronic votronic;
  sensor::Sensor battery_voltage, secondary_voltage, current, power, load, temp, mode_id;
  binary_sensor::BinarySensor charging, discharging, controller_active;
  text_sensor::TextSensor mode_text, battery_status, controller_status;
  votronic.set_charging_converter_battery_voltage_sensor(&battery_voltage);
  votronic.set_charging_converter_secondary_battery_voltage_sensor(&secondary_voltage);
  votronic.set_charging_converter_current_sensor(&current);
  votronic.set_charging_converter_power_sensor(&power);
  votronic.set_charging_converter_load_sensor(&load);
  votronic.set_charging_converter_controller_temperature_sensor(&temp);
  votronic.set_charging_converter_mode_setting_id_sensor(&mode_id);
  votronic.set_charging_converter_charging_binary_sensor(&charging);
  votronic.set_charging_converter_discharging_binary_sensor(&discharging);
  votronic.set_charging_converter_controller_active_binary_sensor(&controller_active);
  votronic.set_charging_converter_mode_setting_text_sensor(&mode_text);
  votronic.set_charging_converter_battery_status_text_sensor(&battery_status);
  votronic.set_charging_converter_controller_status_text_sensor(&controller_status);
  votronic.decode_charging_converter_data_(TRIPLE_CHARGING_CONVERTER_FRAME);
  EXPECT_NEAR(battery_voltage.state, 13.64f, 0.01f);
  EXPECT_NEAR(secondary_voltage.state, 13.54f, 0.01f);
  EXPECT_NEAR(current.state, 29.9f, 0.1f);
  EXPECT_NEAR(power.state, 407.8f, 1.0f);
  EXPECT_FLOAT_EQ(load.state, 99.0f);
  EXPECT_NEAR(temp.state, 15.1f, 0.1f);
  EXPECT_FLOAT_EQ(mode_id.state, 82.0f);
  EXPECT_TRUE(charging.state);
  EXPECT_FALSE(discharging.state);
  EXPECT_TRUE(controller_active.state);
  EXPECT_EQ(mode_text.state, "LiFePo4 14.2V");
  EXPECT_EQ(battery_status.state, "U3 phase");
  EXPECT_EQ(controller_status.state, "Charging Battery 1;Charging Battery 2 / Reverse Charging");
}

TEST(VotronicChargingConverterTest, NullSensorsDoNotCrash) {
  TestableVotronic votronic;
  votronic.decode_charging_converter_data_(TRIPLE_CHARGING_CONVERTER_FRAME);
}

// ── Battery Computer Info1 ────────────────────────────────────────────────────

TEST(VotronicBatteryComputerInfo1Test, ChargingState) {
  TestableVotronic votronic;
  sensor::Sensor battery_voltage, secondary_voltage, capacity_remaining, soc, current, power;
  binary_sensor::BinarySensor charging, discharging;
  votronic.set_battery_voltage_sensor(&battery_voltage);
  votronic.set_secondary_battery_voltage_sensor(&secondary_voltage);
  votronic.set_battery_capacity_remaining_sensor(&capacity_remaining);
  votronic.set_state_of_charge_sensor(&soc);
  votronic.set_current_sensor(&current);
  votronic.set_power_sensor(&power);
  votronic.set_charging_binary_sensor(&charging);
  votronic.set_discharging_binary_sensor(&discharging);
  votronic.decode_battery_computer_info1_data_(SMARTSHUNT_INFO1_FRAME_CHARGING);
  EXPECT_NEAR(battery_voltage.state, 13.54f, 0.01f);
  EXPECT_NEAR(secondary_voltage.state, 13.01f, 0.01f);
  EXPECT_FLOAT_EQ(capacity_remaining.state, 460.0f);
  EXPECT_FLOAT_EQ(soc.state, 100.0f);
  EXPECT_NEAR(current.state, 0.389f, 0.001f);
  EXPECT_NEAR(power.state, 5.27f, 0.1f);
  EXPECT_TRUE(charging.state);
  EXPECT_FALSE(discharging.state);
}

TEST(VotronicBatteryComputerInfo1Test, DischargingState) {
  TestableVotronic votronic;
  sensor::Sensor current, soc;
  binary_sensor::BinarySensor charging, discharging;
  votronic.set_current_sensor(&current);
  votronic.set_state_of_charge_sensor(&soc);
  votronic.set_charging_binary_sensor(&charging);
  votronic.set_discharging_binary_sensor(&discharging);
  votronic.decode_battery_computer_info1_data_(SMARTSHUNT_INFO1_FRAME_DISCHARGING);
  EXPECT_NEAR(current.state, -0.389f, 0.001f);
  EXPECT_FLOAT_EQ(soc.state, 99.0f);
  EXPECT_FALSE(charging.state);
  EXPECT_TRUE(discharging.state);
}

TEST(VotronicBatteryComputerInfo1Test, NullSensorsDoNotCrash) {
  TestableVotronic votronic;
  votronic.decode_battery_computer_info1_data_(SMARTSHUNT_INFO1_FRAME_CHARGING);
}

// ── Battery Computer Info2 ────────────────────────────────────────────────────

TEST(VotronicBatteryComputerInfo2Test, NominalCapacityAndStatus) {
  TestableVotronic votronic;
  sensor::Sensor nominal_capacity, status_bitmask;
  text_sensor::TextSensor status_text;
  votronic.set_battery_nominal_capacity_sensor(&nominal_capacity);
  votronic.set_battery_status_bitmask_sensor(&status_bitmask);
  votronic.set_battery_status_text_sensor(&status_text);
  votronic.decode_battery_computer_info2_data_(SMARTSHUNT_INFO2_FRAME);
  EXPECT_NEAR(nominal_capacity.state, 460.0f, 0.1f);
  EXPECT_FLOAT_EQ(status_bitmask.state, 4.0f);
  EXPECT_EQ(status_text.state, "U2 phase");
}

TEST(VotronicBatteryComputerInfo2Test, NullSensorsDoNotCrash) {
  TestableVotronic votronic;
  votronic.decode_battery_computer_info2_data_(SMARTSHUNT_INFO2_FRAME);
}

// ── Dispatch ──────────────────────────────────────────────────────────────────

TEST(VotronicDispatchTest, SolarChargerFrameDispatched) {
  TestableVotronic votronic;
  sensor::Sensor pv_voltage;
  votronic.set_pv_voltage_sensor(&pv_voltage);
  votronic.on_votronic_data(SOLAR_CHARGER_FRAME_LOW_PV);
  EXPECT_NEAR(pv_voltage.state, 12.61f, 0.01f);
}

TEST(VotronicDispatchTest, ChargerFrameDispatched) {
  TestableVotronic votronic;
  sensor::Sensor battery_voltage;
  votronic.set_battery_voltage_sensor(&battery_voltage);
  votronic.on_votronic_data(TRIPLE_CHARGER_FRAME_STANDBY);
  EXPECT_NEAR(battery_voltage.state, 13.64f, 0.01f);
}

TEST(VotronicDispatchTest, ChargingConverterFrameDispatched) {
  TestableVotronic votronic;
  sensor::Sensor current;
  votronic.set_charging_converter_current_sensor(&current);
  votronic.on_votronic_data(TRIPLE_CHARGING_CONVERTER_FRAME);
  EXPECT_NEAR(current.state, 29.9f, 0.1f);
}

TEST(VotronicDispatchTest, BatteryComputerInfo1FrameDispatched) {
  TestableVotronic votronic;
  sensor::Sensor soc;
  votronic.set_state_of_charge_sensor(&soc);
  votronic.on_votronic_data(SMARTSHUNT_INFO1_FRAME_CHARGING);
  EXPECT_FLOAT_EQ(soc.state, 100.0f);
}

TEST(VotronicDispatchTest, BatteryComputerInfo2FrameDispatched) {
  TestableVotronic votronic;
  sensor::Sensor nominal_capacity;
  votronic.set_battery_nominal_capacity_sensor(&nominal_capacity);
  votronic.on_votronic_data(SMARTSHUNT_INFO2_FRAME);
  EXPECT_NEAR(nominal_capacity.state, 460.0f, 0.1f);
}

TEST(VotronicDispatchTest, InvalidLengthFrameSkipped) {
  TestableVotronic votronic;
  sensor::Sensor pv_voltage;
  pv_voltage.state = -1.0f;
  votronic.set_pv_voltage_sensor(&pv_voltage);
  votronic.on_votronic_data({0xAA, 0x1A, 0x01, 0x02});
  EXPECT_FLOAT_EQ(pv_voltage.state, -1.0f);
}

}  // namespace esphome::votronic::testing
