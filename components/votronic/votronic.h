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
  void set_controller_active_binary_sensor(binary_sensor::BinarySensor *controller_active_binary_sensor) {
    controller_active_binary_sensor_ = controller_active_binary_sensor;
  }
  void set_aes_active_binary_sensor(binary_sensor::BinarySensor *aes_active_binary_sensor) {
    aes_active_binary_sensor_ = aes_active_binary_sensor;
  }
  void set_current_reduction_binary_sensor(binary_sensor::BinarySensor *current_reduction_binary_sensor) {
    current_reduction_binary_sensor_ = current_reduction_binary_sensor;
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
  void set_pv_voltage_sensor(sensor::Sensor *pv_voltage_sensor) { pv_voltage_sensor_ = pv_voltage_sensor; }
  void set_pv_current_sensor(sensor::Sensor *pv_current_sensor) { pv_current_sensor_ = pv_current_sensor; }
  void set_pv_power_sensor(sensor::Sensor *pv_power_sensor) { pv_power_sensor_ = pv_power_sensor; }
  void set_battery_status_bitmask_sensor(sensor::Sensor *battery_status_bitmask_sensor) {
    battery_status_bitmask_sensor_ = battery_status_bitmask_sensor;
  }
  void set_charging_controller_status_bitmask_sensor(sensor::Sensor *charging_controller_status_bitmask_sensor) {
    charging_controller_status_bitmask_sensor_ = charging_controller_status_bitmask_sensor;
  }
  void set_pv_controller_status_bitmask_sensor(sensor::Sensor *pv_controller_status_bitmask_sensor) {
    pv_controller_status_bitmask_sensor_ = pv_controller_status_bitmask_sensor;
  }
  void set_charging_mode_setting_id_sensor(sensor::Sensor *charging_mode_setting_id_sensor) {
    charging_mode_setting_id_sensor_ = charging_mode_setting_id_sensor;
  }
  void set_controller_temperature_sensor(sensor::Sensor *controller_temperature_sensor) {
    controller_temperature_sensor_ = controller_temperature_sensor;
  }

  void set_battery_status_text_sensor(text_sensor::TextSensor *battery_status_text_sensor) {
    battery_status_text_sensor_ = battery_status_text_sensor;
  }
  void set_charging_controller_status_text_sensor(text_sensor::TextSensor *charging_controller_status_text_sensor) {
    charging_controller_status_text_sensor_ = charging_controller_status_text_sensor;
  }
  void set_pv_controller_status_text_sensor(text_sensor::TextSensor *pv_controller_status_text_sensor) {
    pv_controller_status_text_sensor_ = pv_controller_status_text_sensor;
  }
  void set_charging_mode_setting_text_sensor(text_sensor::TextSensor *charging_mode_setting_text_sensor) {
    charging_mode_setting_text_sensor_ = charging_mode_setting_text_sensor;
  }

  void on_votronic_data(const std::vector<uint8_t> &data);
  void set_throttle(uint16_t throttle) { this->throttle_ = throttle; }
  void set_rx_timeout(uint16_t rx_timeout) { rx_timeout_ = rx_timeout; }

 protected:
  binary_sensor::BinarySensor *charging_binary_sensor_;
  binary_sensor::BinarySensor *discharging_binary_sensor_;
  binary_sensor::BinarySensor *controller_active_binary_sensor_;
  binary_sensor::BinarySensor *aes_active_binary_sensor_;
  binary_sensor::BinarySensor *current_reduction_binary_sensor_;

  sensor::Sensor *battery_voltage_sensor_;
  sensor::Sensor *secondary_battery_voltage_sensor_;
  sensor::Sensor *state_of_charge_sensor_;
  sensor::Sensor *current_sensor_;
  sensor::Sensor *power_sensor_;
  sensor::Sensor *pv_voltage_sensor_;
  sensor::Sensor *pv_current_sensor_;
  sensor::Sensor *pv_power_sensor_;
  sensor::Sensor *battery_status_bitmask_sensor_;
  sensor::Sensor *charging_controller_status_bitmask_sensor_;
  sensor::Sensor *pv_controller_status_bitmask_sensor_;
  sensor::Sensor *charging_mode_setting_id_sensor_;
  sensor::Sensor *controller_temperature_sensor_;

  text_sensor::TextSensor *battery_status_text_sensor_;
  text_sensor::TextSensor *charging_controller_status_text_sensor_;
  text_sensor::TextSensor *pv_controller_status_text_sensor_;
  text_sensor::TextSensor *charging_mode_setting_text_sensor_;

  std::vector<uint8_t> rx_buffer_;
  uint32_t last_byte_{0};
  uint32_t last_solar_charger_data_{0};
  uint32_t last_charger_data_{0};
  uint32_t last_battery_computer_info1_data_{0};
  uint32_t last_battery_computer_info2_data_{0};
  uint32_t last_battery_computer_info3_data_{0};
  uint16_t throttle_;
  uint16_t rx_timeout_{150};

  void decode_solar_charger_data_(const std::vector<uint8_t> &data);
  void decode_charger_data_(const uint8_t &frame_type, const std::vector<uint8_t> &data);
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
