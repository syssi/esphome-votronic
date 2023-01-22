#include "votronic_ble.h"
#include "esphome/core/log.h"
#include "esphome/core/helpers.h"

namespace esphome {
namespace votronic_ble {

static const char *const TAG = "votronic_ble";

void VotronicBle::gattc_event_handler(esp_gattc_cb_event_t event, esp_gatt_if_t gattc_if,
                                      esp_ble_gattc_cb_param_t *param) {
  switch (event) {
    case ESP_GATTC_OPEN_EVT: {
      break;
    }
    case ESP_GATTC_CONNECT_EVT: {
      ESP_LOGD(TAG, "ESP_GATTC_CONNECT_EVT");
      esp_ble_set_encryption(param->connect.remote_bda, ESP_BLE_SEC_ENCRYPT_MITM);
      break;
    }
    case ESP_GATTC_DISCONNECT_EVT: {
      this->node_state = espbt::ClientState::IDLE;

      // this->publish_state_(this->voltage_sensor_, NAN);
      break;
    }
    case ESP_GATTC_SEARCH_CMPL_EVT: {
      auto *char_battery = this->parent_->get_characteristic(this->service_monitoring_uuid_, this->char_battery_uuid_);
      if (char_battery == nullptr) {
        ESP_LOGW(TAG, "[%s] No battery characteristic found at device, no battery computer attached?",
                 this->parent_->address_str().c_str());
        break;
      }
      this->char_battery_handle_ = char_battery->handle;

      auto status = esp_ble_gattc_register_for_notify(this->parent()->get_gattc_if(), this->parent()->get_remote_bda(),
                                                      char_battery->handle);
      if (status) {
        ESP_LOGW(TAG, "esp_ble_gattc_register_for_notify failed, status=%d", status);
      }

      auto *char_photovoltaic =
          this->parent_->get_characteristic(this->service_monitoring_uuid_, this->char_photovoltaic_uuid_);
      if (char_photovoltaic == nullptr) {
        ESP_LOGW(TAG, "[%s] No Photovoltaic characteristic found at device, no Photovoltaic solar charger attached?",
                 this->parent_->address_str().c_str());
        break;
      }
      this->char_photovoltaic_handle_ = char_photovoltaic->handle;

      auto status2 = esp_ble_gattc_register_for_notify(this->parent()->get_gattc_if(), this->parent()->get_remote_bda(),
                                                       char_photovoltaic->handle);
      if (status2) {
        ESP_LOGW(TAG, "esp_ble_gattc_register_for_notify failed, status=%d", status2);
      }

      break;
    }
    case ESP_GATTC_REG_FOR_NOTIFY_EVT: {
      this->node_state = espbt::ClientState::ESTABLISHED;
      break;
    }
    case ESP_GATTC_NOTIFY_EVT: {
      ESP_LOGVV(TAG, "Notification received (handle %d): %s", param->notify.handle,
                format_hex_pretty(param->notify.value, param->notify.value_len).c_str());

      std::vector<uint8_t> data(param->notify.value, param->notify.value + param->notify.value_len);

      this->on_votronic_ble_data_(param->notify.handle, data);
      break;
    }
    default:
      break;
  }
}

void VotronicBle::update() {
  if (this->enable_fake_traffic_) {
    this->char_photovoltaic_handle_ = 0x25;
    this->char_battery_handle_ = 0x22;

    // Photovoltaic status frame
    this->on_votronic_ble_data_(0x25, {0xE8, 0x04, 0x76, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x56, 0x00, 0x09,
                                       0x18, 0x00, 0x22, 0x00, 0x00, 0x00});

    // Battery status frame
    this->on_votronic_ble_data_(0x22, {0xE8, 0x04, 0xBF, 0x04, 0x09, 0x01, 0x60, 0x00, 0x5F, 0x00,
                                       0x9A, 0xFE, 0xFF, 0xF0, 0x0A, 0x5E, 0x14, 0x54, 0x02, 0x04});
  }

  if (this->node_state != espbt::ClientState::ESTABLISHED) {
    ESP_LOGW(TAG, "[%s] Not connected", this->parent_->address_str().c_str());
    return;
  }
}

void VotronicBle::on_votronic_ble_data_(const uint8_t &handle, const std::vector<uint8_t> &data) {
  if (handle == this->char_photovoltaic_handle_) {
    this->decode_photovoltaic_data_(data);
    return;
  }

  if (handle == this->char_battery_handle_) {
    this->decode_battery_data_(data);
    return;
  }

  ESP_LOGW(TAG, "Unhandled frame received: %s", format_hex_pretty(&data.front(), data.size()).c_str());
}

void VotronicBle::decode_photovoltaic_data_(const std::vector<uint8_t> &data) {
  if (data.size() != 19) {
    ESP_LOGW(TAG, "Invalid frame size: %zu", data.size());
    return;
  }

  const uint32_t now = millis();
  if (now - this->last_photovoltaic_info_ < this->throttle_) {
    return;
  }
  this->last_photovoltaic_info_ = now;

  auto votronic_get_16bit = [&](size_t i) -> uint16_t {
    return (uint16_t(data[i + 1]) << 8) | (uint16_t(data[i + 0]) << 0);
  };

  ESP_LOGI(TAG, "Photovoltaic data frame received");
  ESP_LOGI(TAG, "  Primary battery voltage: %f V", votronic_get_16bit(0) * 0.01f);
  ESP_LOGI(TAG, "  PV voltage: %f V", votronic_get_16bit(2) * 0.01f);
  ESP_LOGI(TAG, "  PV current: %f A", votronic_get_16bit(4) * 0.01f);
  ESP_LOGD(TAG, "  Unknown (Byte 6): %d (0x%02X)", data[6], data[6]);
  ESP_LOGD(TAG, "  Unknown (Byte 7): %d (0x%02X)", data[7], data[7]);
  ESP_LOGI(TAG, "  Battery status bitmask: %d (0x%02X)", data[8], data[8]);
  ESP_LOGI(TAG, "  Controller status bitmask: %d (0?: Active, 1?: Standby, 2?: Reduce)", data[9]);
  ESP_LOGD(TAG, "  Unknown (Byte 10): %d (0x%02X)", data[10], data[10]);
  ESP_LOGD(TAG, "  Unknown (Byte 11): %d (0x%02X)", data[11], data[11]);
  // Bit0: Standby
  // Bit1: Active
  // Bit2: Reduce
  ESP_LOGD(TAG, "  Controller status bitmask? (Byte 12): %d (0x%02X) (9: Active, 25?: Standby, 2?: Reduce)", data[12],
           data[12]);
  ESP_LOGI(TAG, "  Charged capacity: %d Ah", votronic_get_16bit(13));
  ESP_LOGI(TAG, "  Charged energy: %d Wh", votronic_get_16bit(15) * 10);
  ESP_LOGD(TAG, "  PV power? (Byte 16): %d W? (0x%02X)", data[16], data[16]);
  ESP_LOGD(TAG, "  PV power? (Byte 17): %d W? (0x%02X)", data[17], data[17]);
  ESP_LOGD(TAG, "  PV power? (Byte 18): %d W? (0x%02X)", data[18], data[18]);
}

void VotronicBle::decode_battery_data_(const std::vector<uint8_t> &data) {
  if (data.size() != 20) {
    ESP_LOGW(TAG, "Invalid frame size: %zu", data.size());
    return;
  }

  const uint32_t now = millis();
  if (now - this->last_battery_info_ < this->throttle_) {
    return;
  }
  this->last_battery_info_ = now;

  this->last_photovoltaic_info_ = now;
  auto votronic_get_16bit = [&](size_t i) -> uint16_t {
    return (uint16_t(data[i + 1]) << 8) | (uint16_t(data[i + 0]) << 0);
  };

  ESP_LOGI(TAG, "Battery data frame received");
  ESP_LOGI(TAG, "  Primary battery voltage: %f V", votronic_get_16bit(0) * 0.01f);
  ESP_LOGI(TAG, "  Secondary battery voltage: %f V", votronic_get_16bit(2) * 0.01f);
  ESP_LOGI(TAG, "  Primary battery capacity: %d Ah", votronic_get_16bit(4));
  ESP_LOGD(TAG, "  Unknown (Byte 6): %d (0x%02X)", data[6], data[6]);
  ESP_LOGD(TAG, "  Unknown (Byte 7): %d (0x%02X)", data[7], data[7]);
  ESP_LOGI(TAG, "  State of charge: %d %%", data[8]);
  ESP_LOGD(TAG, "  Unknown (Byte 9): %d (0x%02X)", data[9], data[9]);
  ESP_LOGI(TAG, "  Current: %f A", (float) ((int16_t) votronic_get_16bit(10)) * 0.001f);
  ESP_LOGD(TAG, "  Unknown (Byte 12): %d (0x%02X)", data[12], data[12]);
  ESP_LOGI(TAG, "  Primary battery nominal capacity: %f Ah", votronic_get_16bit(13) * 0.1f);
  ESP_LOGD(TAG, "  Unknown (Byte 15): %d (0x%02X)", data[15], data[15]);
  ESP_LOGD(TAG, "  Unknown (Byte 16-17): %d (0x%02X 0x%02X)", votronic_get_16bit(16), data[16], data[17]);
  ESP_LOGD(TAG, "  Unknown (Byte 18-19): %d (0x%02X 0x%02X)", votronic_get_16bit(18), data[18], data[19]);
}

void VotronicBle::dump_config() {
  ESP_LOGCONFIG(TAG, "VotronicBle:");
  ESP_LOGCONFIG(TAG, "  MAC address                      : %s", this->parent_->address_str().c_str());
  ESP_LOGCONFIG(TAG, "  Monitoring Service UUID          : %s", this->service_monitoring_uuid_.to_string().c_str());
  ESP_LOGCONFIG(TAG, "  Battery Characteristic UUID      : %s", this->char_battery_uuid_.to_string().c_str());
  ESP_LOGCONFIG(TAG, "  Photovoltaic Characteristic UUID : %s", this->char_photovoltaic_uuid_.to_string().c_str());
  ESP_LOGCONFIG(TAG, "  Fake traffic enabled: %s", YESNO(this->enable_fake_traffic_));

  LOG_SENSOR("", "Total voltage", this->total_voltage_sensor_);
  LOG_SENSOR("", "Current", this->current_sensor_);
  LOG_SENSOR("", "Power", this->power_sensor_);
}

void VotronicBle::publish_state_(sensor::Sensor *sensor, float value) {
  if (sensor == nullptr)
    return;

  sensor->publish_state(value);
}

void VotronicBle::publish_state_(text_sensor::TextSensor *text_sensor, const std::string &state) {
  if (text_sensor == nullptr)
    return;

  text_sensor->publish_state(state);
}

}  // namespace votronic_ble
}  // namespace esphome
