#pragma once

#include "esphome/core/component.h"
#include "esphome/components/binary_sensor/binary_sensor.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/text_sensor/text_sensor.h"
#include "esphome/components/uart/uart.h"

namespace esphome {
namespace votronic {

class Votronic : public uart::UARTDevice, public PollingComponent {
 public:
  void loop() override;
  void dump_config() override;
  void update() override;
  float get_setup_priority() const override;

  void set_charging_binary_sensor(binary_sensor::BinarySensor *charging_binary_sensor) {
    charging_binary_sensor_ = charging_binary_sensor;
  }
  void set_discharging_binary_sensor(binary_sensor::BinarySensor *discharging_binary_sensor) {
    discharging_binary_sensor_ = discharging_binary_sensor;
  }
  void set_charger_charging_binary_sensor(binary_sensor::BinarySensor *charger_charging_binary_sensor) {
    charger_charging_binary_sensor_ = charger_charging_binary_sensor;
  }
  void set_charger_discharging_binary_sensor(binary_sensor::BinarySensor *charger_discharging_binary_sensor) {
    charger_discharging_binary_sensor_ = charger_discharging_binary_sensor;
  }
  void set_charger_controller_active_binary_sensor(
      binary_sensor::BinarySensor *charger_controller_active_binary_sensor) {
    charger_controller_active_binary_sensor_ = charger_controller_active_binary_sensor;
  }
  void set_charger_aes_active_binary_sensor(binary_sensor::BinarySensor *charger_aes_active_binary_sensor) {
    charger_aes_active_binary_sensor_ = charger_aes_active_binary_sensor;
  }
  void set_charger_current_reduction_binary_sensor(
      binary_sensor::BinarySensor *charger_current_reduction_binary_sensor) {
    charger_current_reduction_binary_sensor_ = charger_current_reduction_binary_sensor;
  }
  void set_pv_controller_active_binary_sensor(binary_sensor::BinarySensor *pv_controller_active_binary_sensor) {
    pv_controller_active_binary_sensor_ = pv_controller_active_binary_sensor;
  }
  void set_pv_aes_active_binary_sensor(binary_sensor::BinarySensor *pv_aes_active_binary_sensor) {
    pv_aes_active_binary_sensor_ = pv_aes_active_binary_sensor;
  }
  void set_pv_current_reduction_binary_sensor(binary_sensor::BinarySensor *pv_current_reduction_binary_sensor) {
    pv_current_reduction_binary_sensor_ = pv_current_reduction_binary_sensor;
  }
  void set_charging_converter_charging_binary_sensor(
      binary_sensor::BinarySensor *charging_converter_charging_binary_sensor) {
    charging_converter_charging_binary_sensor_ = charging_converter_charging_binary_sensor;
  }
  void set_charging_converter_discharging_binary_sensor(
      binary_sensor::BinarySensor *charging_converter_discharging_binary_sensor) {
    charging_converter_discharging_binary_sensor_ = charging_converter_discharging_binary_sensor;
  }
  void set_charging_converter_controller_active_binary_sensor(
      binary_sensor::BinarySensor *charging_converter_controller_active_binary_sensor) {
    charging_converter_controller_active_binary_sensor_ = charging_converter_controller_active_binary_sensor;
  }
  void set_charging_converter_current_reduction_binary_sensor(
      binary_sensor::BinarySensor *charging_converter_current_reduction_binary_sensor) {
    charging_converter_current_reduction_binary_sensor_ = charging_converter_current_reduction_binary_sensor;
  }
  void set_charging_converter_aes_active_binary_sensor(
      binary_sensor::BinarySensor *charging_converter_aes_active_binary_sensor) {
    charging_converter_aes_active_binary_sensor_ = charging_converter_aes_active_binary_sensor;
  }

