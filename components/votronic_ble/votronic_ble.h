#pragma once

#include "esphome/core/component.h"
#include "esphome/core/hal.h"
#include "esphome/components/ble_client/ble_client.h"
#include "esphome/components/esp32_ble_tracker/esp32_ble_tracker.h"
#include "esphome/components/binary_sensor/binary_sensor.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/text_sensor/text_sensor.h"

#ifdef USE_ESP32

#include <esp_gattc_api.h>

namespace esphome {
namespace votronic_ble {

namespace espbt = esphome::esp32_ble_tracker;

class VotronicBle : public esphome::ble_client::BLEClientNode, public PollingComponent {
 public:
  void gattc_event_handler(esp_gattc_cb_event_t event, esp_gatt_if_t gattc_if,
                           esp_ble_gattc_cb_param_t *param) override;
  void dump_config() override;
  void update() override;
  float get_setup_priority() const override { return setup_priority::DATA; }

  void set_charging_binary_sensor(binary_sensor::BinarySensor *charging_binary_sensor) {
    charging_binary_sensor_ = charging_binary_sensor;
  }
  void set_discharging_binary_sensor(binary_sensor::BinarySensor *discharging_binary_sensor) {
    discharging_binary_sensor_ = discharging_binary_sensor;
  }
  void set_controller_active_binary_sensor(binary_sensor::BinarySensor *controller_active_binary_sensor) {
    controller_active_binary_sensor_ = controller_active_binary_sensor;
  }
  void set_current_reduction_binary_sensor(binary_sensor::BinarySensor *current_reduction_binary_sensor) {
    current_reduction_binary_sensor_ = current_reduction_binary_sensor;
  }
  void set_aes_active_binary_sensor(binary_sensor::BinarySensor *aes_active_binary_sensor) {
    aes_active_binary_sensor_ = aes_active_binary_sensor;
  }

  void set_battery_voltage_sensor(sensor::Sensor *battery_voltage_sensor) {
    battery_voltage_sensor_ = battery_voltage_sensor;
  }
  void set_secondary_battery_voltage_sensor(sensor::Sensor *secondary_battery_voltage_sensor) {
    secondary_battery_voltage_sensor_ = secondary_battery_voltage_sensor;
  }
  void set_battery_capacity_remaining_sensor(sensor::Sensor *battery_capacity_remaining_sensor) {
    battery_capacity_remaining_sensor_ = battery_capacity_remaining_sensor;
  }
  void set_state_of_charge_sensor(sensor::Sensor *state_of_charge_sensor) {
    state_of_charge_sensor_ = state_of_charge_sensor;
  }
  void set_current_sensor(sensor::Sensor *current_sensor) { current_sensor_ = current_sensor; }
  void set_power_sensor(sensor::Sensor *power_sensor) { power_sensor_ = power_sensor; }
  void set_battery_nominal_capacity_sensor(sensor::Sensor *battery_nominal_capacity_sensor) {
    battery_nominal_capacity_sensor_ = battery_nominal_capacity_sensor;
  }
  void set_pv_voltage_sensor(sensor::Sensor *pv_voltage_sensor) { pv_voltage_sensor_ = pv_voltage_sensor; }
  void set_pv_current_sensor(sensor::Sensor *pv_current_sensor) { pv_current_sensor_ = pv_current_sensor; }
  void set_battery_status_bitmask_sensor(sensor::Sensor *battery_status_bitmask_sensor) {
    battery_status_bitmask_sensor_ = battery_status_bitmask_sensor;
  }
  void set_pv_controller_status_bitmask_sensor(sensor::Sensor *pv_controller_status_bitmask_sensor) {
    pv_controller_status_bitmask_sensor_ = pv_controller_status_bitmask_sensor;
  }
  void set_charged_capacity_sensor(sensor::Sensor *charged_capacity_sensor) {
    charged_capacity_sensor_ = charged_capacity_sensor;
  }
  void set_charged_energy_sensor(sensor::Sensor *charged_energy_sensor) {
    charged_energy_sensor_ = charged_energy_sensor;
  }
  void set_pv_power_sensor(sensor::Sensor *pv_power_sensor) { pv_power_sensor_ = pv_power_sensor; }

