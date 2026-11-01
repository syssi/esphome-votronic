#include "votronic_ble.h"
#include "esphome/core/log.h"
#include "esphome/core/helpers.h"

// Fallback for ESPHome < 2026.10.0
#ifndef ESPHOME_LOG_TAG
#define ESPHOME_LOG_TAG(name, tag) static const char *const name = tag
#endif

namespace esphome::votronic_ble {

ESPHOME_LOG_TAG(TAG, "votronic_ble");

static constexpr size_t MAX_HEX_DUMP_BYTES = 100;

#ifdef USE_ESP32

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
      this->pending_notify_registrations_ = 0;

      this->publish_state_(this->battery_voltage_sensor_, NAN);
      this->publish_state_(this->secondary_battery_voltage_sensor_, NAN);
      this->publish_state_(this->battery_capacity_remaining_sensor_, NAN);
      this->publish_state_(this->state_of_charge_sensor_, NAN);
      this->publish_state_(this->current_sensor_, NAN);
      this->publish_state_(this->power_sensor_, NAN);
      this->publish_state_(this->battery_nominal_capacity_sensor_, NAN);
      this->publish_state_(this->battery_voltage_sensor_, NAN);
      this->publish_state_(this->pv_voltage_sensor_, NAN);
      this->publish_state_(this->pv_current_sensor_, NAN);
      this->publish_state_(this->pv_controller_temperature_sensor_, NAN);
      this->publish_state_(this->battery_status_bitmask_sensor_, NAN);
      this->publish_state_(this->pv_controller_status_bitmask_sensor_, NAN);
      this->publish_state_(this->charged_capacity_sensor_, NAN);
      this->publish_state_(this->charged_energy_sensor_, NAN);
      this->publish_state_(this->pv_power_sensor_, NAN);

      if (this->char_battery_computer_handle_ != 0) {
        auto status = esp_ble_gattc_unregister_for_notify(
            this->parent()->get_gattc_if(), this->parent()->get_remote_bda(), this->char_battery_computer_handle_);
        if (status) {
          ESP_LOGW(TAG, "esp_ble_gattc_unregister_for_notify failed, status=%d", status);
        }
      }
      this->char_battery_computer_handle_ = 0;

      if (this->char_solar_charger_handle_ != 0) {
        auto status = esp_ble_gattc_unregister_for_notify(
            this->parent()->get_gattc_if(), this->parent()->get_remote_bda(), this->char_solar_charger_handle_);
        if (status) {
          ESP_LOGW(TAG, "esp_ble_gattc_unregister_for_notify failed, status=%d", status);
        }
      }
      this->char_solar_charger_handle_ = 0;

      if (this->char_bulk_data_handle_ != 0) {
        auto status = esp_ble_gattc_unregister_for_notify(
            this->parent()->get_gattc_if(), this->parent()->get_remote_bda(), this->char_bulk_data_handle_);
        if (status) {
          ESP_LOGW(TAG, "esp_ble_gattc_unregister_for_notify failed, status=%d", status);
        }
      }
      this->char_bulk_data_handle_ = 0;
      this->char_management_handle_ = 0;
      this->bulk_data_receiving_ = false;