  void set_battery_voltage_sensor(sensor::Sensor *battery_voltage_sensor) {
    battery_voltage_sensor_ = battery_voltage_sensor;
  }
  void set_secondary_battery_voltage_sensor(sensor::Sensor *secondary_battery_voltage_sensor) {
    secondary_battery_voltage_sensor_ = secondary_battery_voltage_sensor;
  }
  void set_state_of_charge_sensor(sensor::Sensor *state_of_charge_sensor) {
    state_of_charge_sensor_ = state_of_charge_sensor;
  }
  void set_current_sensor(sensor::Sensor *current_sensor) { current_sensor_ = current_sensor; }
  void set_power_sensor(sensor::Sensor *power_sensor) { power_sensor_ = power_sensor; }
  void set_charger_current_sensor(sensor::Sensor *charger_current_sensor) {
    charger_current_sensor_ = charger_current_sensor;
  }
  void set_charger_power_sensor(sensor::Sensor *charger_power_sensor) { charger_power_sensor_ = charger_power_sensor; }
  void set_charger_load_sensor(sensor::Sensor *charger_load_sensor) { charger_load_sensor_ = charger_load_sensor; }
  void set_charger_battery_status_bitmask_sensor(sensor::Sensor *charger_battery_status_bitmask_sensor) {
    charger_battery_status_bitmask_sensor_ = charger_battery_status_bitmask_sensor;
  }
  void set_pv_voltage_sensor(sensor::Sensor *pv_voltage_sensor) { pv_voltage_sensor_ = pv_voltage_sensor; }
  void set_pv_current_sensor(sensor::Sensor *pv_current_sensor) { pv_current_sensor_ = pv_current_sensor; }
  void set_pv_power_sensor(sensor::Sensor *pv_power_sensor) { pv_power_sensor_ = pv_power_sensor; }
  void set_battery_status_bitmask_sensor(sensor::Sensor *battery_status_bitmask_sensor) {
    battery_status_bitmask_sensor_ = battery_status_bitmask_sensor;
  }
  void set_pv_battery_status_bitmask_sensor(sensor::Sensor *pv_battery_status_bitmask_sensor) {
    pv_battery_status_bitmask_sensor_ = pv_battery_status_bitmask_sensor;
  }
  void set_charger_controller_status_bitmask_sensor(sensor::Sensor *charger_controller_status_bitmask_sensor) {
    charger_controller_status_bitmask_sensor_ = charger_controller_status_bitmask_sensor;
  }
  void set_pv_controller_status_bitmask_sensor(sensor::Sensor *pv_controller_status_bitmask_sensor) {
    pv_controller_status_bitmask_sensor_ = pv_controller_status_bitmask_sensor;
  }
  void set_charger_mode_setting_id_sensor(sensor::Sensor *charger_mode_setting_id_sensor) {
    charger_mode_setting_id_sensor_ = charger_mode_setting_id_sensor;
  }
  void set_charger_battery_status_text_sensor(text_sensor::TextSensor *charger_battery_status_text_sensor) {
    charger_battery_status_text_sensor_ = charger_battery_status_text_sensor;
  }
  void set_charger_controller_temperature_sensor(sensor::Sensor *charger_controller_temperature_sensor) {
    charger_controller_temperature_sensor_ = charger_controller_temperature_sensor;
  }
  void set_pv_controller_temperature_sensor(sensor::Sensor *pv_controller_temperature_sensor) {
    pv_controller_temperature_sensor_ = pv_controller_temperature_sensor;
  }
  void set_pv_mode_setting_id_sensor(sensor::Sensor *pv_mode_setting_id_sensor) {
    pv_mode_setting_id_sensor_ = pv_mode_setting_id_sensor;
  }
  void set_battery_capacity_remaining_sensor(sensor::Sensor *battery_capacity_remaining_sensor) {
    battery_capacity_remaining_sensor_ = battery_capacity_remaining_sensor;
  }
  void set_battery_nominal_capacity_sensor(sensor::Sensor *battery_nominal_capacity_sensor) {
    battery_nominal_capacity_sensor_ = battery_nominal_capacity_sensor;
  }
  void set_charging_converter_battery_voltage_sensor(sensor::Sensor *charging_converter_battery_voltage_sensor) {
    charging_converter_battery_voltage_sensor_ = charging_converter_battery_voltage_sensor;
  }
  void set_charging_converter_secondary_battery_voltage_sensor(
      sensor::Sensor *charging_converter_secondary_battery_voltage_sensor) {
    charging_converter_secondary_battery_voltage_sensor_ = charging_converter_secondary_battery_voltage_sensor;
  }
  void set_charging_converter_current_sensor(sensor::Sensor *charging_converter_current_sensor) {
    charging_converter_current_sensor_ = charging_converter_current_sensor;
  }
  void set_charging_converter_power_sensor(sensor::Sensor *charging_converter_power_sensor) {
    charging_converter_power_sensor_ = charging_converter_power_sensor;
  }
  void set_charging_converter_load_sensor(sensor::Sensor *charging_converter_load_sensor) {
    charging_converter_load_sensor_ = charging_converter_load_sensor;
  }
  void set_charging_converter_controller_temperature_sensor(
      sensor::Sensor *charging_converter_controller_temperature_sensor) {
    charging_converter_controller_temperature_sensor_ = charging_converter_controller_temperature_sensor;
  }
  void set_charging_converter_mode_setting_id_sensor(sensor::Sensor *charging_converter_mode_setting_id_sensor) {
    charging_converter_mode_setting_id_sensor_ = charging_converter_mode_setting_id_sensor;
  }
  void set_charging_converter_battery_status_bitmask_sensor(
      sensor::Sensor *charging_converter_battery_status_bitmask_sensor) {
    charging_converter_battery_status_bitmask_sensor_ = charging_converter_battery_status_bitmask_sensor;
  }
  void set_charging_converter_controller_status_bitmask_sensor(
      sensor::Sensor *charging_converter_controller_status_bitmask_sensor) {
    charging_converter_controller_status_bitmask_sensor_ = charging_converter_controller_status_bitmask_sensor;
  }