  void set_battery_status_text_sensor(text_sensor::TextSensor *battery_status_text_sensor) {
    battery_status_text_sensor_ = battery_status_text_sensor;
  }
  void set_pv_controller_status_text_sensor(text_sensor::TextSensor *pv_controller_status_text_sensor) {
    pv_controller_status_text_sensor_ = pv_controller_status_text_sensor;
  }

  void on_votronic_ble_data(const uint8_t &handle, const std::vector<uint8_t> &data);
  void set_throttle(uint32_t throttle) { this->throttle_ = throttle; }

 protected:
  binary_sensor::BinarySensor *charging_binary_sensor_;
  binary_sensor::BinarySensor *discharging_binary_sensor_;
  binary_sensor::BinarySensor *controller_active_binary_sensor_;
  binary_sensor::BinarySensor *current_reduction_binary_sensor_;
  binary_sensor::BinarySensor *aes_active_binary_sensor_;

  sensor::Sensor *battery_voltage_sensor_;
  sensor::Sensor *secondary_battery_voltage_sensor_;
  sensor::Sensor *battery_capacity_remaining_sensor_;
  sensor::Sensor *state_of_charge_sensor_;
  sensor::Sensor *current_sensor_;
  sensor::Sensor *power_sensor_;
  sensor::Sensor *battery_nominal_capacity_sensor_;
  sensor::Sensor *pv_voltage_sensor_;
  sensor::Sensor *pv_current_sensor_;
  sensor::Sensor *battery_status_bitmask_sensor_;
  sensor::Sensor *pv_controller_status_bitmask_sensor_;
  sensor::Sensor *charged_capacity_sensor_;
  sensor::Sensor *charged_energy_sensor_;
  sensor::Sensor *pv_power_sensor_;

  text_sensor::TextSensor *battery_status_text_sensor_;
  text_sensor::TextSensor *pv_controller_status_text_sensor_;

  uint16_t char_battery_computer_handle_{0x22};
  uint16_t char_solar_charger_handle_{0x25};
  uint32_t last_battery_computer_data_{0};
  uint32_t last_solar_charger_data_{0};
  uint32_t throttle_;

  esp32_ble_tracker::ESPBTUUID service_bond_uuid_ =
      esp32_ble_tracker::ESPBTUUID::from_raw("70521e61-022d-f899-d046-4885a76acbd0");
  esp32_ble_tracker::ESPBTUUID service_monitoring_uuid_ =
      esp32_ble_tracker::ESPBTUUID::from_raw("d0cb6aa7-8548-46d0-99f8-2d02611e5270");
  esp32_ble_tracker::ESPBTUUID service_log_data_uuid_ =
      esp32_ble_tracker::ESPBTUUID::from_raw("ae64a924-1184-4554-8bbc-295db9f2324a");

  esp32_ble_tracker::ESPBTUUID char_battery_computer_uuid_ =
      esp32_ble_tracker::ESPBTUUID::from_raw("9a082a4e-5bcc-4b1d-9958-a97cfccfa5ec");
  esp32_ble_tracker::ESPBTUUID char_solar_charger_uuid_ =
      esp32_ble_tracker::ESPBTUUID::from_raw("971ccec2-521d-42fd-b570-cf46fe5ceb65");

  esp32_ble_tracker::ESPBTUUID char_management_uuid_ =
      esp32_ble_tracker::ESPBTUUID::from_raw("ac12f485-cab7-4e0a-aac5-3585918852f6");
  esp32_ble_tracker::ESPBTUUID char_bulk_data_uuid_ =
      esp32_ble_tracker::ESPBTUUID::from_raw("b8a37ffe-c57b-4007-b3c1-ca05a6b7f0c6");

  void decode_solar_charger_data_(const std::vector<uint8_t> &data);
  void decode_battery_computer_data_(const std::vector<uint8_t> &data);
  void publish_state_(binary_sensor::BinarySensor *binary_sensor, const bool &state);
  void publish_state_(sensor::Sensor *sensor, float value);
  void publish_state_(text_sensor::TextSensor *text_sensor, const std::string &state);
  std::string battery_status_bitmask_to_string_(uint8_t mask);
  std::string solar_charger_status_bitmask_to_string_(uint8_t mask);
};

}  // namespace votronic_ble
}  // namespace esphome

#endif