      break;
    }
    case ESP_GATTC_SEARCH_CMPL_EVT: {
      this->char_battery_computer_handle_ = 0;
      this->char_solar_charger_handle_ = 0;
      this->char_management_handle_ = 0;
      this->char_bulk_data_handle_ = 0;
      this->pending_notify_registrations_ = 0;

      auto *char_battery_computer =
          this->parent_->get_characteristic(this->service_monitoring_uuid_, this->char_battery_computer_uuid_);
      if (char_battery_computer == nullptr) {
        ESP_LOGW(TAG, "[%s] No battery computer characteristic found at device. No battery computer attached?",
                 this->parent_->address_str());
      } else {
        this->char_battery_computer_handle_ = char_battery_computer->handle;
        auto status = esp_ble_gattc_register_for_notify(
            this->parent()->get_gattc_if(), this->parent()->get_remote_bda(), char_battery_computer->handle);
        if (status) {
          ESP_LOGW(TAG, "esp_ble_gattc_register_for_notify failed, status=%d", status);
        } else {
          this->pending_notify_registrations_++;
        }
      }

      auto *char_solar_charger =
          this->parent_->get_characteristic(this->service_monitoring_uuid_, this->char_solar_charger_uuid_);
      if (char_solar_charger == nullptr) {
        ESP_LOGW(TAG, "[%s] No solar charger characteristic found at device. No solar charger attached?",
                 this->parent_->address_str());
      } else {
        this->char_solar_charger_handle_ = char_solar_charger->handle;
        auto status = esp_ble_gattc_register_for_notify(this->parent()->get_gattc_if(),
                                                        this->parent()->get_remote_bda(), char_solar_charger->handle);
        if (status) {
          ESP_LOGW(TAG, "esp_ble_gattc_register_for_notify failed, status=%d", status);
        } else {
          this->pending_notify_registrations_++;
        }
      }

      // Both characteristics belong to the log data service (AE64A924-...)
      auto *char_management =
          this->parent_->get_characteristic(this->service_log_data_uuid_, this->char_management_uuid_);
      if (char_management == nullptr) {
        ESP_LOGW(TAG, "[%s] No management characteristic found at device. Commands are not available",
                 this->parent_->address_str());
      } else {
        this->char_management_handle_ = char_management->handle;
      }

      auto *char_bulk_data =
          this->parent_->get_characteristic(this->service_log_data_uuid_, this->char_bulk_data_uuid_);
      if (char_bulk_data == nullptr) {
        ESP_LOGW(TAG, "[%s] No bulk data characteristic found at device. Log data is not available",
                 this->parent_->address_str());
      } else {
        this->char_bulk_data_handle_ = char_bulk_data->handle;
        // The characteristic supports indications only. The ble_client enables them via the CCCD.
        auto status = esp_ble_gattc_register_for_notify(this->parent()->get_gattc_if(),
                                                        this->parent()->get_remote_bda(), char_bulk_data->handle);
        if (status) {
          ESP_LOGW(TAG, "esp_ble_gattc_register_for_notify failed, status=%d", status);
        } else {
          this->pending_notify_registrations_++;
        }
      }

      break;
    }
    case ESP_GATTC_REG_FOR_NOTIFY_EVT: {
      // Wait until all notification registrations are completed. Signaling ESTABLISHED too early
      // makes the ble_client release its services before the remaining notify descriptors are written.
      if (this->pending_notify_registrations_ > 0) {
        this->pending_notify_registrations_--;
      }
      if (this->pending_notify_registrations_ == 0) {
        this->node_state = espbt::ClientState::ESTABLISHED;
      }
      break;
    }
    case ESP_GATTC_NOTIFY_EVT: {
      char hex_buf[format_hex_pretty_size(MAX_HEX_DUMP_BYTES)];
      ESP_LOGVV(TAG, "Notification received (handle %d): %s", param->notify.handle,
                format_hex_pretty_to(hex_buf, param->notify.value, param->notify.value_len, '.'));

      std::vector<uint8_t> data(param->notify.value, param->notify.value + param->notify.value_len);

      this->on_votronic_ble_data(param->notify.handle, data);
      break;
    }
    default:
      break;
  }
}

void VotronicBle::on_votronic_ble_data(const uint8_t &handle, const std::vector<uint8_t> &data) {
  if (handle == this->char_solar_charger_handle_) {
    this->decode_solar_charger_data_(data);
    return;
  }

  if (handle == this->char_battery_computer_handle_) {
    this->decode_battery_computer_data_(data);
    return;
  }

  if (handle == this->char_bulk_data_handle_) {
    this->decode_bulk_data_(data);
    return;
  }

  ESP_LOGW(TAG, "Your device is probably not supported. Please create an issue here: "
                "https://github.com/syssi/esphome-votronic/issues");
  char hex_buf[format_hex_pretty_size(MAX_HEX_DUMP_BYTES)];
  ESP_LOGW(TAG, "Please provide the following unhandled message data: %s", format_hex_pretty_to(hex_buf, data, '.'));
}

#endif  // USE_ESP32

void VotronicBle::update() {
#ifdef USE_ESP32
  if (this->node_state != espbt::ClientState::ESTABLISHED) {
    ESP_LOGW(TAG, "[%s] Not connected", this->parent_->address_str());
    return;
  }
#endif
}