  void set_battery_status_text_sensor(text_sensor::TextSensor *battery_status_text_sensor) {
    battery_status_text_sensor_ = battery_status_text_sensor;
  }
  void set_pv_mode_setting_text_sensor(text_sensor::TextSensor *pv_mode_setting_text_sensor) {
    pv_mode_setting_text_sensor_ = pv_mode_setting_text_sensor;
  }
  void set_pv_battery_status_text_sensor(text_sensor::TextSensor *pv_battery_status_text_sensor) {
    pv_battery_status_text_sensor_ = pv_battery_status_text_sensor;
  }
  void set_charger_controller_status_text_sensor(text_sensor::TextSensor *charger_controller_status_text_sensor) {
    charger_controller_status_text_sensor_ = charger_controller_status_text_sensor;
  }
  void set_pv_controller_status_text_sensor(text_sensor::TextSensor *pv_controller_status_text_sensor) {
    pv_controller_status_text_sensor_ = pv_controller_status_text_sensor;
  }
  void set_charger_mode_setting_text_sensor(text_sensor::TextSensor *charger_mode_setting_text_sensor) {
    charger_mode_setting_text_sensor_ = charger_mode_setting_text_sensor;
  }
  void set_charging_converter_mode_setting_text_sensor(
      text_sensor::TextSensor *charging_converter_mode_setting_text_sensor) {
    charging_converter_mode_setting_text_sensor_ = charging_converter_mode_setting_text_sensor;
  }
  void set_charging_converter_battery_status_text_sensor(
      text_sensor::TextSensor *charging_converter_battery_status_text_sensor) {
    charging_converter_battery_status_text_sensor_ = charging_converter_battery_status_text_sensor;
  }
  void set_charging_converter_controller_status_text_sensor(
      text_sensor::TextSensor *charging_converter_controller_status_text_sensor) {
    charging_converter_controller_status_text_sensor_ = charging_converter_controller_status_text_sensor;
  }

  void on_votronic_data(const std::vector<uint8_t> &data);
  void set_throttle(uint32_t throttle) { this->throttle_ = throttle; }
  void set_rx_timeout(uint16_t rx_timeout) { rx_timeout_ = rx_timeout; }

 protected:
  binary_sensor::BinarySensor *charging_binary_sensor_;
  binary_sensor::BinarySensor *discharging_binary_sensor_;

  binary_sensor::BinarySensor *charger_charging_binary_sensor_;
  binary_sensor::BinarySensor *charger_discharging_binary_sensor_;
  binary_sensor::BinarySensor *charger_controller_active_binary_sensor_;
  binary_sensor::BinarySensor *charger_aes_active_binary_sensor_;
  binary_sensor::BinarySensor *charger_current_reduction_binary_sensor_;

  binary_sensor::BinarySensor *charging_converter_charging_binary_sensor_;
  binary_sensor::BinarySensor *charging_converter_discharging_binary_sensor_;
  binary_sensor::BinarySensor *charging_converter_controller_active_binary_sensor_;
  binary_sensor::BinarySensor *charging_converter_current_reduction_binary_sensor_;
  binary_sensor::BinarySensor *charging_converter_aes_active_binary_sensor_;

  binary_sensor::BinarySensor *pv_controller_active_binary_sensor_;
  binary_sensor::BinarySensor *pv_aes_active_binary_sensor_;
  binary_sensor::BinarySensor *pv_current_reduction_binary_sensor_;

