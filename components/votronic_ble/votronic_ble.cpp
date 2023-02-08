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

      this->publish_state_(this->battery_voltage_sensor_, NAN);
      this->publish_state_(this->secondary_battery_voltage_sensor_, NAN);
      this->publish_state_(this->battery_capacity_sensor_, NAN);
      this->publish_state_(this->state_of_charge_sensor_, NAN);
      this->publish_state_(this->current_sensor_, NAN);
      this->publish_state_(this->battery_nominal_capacity_sensor_, NAN);
      this->publish_state_(this->battery_voltage_sensor_, NAN);
      this->publish_state_(this->pv_voltage_sensor_, NAN);
      this->publish_state_(this->pv_current_sensor_, NAN);
      this->publish_state_(this->battery_status_bitmask_sensor_, NAN);
      this->publish_state_(this->pv_controller_status_bitmask_sensor_, NAN);
      this->publish_state_(this->charged_capacity_sensor_, NAN);
      this->publish_state_(this->charged_energy_sensor_, NAN);
      this->publish_state_(this->pv_power_sensor_, NAN);

      break;
    }
    case ESP_GATTC_SEARCH_CMPL_EVT: {
      auto *char_battery_computer =
          this->parent_->get_characteristic(this->service_monitoring_uuid_, this->char_battery_computer_uuid_);
      if (char_battery_computer == nullptr) {
        ESP_LOGW(TAG, "[%s] No battery computer characteristic found at device. No battery computer attached?",
                 this->parent_->address_str().c_str());
        break;
      }
      this->char_battery_computer_handle_ = char_battery_computer->handle;

      auto status = esp_ble_gattc_register_for_notify(this->parent()->get_gattc_if(), this->parent()->get_remote_bda(),
                                                      char_battery_computer->handle);
      if (status) {
        ESP_LOGW(TAG, "esp_ble_gattc_register_for_notify failed, status=%d", status);
      }

      auto *char_solar_charger =
          this->parent_->get_characteristic(this->service_monitoring_uuid_, this->char_solar_charger_uuid_);
      if (char_solar_charger == nullptr) {
        ESP_LOGW(TAG, "[%s] No solar charger characteristic found at device. No solar charger attached?",
                 this->parent_->address_str().c_str());
        break;
      }
      this->char_solar_charger_handle_ = char_solar_charger->handle;

      auto status2 = esp_ble_gattc_register_for_notify(this->parent()->get_gattc_if(), this->parent()->get_remote_bda(),
                                                       char_solar_charger->handle);
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
    this->char_solar_charger_handle_ = 0x25;
    this->char_battery_computer_handle_ = 0x22;

    // Solar charger status frame
    this->on_votronic_ble_data_(0x25, {0xE8, 0x04, 0x76, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x56, 0x00, 0x09,
                                       0x18, 0x00, 0x22, 0x00, 0x00, 0x00});

    // Battery computer status frame
    this->on_votronic_ble_data_(0x22, {0xE8, 0x04, 0xBF, 0x04, 0x09, 0x01, 0x60, 0x00, 0x5F, 0x00,
                                       0x9A, 0xFE, 0xFF, 0xF0, 0x0A, 0x5E, 0x14, 0x54, 0x02, 0x04});

    // Battery computer status frame (max current: +8388.607 A)
    this->on_votronic_ble_data_(0x22, {0xE8, 0x04, 0xBF, 0x04, 0x09, 0x01, 0x60, 0x00, 0x5F, 0x00,
                                       0xFF, 0xFF, 0x7F, 0xF0, 0x0A, 0x5E, 0x14, 0x54, 0x02, 0x04});
    // Battery computer status frame (min current: -8388.608 A)
    this->on_votronic_ble_data_(0x22, {0xE8, 0x04, 0xBF, 0x04, 0x09, 0x01, 0x60, 0x00, 0x5F, 0x00,
                                       0x00, 0x00, 0x80, 0xF0, 0x0A, 0x5E, 0x14, 0x54, 0x02, 0x04});
  }

  if (this->node_state != espbt::ClientState::ESTABLISHED) {
    ESP_LOGW(TAG, "[%s] Not connected", this->parent_->address_str().c_str());
    return;
  }
}

void VotronicBle::on_votronic_ble_data_(const uint8_t &handle, const std::vector<uint8_t> &data) {
  if (handle == this->char_solar_charger_handle_) {
    this->decode_solar_charger_data_(data);
    return;
  }

  if (handle == this->char_battery_computer_handle_) {
    this->decode_battery_computer_data_(data);
    return;
  }

  ESP_LOGW(TAG, "Your device is probably not supported. Please create an issue here: "
                "https://github.com/syssi/esphome-votronic/issues");
  ESP_LOGW(TAG, "Please provide the following unhandled message data: %s",
           format_hex_pretty(&data.front(), data.size()).c_str());
}