bool VotronicBle::send_command(uint8_t command) {
#ifdef USE_ESP32
  if (this->node_state != espbt::ClientState::ESTABLISHED) {
    ESP_LOGW(TAG, "[%s] Cannot send command 0x%02X: not connected", this->parent_->address_str(), command);
    return false;
  }

  if (this->char_management_handle_ == 0) {
    ESP_LOGW(TAG, "[%s] Cannot send command 0x%02X: management characteristic not available",
             this->parent_->address_str(), command);
    return false;
  }

  ESP_LOGD(TAG, "[%s] Send command 0x%02X", this->parent_->address_str(), command);

  uint8_t frame[1] = {command};
  auto status = esp_ble_gattc_write_char(this->parent()->get_gattc_if(), this->parent()->get_conn_id(),
                                         this->char_management_handle_, sizeof(frame), frame, ESP_GATT_WRITE_TYPE_RSP,
                                         ESP_GATT_AUTH_REQ_NONE);
  if (status) {
    ESP_LOGW(TAG, "[%s] esp_ble_gattc_write_char failed, status=%d", this->parent_->address_str(), status);
    return false;
  }

  return true;
#else
  ESP_LOGW(TAG, "Commands are only supported on ESP32");
  return false;
#endif
}

void VotronicBle::decode_bulk_data_(const std::vector<uint8_t> &data) {
  static constexpr uint8_t BEGIN_OF_STREAM = 0xAA;
  static constexpr uint8_t END_OF_STREAM = 0xFF;
  static constexpr size_t RECORD_SIZE_24H_LOG = 12;
  static constexpr size_t RECORD_SIZE_DAILY_LOG = 18;

  if (data.empty()) {
    return;
  }

  char hex_buf[format_hex_pretty_size(MAX_HEX_DUMP_BYTES)];
  ESP_LOGVV(TAG, "Bulk data frame: %s", format_hex_pretty_to(hex_buf, data, '.'));

  // Begin of stream: 0xAA, number of records (0x90: 144 x 10 min, 0x3C/0x78: 60/120 days), unknown
  if (data[0] == BEGIN_OF_STREAM) {
    this->bulk_data_receiving_ = true;
    this->bulk_data_expected_records_ = (data.size() > 1) ? data[1] : 0;
    this->bulk_data_received_records_ = 0;
    ESP_LOGI(TAG, "Log data transmission started (%u records announced)", this->bulk_data_expected_records_);
    return;
  }

  // End of stream: 0xFF, number of records
  if (data[0] == END_OF_STREAM) {
    ESP_LOGI(TAG, "Log data transmission completed (%u of %u records received)", this->bulk_data_received_records_,
             this->bulk_data_expected_records_);
    this->bulk_data_receiving_ = false;
    return;
  }

  if (!this->bulk_data_receiving_) {
    ESP_LOGW(TAG, "Log data record received outside of a transmission");
    return;
  }

  this->bulk_data_received_records_++;

  switch (data.size()) {
    case RECORD_SIZE_24H_LOG:
      this->decode_24h_log_record_(data);
      break;
    case RECORD_SIZE_DAILY_LOG:
      this->decode_daily_log_record_(data);
      break;
    default:
      ESP_LOGW(TAG, "Unexpected log data record size: %zu", data.size());
      break;
  }
}

void VotronicBle::decode_24h_log_record_(const std::vector<uint8_t> &data) {
  // Log data records are big endian
  auto get_16bit = [&](size_t i) -> uint16_t { return (uint16_t(data[i + 0]) << 8) | (uint16_t(data[i + 1]) << 0); };

  // 10 minute resolution, package 0 is the most recent record
  ESP_LOGI(TAG,
           "24h log record %3u (-%4u min): Battery %.1f V, PV %.1f A, status 0x%02X, battery %.1f A, %u Ah, "
           "starter battery %.1f V",
           data[0], data[0] * 10u, get_16bit(1) * 0.1f, get_16bit(3) * 0.1f, data[5], ((int16_t) get_16bit(6)) * 0.1f,
           get_16bit(8), get_16bit(10) * 0.1f);
}

