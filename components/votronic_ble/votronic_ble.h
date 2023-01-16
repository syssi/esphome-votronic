#pragma once

#include "esphome/core/component.h"
#include "esphome/core/hal.h"
#include "esphome/components/ble_client/ble_client.h"
#include "esphome/components/esp32_ble_tracker/esp32_ble_tracker.h"
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

  void set_total_voltage_sensor(sensor::Sensor *total_voltage_sensor) { total_voltage_sensor_ = total_voltage_sensor; }
  void set_current_sensor(sensor::Sensor *current_sensor) { current_sensor_ = current_sensor; }
  void set_power_sensor(sensor::Sensor *power_sensor) { power_sensor_ = power_sensor; }

  void set_operation_status_text_sensor(text_sensor::TextSensor *operation_status_text_sensor) {
    operation_status_text_sensor_ = operation_status_text_sensor;
  }
  void set_errors_text_sensor(text_sensor::TextSensor *errors_text_sensor) { errors_text_sensor_ = errors_text_sensor; }
  void set_throttle(uint16_t throttle) { this->throttle_ = throttle; }
  void set_enable_fake_traffic(bool enable_fake_traffic) { enable_fake_traffic_ = enable_fake_traffic; }

 protected:
  sensor::Sensor *total_voltage_sensor_;
  sensor::Sensor *current_sensor_;
  sensor::Sensor *power_sensor_;

  text_sensor::TextSensor *operation_status_text_sensor_;
  text_sensor::TextSensor *errors_text_sensor_;

  uint16_t char_battery_handle_;
  uint16_t char_photovoltaic_handle_;
  uint32_t last_battery_info_{0};
  uint32_t last_photovoltaic_info_{0};
  uint16_t throttle_;
  bool enable_fake_traffic_;

  esp32_ble_tracker::ESPBTUUID service_bond_uuid_ =
      esp32_ble_tracker::ESPBTUUID::from_raw("70521e61-022d-f899-d046-4885a76acbd0");
  esp32_ble_tracker::ESPBTUUID service_monitoring_uuid_ =
      esp32_ble_tracker::ESPBTUUID::from_raw("d0cb6aa7-8548-46d0-99f8-2d02611e5270");
  esp32_ble_tracker::ESPBTUUID service_log_data_uuid_ =
      esp32_ble_tracker::ESPBTUUID::from_raw("ae64a924-1184-4554-8bbc-295db9f2324a");

  esp32_ble_tracker::ESPBTUUID char_battery_uuid_ =
      esp32_ble_tracker::ESPBTUUID::from_raw("9a082a4e-5bcc-4b1d-9958-a97cfccfa5ec");
  esp32_ble_tracker::ESPBTUUID char_photovoltaic_uuid_ =
      esp32_ble_tracker::ESPBTUUID::from_raw("971ccec2-521d-42fd-b570-cf46fe5ceb65");

  esp32_ble_tracker::ESPBTUUID char_management_uuid_ =
      esp32_ble_tracker::ESPBTUUID::from_raw("ac12f485-cab7-4e0a-aac5-3585918852f6");
  esp32_ble_tracker::ESPBTUUID char_bulk_data_uuid_ =
      esp32_ble_tracker::ESPBTUUID::from_raw("b8a37ffe-c57b-4007-b3c1-ca05a6b7f0c6");

  void on_votronic_ble_data_(const uint8_t &handle, const std::vector<uint8_t> &data);
  void decode_photovoltaic_data_(const std::vector<uint8_t> &data);
  void decode_battery_data_(const std::vector<uint8_t> &data);
  void publish_state_(sensor::Sensor *sensor, float value);
  void publish_state_(text_sensor::TextSensor *text_sensor, const std::string &state);
};

}  // namespace votronic_ble
}  // namespace esphome

#endif