void VotronicBle::decode_battery_computer_data_(const std::vector<uint8_t> &data) {
  if (data.size() != 20) {
    ESP_LOGW(TAG, "Invalid frame size: %zu", data.size());
    return;
  }

  const uint32_t now = millis();
  if (now - this->last_battery_computer_data_ < this->throttle_) {
    return;
  }
  this->last_battery_computer_data_ = now;

  auto votronic_get_16bit = [&](size_t i) -> uint16_t {
    return (uint16_t(data[i + 1]) << 8) | (uint16_t(data[i + 0]) << 0);
  };

  auto votronic_get_24bit = [&](size_t i) -> uint32_t {
    uint32_t val = (uint32_t(data[i + 2]) << 16) | (uint32_t(data[i + 1]) << 8) | (uint32_t(data[i + 0]) << 0);
    if (val & 0x800000) {
      val |= ~0xffffff;
    }
    return val;
  };

  ESP_LOGI(TAG, "Battery computer data received");
  ESP_LOGD(TAG, "  Unknown (Byte     6): %d (0x%02X)", data[6], data[6]);
  ESP_LOGD(TAG, "  Unknown (Byte     7): %d (0x%02X)", data[7], data[7]);
  ESP_LOGD(TAG, "  Unknown (Byte     9): %d (0x%02X)", data[9], data[9]);
  ESP_LOGD(TAG, "  Unknown (Byte    15): %d (0x%02X)", data[15], data[15]);
  ESP_LOGD(TAG, "  Unknown (Byte 16-17): %d (0x%02X 0x%02X)", votronic_get_16bit(16), data[16], data[17]);
  ESP_LOGD(TAG, "  Unknown (Byte 18-19): %d (0x%02X 0x%02X)", votronic_get_16bit(18), data[18], data[19]);

  this->publish_state_(this->battery_voltage_sensor_, votronic_get_16bit(0) * 0.01f);
  this->publish_state_(this->secondary_battery_voltage_sensor_, votronic_get_16bit(2) * 0.01f);
  this->publish_state_(this->battery_capacity_sensor_, (float) votronic_get_16bit(4));
  this->publish_state_(this->state_of_charge_sensor_, (float) data[8]);

  float current = (float) ((int32_t) votronic_get_24bit(10)) * 0.001f;
  ESP_LOGV(TAG, "  Current (raw): 0x%02X%02X%02X (%.3f A)", data[12], data[11], data[10], current);
  this->publish_state_(this->current_sensor_, current);
  this->publish_state_(this->charging_binary_sensor_, (current > 0.0f));
  this->publish_state_(this->discharging_binary_sensor_, (current < 0.0f));

  this->publish_state_(this->battery_nominal_capacity_sensor_, votronic_get_16bit(13) * 0.1f);
}

void VotronicBle::decode_solar_charger_data_(const std::vector<uint8_t> &data) {
  if (data.size() != 19) {
    ESP_LOGW(TAG, "Invalid frame size: %zu", data.size());
    return;
  }

  const uint32_t now = millis();
  if (now - this->last_solar_charger_data_ < this->throttle_) {
    return;
  }
  this->last_solar_charger_data_ = now;

  auto votronic_get_16bit = [&](size_t i) -> uint16_t {
    return (uint16_t(data[i + 1]) << 8) | (uint16_t(data[i + 0]) << 0);
  };

  ESP_LOGI(TAG, "Solar charger data received");
  this->publish_state_(this->battery_voltage_sensor_, votronic_get_16bit(0) * 0.01f);
  this->publish_state_(this->pv_voltage_sensor_, votronic_get_16bit(2) * 0.01f);
  this->publish_state_(this->pv_current_sensor_, votronic_get_16bit(4) * 0.1f);
  this->publish_state_(this->battery_status_bitmask_sensor_, data[8]);
  this->publish_state_(this->battery_status_text_sensor_, this->battery_status_to_string_(data[8]));
  this->publish_state_(this->pv_controller_status_bitmask_sensor_, data[12]);
  this->publish_state_(this->pv_controller_status_text_sensor_, this->pv_controller_status_to_string_(data[12]));
  this->publish_state_(this->charged_capacity_sensor_, (float) votronic_get_16bit(13));
  this->publish_state_(this->charged_energy_sensor_, votronic_get_16bit(15) * 10.0f);
  this->publish_state_(this->pv_power_sensor_, (float) votronic_get_16bit(17) * 0.1f);

  ESP_LOGD(TAG, "  Unknown (Byte  6): %d (0x%02X)", data[6], data[6]);
  ESP_LOGD(TAG, "  Unknown (Byte  7): %d (0x%02X)", data[7], data[7]);
  ESP_LOGD(TAG, "  Battery status bitmask: %d (0x%02X)", data[8], data[8]);
  ESP_LOGD(TAG, "  Unknown (Byte  9): %d (0x%02X)", data[9], data[9]);
  ESP_LOGD(TAG, "  Unknown (Byte 10): %d (0x%02X)", data[10], data[10]);
  ESP_LOGD(TAG, "  Unknown (Byte 11): %d (0x%02X)", data[11], data[11]);
  ESP_LOGD(TAG, "  Unknown (Byte 16): %d (0x%02X)", data[16], data[16]);
  ESP_LOGD(TAG, "  Unknown (Byte 17): %d (0x%02X)", data[17], data[17]);
  ESP_LOGD(TAG, "  Unknown (Byte 18): %d (0x%02X)", data[18], data[18]);
}