void VotronicBle::decode_daily_log_record_(const std::vector<uint8_t> &data) {
  // Log data records are big endian
  auto get_16bit = [&](size_t i) -> uint16_t { return (uint16_t(data[i + 0]) << 8) | (uint16_t(data[i + 1]) << 0); };
  // Single byte voltages: 7.0 V + raw * 0.1 V
  auto get_voltage = [&](size_t i) -> float { return 7.0f + data[i] * 0.1f; };

  // Package 0 is the current day
  ESP_LOGI(TAG,
           "Daily log record %3u: Battery %.1f..%.1f V, PV %.1f A, current limitation %u min, %u Ah charged, "
           "%u..%u Ah, battery %.1f..%.1f A, starter battery %.1f..%.1f V",
           data[0], get_voltage(1), get_voltage(2), get_16bit(3) * 0.1f, data[5] * 10u, get_16bit(6), get_16bit(8),
           get_16bit(10), ((int16_t) get_16bit(12)) * 0.1f, ((int16_t) get_16bit(14)) * 0.1f, get_voltage(16),
           get_voltage(17));
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

  auto votronic_get_24bit_signed = [&](size_t i) -> int32_t {
    uint32_t val = (uint32_t(data[i + 2]) << 16) | (uint32_t(data[i + 1]) << 8) | (uint32_t(data[i + 0]) << 0);
    if (val & 0x800000) {
      val |= ~0xffffff;
    }
    return (int32_t) val;
  };

  ESP_LOGI(TAG, "Battery computer data received");
  ESP_LOGD(TAG, "  Unknown (Byte     6): %d (0x%02X)", data[6], data[6]);
  ESP_LOGD(TAG, "  Unknown (Byte     7): %d (0x%02X)", data[7], data[7]);
  ESP_LOGD(TAG, "  Unknown (Byte     9): %d (0x%02X)", data[9], data[9]);
  ESP_LOGD(TAG, "  Unknown (Byte    15): %d (0x%02X)", data[15], data[15]);
  ESP_LOGD(TAG, "  Unknown (Byte 16-17): %d (0x%02X 0x%02X)", votronic_get_16bit(16), data[16], data[17]);
  ESP_LOGD(TAG, "  Unknown (Byte 18-19): %d (0x%02X 0x%02X)", votronic_get_16bit(18), data[18], data[19]);

  float battery_voltage = votronic_get_16bit(0) * 0.01f;
  this->publish_state_(this->battery_voltage_sensor_, battery_voltage);
  this->publish_state_(this->secondary_battery_voltage_sensor_, votronic_get_16bit(2) * 0.01f);
  this->publish_state_(this->battery_capacity_remaining_sensor_, (float) votronic_get_16bit(4));
  this->publish_state_(this->state_of_charge_sensor_, (float) data[8]);

  float current = votronic_get_24bit_signed(10) * 0.001f;
  ESP_LOGV(TAG, "  Current (raw): 0x%02X%02X%02X (%.3f A)", data[12], data[11], data[10], current);
  this->publish_state_(this->current_sensor_, current);
  this->publish_state_(this->power_sensor_, current * battery_voltage);
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
  this->publish_state_(this->pv_controller_temperature_sensor_, data[12] != 0 ? (float) data[9] : NAN);
  this->publish_state_(this->battery_status_bitmask_sensor_, data[8]);
  this->publish_state_(this->battery_status_text_sensor_, this->battery_status_bitmask_to_string_(data[8]));
  this->publish_state_(this->pv_controller_status_bitmask_sensor_, data[12]);
  this->publish_state_(this->pv_controller_status_text_sensor_,
                       this->solar_charger_status_bitmask_to_string_(data[12]));
  this->publish_state_(this->controller_active_binary_sensor_, (data[12] & (1 << 3)));
  this->publish_state_(this->current_reduction_binary_sensor_, (data[12] & (1 << 4)));
  this->publish_state_(this->aes_active_binary_sensor_, (data[12] & (1 << 5)));
  this->publish_state_(this->charged_capacity_sensor_, (float) votronic_get_16bit(13));
  this->publish_state_(this->charged_energy_sensor_, votronic_get_16bit(15) * 10.0f);
  this->publish_state_(this->pv_power_sensor_, (float) votronic_get_16bit(17) * 0.1f);

  ESP_LOGD(TAG, "  Unknown (Byte  6): %d (0x%02X)", data[6], data[6]);
  ESP_LOGD(TAG, "  Unknown (Byte  7): %d (0x%02X)", data[7], data[7]);
  ESP_LOGD(TAG, "  Battery status bitmask: %d (0x%02X)", data[8], data[8]);
  ESP_LOGD(TAG, "  PV controller temperature: %d °C", data[9]);
  ESP_LOGD(TAG, "  Unknown (Byte 10): %d (0x%02X)", data[10], data[10]);
  ESP_LOGD(TAG, "  Unknown (Byte 11): %d (0x%02X)", data[11], data[11]);
}