  sensor::Sensor *battery_voltage_sensor_;
  sensor::Sensor *secondary_battery_voltage_sensor_;
  sensor::Sensor *state_of_charge_sensor_;
  sensor::Sensor *current_sensor_;
  sensor::Sensor *power_sensor_;
  sensor::Sensor *battery_status_bitmask_sensor_;
  sensor::Sensor *battery_capacity_remaining_sensor_;
  sensor::Sensor *battery_nominal_capacity_sensor_;

  sensor::Sensor *charger_current_sensor_;
  sensor::Sensor *charger_power_sensor_;
  sensor::Sensor *charger_load_sensor_;
  sensor::Sensor *charger_controller_temperature_sensor_;
  sensor::Sensor *charger_mode_setting_id_sensor_;
  sensor::Sensor *charger_battery_status_bitmask_sensor_;
  sensor::Sensor *charger_controller_status_bitmask_sensor_;

  sensor::Sensor *charging_converter_battery_voltage_sensor_;
  sensor::Sensor *charging_converter_secondary_battery_voltage_sensor_;
  sensor::Sensor *charging_converter_current_sensor_;
  sensor::Sensor *charging_converter_power_sensor_;
  sensor::Sensor *charging_converter_load_sensor_;
  sensor::Sensor *charging_converter_controller_temperature_sensor_;
  sensor::Sensor *charging_converter_mode_setting_id_sensor_;
  sensor::Sensor *charging_converter_battery_status_bitmask_sensor_;
  sensor::Sensor *charging_converter_controller_status_bitmask_sensor_;

  sensor::Sensor *pv_voltage_sensor_;
  sensor::Sensor *pv_current_sensor_;
  sensor::Sensor *pv_power_sensor_;
  sensor::Sensor *pv_battery_status_bitmask_sensor_;
  sensor::Sensor *pv_controller_status_bitmask_sensor_;
  sensor::Sensor *pv_controller_temperature_sensor_;
  sensor::Sensor *pv_mode_setting_id_sensor_;

  text_sensor::TextSensor *battery_status_text_sensor_;

  text_sensor::TextSensor *charger_mode_setting_text_sensor_;
  text_sensor::TextSensor *charger_battery_status_text_sensor_;
  text_sensor::TextSensor *charger_controller_status_text_sensor_;

  text_sensor::TextSensor *charging_converter_mode_setting_text_sensor_;
  text_sensor::TextSensor *charging_converter_battery_status_text_sensor_;
  text_sensor::TextSensor *charging_converter_controller_status_text_sensor_;

  text_sensor::TextSensor *pv_mode_setting_text_sensor_;
  text_sensor::TextSensor *pv_battery_status_text_sensor_;
  text_sensor::TextSensor *pv_controller_status_text_sensor_;

  std::vector<uint8_t> rx_buffer_;
  uint32_t last_byte_{0};
  uint32_t last_solar_charger_data_{0};
  uint32_t last_charger_data_{0};
  uint32_t last_battery_computer_info1_data_{0};
  uint32_t last_battery_computer_info2_data_{0};
  uint32_t last_battery_computer_info3_data_{0};
  uint32_t throttle_;
  uint16_t rx_timeout_{150};

  void decode_solar_charger_data_(const std::vector<uint8_t> &data);
  void decode_charger_data_(const std::vector<uint8_t> &data);
  void decode_charging_converter_data_(const std::vector<uint8_t> &data);
  void decode_battery_computer_info1_data_(const std::vector<uint8_t> &data);
  void decode_battery_computer_info2_data_(const std::vector<uint8_t> &data);
  void decode_battery_computer_info3_data_(const std::vector<uint8_t> &data);
  bool parse_votronic_byte_(uint8_t byte);
  void publish_state_(binary_sensor::BinarySensor *binary_sensor, const bool &state);
  void publish_state_(sensor::Sensor *sensor, float value);
  void publish_state_(text_sensor::TextSensor *text_sensor, const std::string &state);

  std::string charging_mode_setting_to_string_(uint8_t charging_mode_setting);
  std::string battery_status_bitmask_to_string_(uint8_t mask);
  std::string solar_charger_status_bitmask_to_string_(uint8_t mask);
  std::string charger_status_bitmask_to_string_(uint8_t mask);

  uint8_t chksum_(const uint8_t data[], const uint8_t len) {
    uint8_t checksum = 0x00;
    for (uint8_t i = 0; i < len; i++) {
      checksum = checksum ^ data[i];
    }
    return checksum;
  }
};

}  // namespace votronic
}  // namespace esphome