void VotronicBle::dump_config() {
  ESP_LOGCONFIG(TAG, "VotronicBle:");
  ESP_LOGCONFIG(TAG, "  MAC address                         : %s", this->parent_->address_str().c_str());
  ESP_LOGCONFIG(TAG, "  Monitoring Service UUID             : %s", this->service_monitoring_uuid_.to_string().c_str());
  ESP_LOGCONFIG(TAG, "  Battery Computer Characteristic UUID: %s",
                this->char_battery_computer_uuid_.to_string().c_str());
  ESP_LOGCONFIG(TAG, "  Solar Charger Characteristic UUID   : %s", this->char_solar_charger_uuid_.to_string().c_str());
  ESP_LOGCONFIG(TAG, "  Fake traffic enabled: %s", YESNO(this->enable_fake_traffic_));

  LOG_BINARY_SENSOR("", "Charging", this->charging_binary_sensor_);
  LOG_BINARY_SENSOR("", "Discharging", this->discharging_binary_sensor_);

  LOG_SENSOR("", "Battery voltage", this->battery_voltage_sensor_);
  LOG_SENSOR("", "Secondary battery voltage", this->secondary_battery_voltage_sensor_);
  LOG_SENSOR("", "Battery capacity", this->battery_capacity_sensor_);
  LOG_SENSOR("", "State of charge", this->state_of_charge_sensor_);
  LOG_SENSOR("", "Current", this->current_sensor_);
  LOG_SENSOR("", "Battery nominal capacity", this->battery_nominal_capacity_sensor_);
  LOG_SENSOR("", "PV voltage", this->pv_voltage_sensor_);
  LOG_SENSOR("", "PV current", this->pv_current_sensor_);
  LOG_SENSOR("", "PV power", this->pv_power_sensor_);
  LOG_SENSOR("", "Battery status bitmask", this->battery_status_bitmask_sensor_);
  LOG_SENSOR("", "PV controller status bitmask", this->pv_controller_status_bitmask_sensor_);
  LOG_SENSOR("", "Charged capacity", this->charged_capacity_sensor_);
  LOG_SENSOR("", "Charged energy", this->charged_energy_sensor_);

  LOG_TEXT_SENSOR("", "Battery status", this->battery_status_text_sensor_);
  LOG_TEXT_SENSOR("", "PV controller status", this->pv_controller_status_text_sensor_);
}

void VotronicBle::publish_state_(binary_sensor::BinarySensor *binary_sensor, const bool &state) {
  if (binary_sensor == nullptr)
    return;

  binary_sensor->publish_state(state);
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

std::string VotronicBle::pv_controller_status_to_string_(const uint8_t mask) {
  if ((mask & 24) == 24) {
    return "Current reduction";
  }

  if ((mask & 8) == 8) {
    return "Active";
  }

  if ((mask & 33) == 33) {
    return "Standby";
  }

  return str_snprintf("Unknown (0x%02X)", 15, mask);
}

std::string VotronicBle::battery_status_to_string_(const uint8_t mask) {
  if ((mask & 25) == 25) {
    return "Current reduction";
  }

  if ((mask & 32) == 32) {
    return "Active";
  }

  if ((mask & 33) == 33) {
    return "Standby";
  }

  return str_snprintf("Unknown (0x%02X)", 15, mask);
}

}  // namespace votronic_ble
}  // namespace esphome