void VotronicBle::dump_config() {
  ESP_LOGCONFIG(TAG, "VotronicBle:");
#ifdef USE_ESP32
  ESP_LOGCONFIG(TAG, "  MAC address                         : %s", this->parent_->address_str());
  char service_monitoring_uuid_str[esp32_ble::UUID_STR_LEN];
  char char_battery_computer_uuid_str[esp32_ble::UUID_STR_LEN];
  char char_solar_charger_uuid_str[esp32_ble::UUID_STR_LEN];
  ESP_LOGCONFIG(TAG, "  Monitoring Service UUID             : %s",
                this->service_monitoring_uuid_.to_str(service_monitoring_uuid_str));
  ESP_LOGCONFIG(TAG, "  Battery Computer Characteristic UUID: %s",
                this->char_battery_computer_uuid_.to_str(char_battery_computer_uuid_str));
  ESP_LOGCONFIG(TAG, "  Solar Charger Characteristic UUID   : %s",
                this->char_solar_charger_uuid_.to_str(char_solar_charger_uuid_str));
#endif

  LOG_BINARY_SENSOR("", "Charging", this->charging_binary_sensor_);
  LOG_BINARY_SENSOR("", "Discharging", this->discharging_binary_sensor_);
  LOG_BINARY_SENSOR("", "Controller active", this->controller_active_binary_sensor_);
  LOG_BINARY_SENSOR("", "Current reduction", this->current_reduction_binary_sensor_);
  LOG_BINARY_SENSOR("", "AES active", this->aes_active_binary_sensor_);

  LOG_SENSOR("", "Battery voltage", this->battery_voltage_sensor_);
  LOG_SENSOR("", "Secondary battery voltage", this->secondary_battery_voltage_sensor_);
  LOG_SENSOR("", "Battery capacity remaining", this->battery_capacity_remaining_sensor_);
  LOG_SENSOR("", "State of charge", this->state_of_charge_sensor_);
  LOG_SENSOR("", "Current", this->current_sensor_);
  LOG_SENSOR("", "Power", this->power_sensor_);
  LOG_SENSOR("", "Battery nominal capacity", this->battery_nominal_capacity_sensor_);
  LOG_SENSOR("", "PV voltage", this->pv_voltage_sensor_);
  LOG_SENSOR("", "PV current", this->pv_current_sensor_);
  LOG_SENSOR("", "PV controller temperature", this->pv_controller_temperature_sensor_);
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

static std::string format_unknown_hex(uint8_t value) {
  char buf[16];
  snprintf(buf, sizeof(buf), "Unknown (0x%02X)", value);
  return buf;
}

std::string VotronicBle::battery_status_bitmask_to_string_(const uint8_t mask) {
  if (mask == 0x00) {
    return "Standby";
  }

  if (mask & (1 << 3)) {
    return "U3 phase";
  }

  if (mask & (1 << 2)) {
    return "U2 phase";
  }

  if (mask & (1 << 1)) {
    return "U1 phase";
  }

  if (mask & (1 << 0)) {
    return "I phase";
  }

  return format_unknown_hex(mask);
}

std::string VotronicBle::solar_charger_status_bitmask_to_string_(const uint8_t mask) {
  if (mask == 0x00) {
    return "Standby";
  }

  if (mask & (1 << 5)) {
    return "AES active";
  }

  if (mask & (1 << 4)) {
    return "Reduced";
  }

  if (mask & (1 << 3)) {
    return "Active";
  }

  return format_unknown_hex(mask);
}

}  // namespace esphome::votronic_ble
