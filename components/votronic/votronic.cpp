#include "votronic.h"
#include "esphome/core/log.h"
#include "esphome/core/helpers.h"

namespace esphome {
namespace votronic {

static const char *const TAG = "votronic";

static const uint8_t VOTRONIC_FRAME_START = 0xAA;
static const uint8_t VOTRONIC_FRAME_LENGTH = 16;

static const uint8_t VOTRONIC_FRAME_TYPE_SOLAR_CHARGER = 0x1A;
static const uint8_t VOTRONIC_FRAME_TYPE_CHARGER = 0x3A;
static const uint8_t VOTRONIC_FRAME_TYPE_CHARGING_CONVERTER = 0x7A;

static const uint8_t VOTRONIC_FRAME_TYPE_BATTERY_COMPUTER_INFO1 = 0xCA;
static const uint8_t VOTRONIC_FRAME_TYPE_BATTERY_COMPUTER_INFO2 = 0xDA;
static const uint8_t VOTRONIC_FRAME_TYPE_BATTERY_COMPUTER_INFO3 = 0xFA;

static const uint8_t VOTRONIC_FRAME_TYPE_CONTROL_CHARGER = 0x7A;             // Incorrect protocol description?
static const uint8_t VOTRONIC_FRAME_TYPE_CONTROL_CHARGING_CONVERTER = 0x3A;  // Incorrect protocol description?
static const uint8_t VOTRONIC_FRAME_TYPE_CONTROL_BATTERY_COMPUTER = 0xEA;

static const uint8_t BATTERY_STATUS_SIZE = 8;
static const char *const BATTERY_STATUS[BATTERY_STATUS_SIZE] = {
    "I phase",         // 0000 0001
    "U1 phase",        // 0000 0010
    "U2 phase",        // 0000 0100
    "U3 phase",        // 0000 1000
    "Unused (Bit 4)",  // 0001 0000
    "Unused (Bit 5)",  // 0010 0000
    "Unused (Bit 6)",  // 0100 0000
    "Unused (Bit 7)",  // 1000 0000
};

static const uint8_t SOLAR_CHARGER_STATUS_SIZE = 8;
static const char *const SOLAR_CHARGER_STATUS[SOLAR_CHARGER_STATUS_SIZE] = {
    "Unused (Bit 0)",  // 0000 0001
    "Unused (Bit 1)",  // 0000 0010
    "Unused (Bit 2)",  // 0000 0100
    "Active",          // 0000 1000
    "Reduced",         // 0001 0000
    "AES active",      // 0010 0000
    "Unused (Bit 6)",  // 0100 0000
    "Unused (Bit 7)",  // 1000 0000
};

static const uint8_t CHARGER_STATUS_SIZE = 8;
static const char *const CHARGER_STATUS[CHARGER_STATUS_SIZE] = {
    "Unused (Bit 0)",                         // 0000 0001
    "Unused (Bit 1)",                         // 0000 0010
    "Charging Battery 1",                     // 0000 0100
    "Charging Battery 2 / Reverse Charging",  // 0000 1000
    "Reduced",                                // 0001 0000
    "AC Power Limit",                         // 0010 0000
    "Unused (Bit 6)",                         // 0100 0000
    "Unused (Bit 7)",                         // 1000 0000
};

void Votronic::loop() {
  const uint32_t now = millis();

  if (now - this->last_byte_ > this->rx_timeout_) {
    this->rx_buffer_.clear();
    this->last_byte_ = now;
  }

  while (this->available()) {
    uint8_t byte;
    this->read_byte(&byte);
    if (this->parse_votronic_byte_(byte)) {
      this->last_byte_ = now;
    } else {
      this->rx_buffer_.clear();
    }
  }
}

void Votronic::update() {}

bool Votronic::parse_votronic_byte_(uint8_t byte) {
  size_t at = this->rx_buffer_.size();
  this->rx_buffer_.push_back(byte);
  const uint8_t *raw = &this->rx_buffer_[0];
  const uint8_t frame_len = VOTRONIC_FRAME_LENGTH;

  if (at == 0)
    return true;

  if (raw[0] != VOTRONIC_FRAME_START) {
    ESP_LOGW(TAG, "Invalid header");

    // return false to reset buffer
    return false;
  }

  if (at < frame_len - 1)
    return true;

  uint8_t computed_crc = chksum_(raw + 1, frame_len - 2);
  uint8_t remote_crc = raw[frame_len - 1];
  if (computed_crc != remote_crc) {
    ESP_LOGW(TAG, "CRC check failed! 0x%02X != 0x%02X", computed_crc, remote_crc);
    return false;
  }

  ESP_LOGVV(TAG, "RX <- %s", format_hex_pretty(raw, at + 1).c_str());

  std::vector<uint8_t> data(this->rx_buffer_.begin(), this->rx_buffer_.begin() + frame_len);

  this->on_votronic_data(data);

  // return false to reset buffer
  return false;
}

void Votronic::on_votronic_data(const std::vector<uint8_t> &data) {
  uint8_t data_len = data.size();
  if (data_len != VOTRONIC_FRAME_LENGTH) {
    ESP_LOGW(TAG, "Skipping frame because of invalid length: %d", data_len);
    return;
  }

  const uint8_t frame_type = data[1];
  switch (frame_type) {
    case VOTRONIC_FRAME_TYPE_SOLAR_CHARGER:
      this->decode_solar_charger_data_(data);
      break;
    case VOTRONIC_FRAME_TYPE_CHARGER:
    case VOTRONIC_FRAME_TYPE_CHARGING_CONVERTER:
      this->decode_charger_data_(frame_type, data);
      break;
    case VOTRONIC_FRAME_TYPE_BATTERY_COMPUTER_INFO1:
      this->decode_battery_computer_info1_data_(data);
      break;
    case VOTRONIC_FRAME_TYPE_BATTERY_COMPUTER_INFO2:
      this->decode_battery_computer_info2_data_(data);
      break;
    case VOTRONIC_FRAME_TYPE_BATTERY_COMPUTER_INFO3:
      this->decode_battery_computer_info3_data_(data);
      break;
    default:
      ESP_LOGW(TAG, "Your device is probably not supported. Please create an issue here: "
                    "https://github.com/syssi/esphome-votronic/issues");
      ESP_LOGW(TAG, "Please provide the following unhandled message data (0x%02X): %s", frame_type,
               format_hex_pretty(&data.front(), data.size()).c_str());
  }
}

void Votronic::decode_solar_charger_data_(const std::vector<uint8_t> &data) {
  const uint32_t now = millis();
  if (now - this->last_solar_charger_data_ < this->throttle_) {
    return;
  }
  this->last_solar_charger_data_ = now;

  auto votronic_get_16bit = [&](size_t i) -> uint16_t {
    return (uint16_t(data[i + 1]) << 8) | (uint16_t(data[i + 0]) << 0);
  };

  ESP_LOGI(TAG, "Solar charger data received");
  ESP_LOGVV(TAG, "  %s", format_hex_pretty(&data.front(), data.size()).c_str());

  // Byte Len Payload     Description                      Unit  Precision
  //   0   1  0xAA        Sync Byte
  //   1   1  0x1A        Frame Type
  //   2   2  0xA0 0x05   Battery Voltage                    V    U16 10mV/Bit
  this->publish_state_(this->battery_voltage_sensor_, votronic_get_16bit(2) * 0.01f);
  //   4   2  0xA4 0x06   PV Voltage                         V    U16 10mV/Bit Nur bei MPP-Version
  float pv_voltage = votronic_get_16bit(4) * 0.01f;
  this->publish_state_(this->pv_voltage_sensor_, pv_voltage);
  //   6   2  0x78 0x00   PV Current                         A    S16 100mA/Bit
  float pv_current = votronic_get_16bit(6) * 0.1f;
  this->publish_state_(this->pv_current_sensor_, pv_current);
  this->publish_state_(this->pv_power_sensor_, pv_voltage * pv_current);
  //   8   1  0x00        Reserved
  //   9   1  0x00        Reserved
  //  10   1  0xA0        Reserved
  //  11   1  0x15        Reserved
  this->publish_state_(this->controller_temperature_sensor_, (float) data[11]);
  //  12   1  0x03        Charging mode setting (dip switches)
  this->publish_state_(this->charging_mode_setting_id_sensor_, data[12]);
  this->publish_state_(this->charging_mode_setting_text_sensor_, this->charging_mode_setting_to_string_(data[12]));
  //  13   1  0x00        Battery Controller Status               Bitmask
  this->publish_state_(this->battery_status_bitmask_sensor_, data[13]);
  this->publish_state_(this->battery_status_text_sensor_, this->battery_status_bitmask_to_string_(data[13]));
  //  14   1  0x00        PV Controller Status                    Bitmask
  this->publish_state_(this->pv_controller_status_bitmask_sensor_, data[14]);
  this->publish_state_(this->pv_controller_status_text_sensor_,
                       this->solar_charger_status_bitmask_to_string_(data[14]));
  this->publish_state_(this->controller_active_binary_sensor_, (data[14] & (1 << 3)));
  this->publish_state_(this->current_reduction_binary_sensor_, (data[14] & (1 << 4)));
  this->publish_state_(this->aes_active_binary_sensor_, (data[14] & (1 << 5)));
}

void Votronic::decode_charger_data_(const uint8_t &frame_type, const std::vector<uint8_t> &data) {
  const uint32_t now = millis();
  if (now - this->last_charger_data_ < this->throttle_) {
    return;
  }
  this->last_charger_data_ = now;

  auto votronic_get_16bit = [&](size_t i) -> uint16_t {
    return (uint16_t(data[i + 1]) << 8) | (uint16_t(data[i + 0]) << 0);
  };

  ESP_LOGI(TAG, "Charger data received");
  ESP_LOGVV(TAG, "  %s", format_hex_pretty(&data.front(), data.size()).c_str());

  // Byte Len Payload     Description                      Unit  Precision
  //   0   1  0xAA        Sync Byte
  //   1   1  0x3A        Frame Type
  //   2   2  0xA0 0x05   Battery Voltage                    V    U16 10mV/Bit
  float battery_voltage = votronic_get_16bit(2) * 0.01f;
  this->publish_state_(this->battery_voltage_sensor_, battery_voltage);
  //   4   2  0xA4 0x06   Second Battery Voltage             V    U16 10mV/Bit
  this->publish_state_(this->secondary_battery_voltage_sensor_, votronic_get_16bit(4) * 0.01f);
  //   6   2  0x78 0x00   Charging Current                   A    S16 100mA/Bit
  float current = (float) ((int16_t) votronic_get_16bit(6)) * 0.1f;
  this->publish_state_(this->current_sensor_, current);
  this->publish_state_(this->power_sensor_, current * battery_voltage);
  this->publish_state_(this->charging_binary_sensor_, (current > 0.0f));
  this->publish_state_(this->discharging_binary_sensor_, (current < 0.0f));
  //   8   1  0x00        Reserved
  //   9   1  0x00        Reserved
  //  10   1  0xA0        Charging Power                     %    0-100% 1%/Bit
  this->publish_state_(this->state_of_charge_sensor_, (float) data[10]);
  //  11   1  0x15        Controller temperature
  this->publish_state_(this->controller_temperature_sensor_, (float) data[11]);
  //  12   1  0x03        Charging mode setting (dip switches)
  this->publish_state_(this->charging_mode_setting_id_sensor_, data[12]);
  this->publish_state_(this->charging_mode_setting_text_sensor_, this->charging_mode_setting_to_string_(data[12]));
  //  13   1  0x00        Battery Controller Status               Bitmask
  this->publish_state_(this->battery_status_bitmask_sensor_, data[13]);
  this->publish_state_(this->battery_status_text_sensor_, this->battery_status_bitmask_to_string_(data[13]));
  //  14   1  0x00        Charging Controller Status              Bitmask
  this->publish_state_(this->charging_controller_status_bitmask_sensor_, data[14]);
  this->publish_state_(this->charging_controller_status_text_sensor_,
                       this->charger_status_bitmask_to_string_(data[14]));
  this->publish_state_(this->controller_active_binary_sensor_, (data[14] & (1 << 3)));
  this->publish_state_(this->current_reduction_binary_sensor_, (data[14] & (1 << 4)));
  this->publish_state_(this->aes_active_binary_sensor_, (data[14] & (1 << 5)));
}

void Votronic::decode_battery_computer_info1_data_(const std::vector<uint8_t> &data) {
  const uint32_t now = millis();
  if (now - this->last_battery_computer_info1_data_ < this->throttle_) {
    return;
  }
  this->last_battery_computer_info1_data_ = now;

  auto votronic_get_16bit = [&](size_t i) -> uint16_t {
    return (uint16_t(data[i + 1]) << 8) | (uint16_t(data[i + 0]) << 0);
  };

  // Example frame of a Votronic Smart Shunt 400 S:
  // 0xAA 0xCA 0x03 0x05 0x0F 0x05 0xC7 0x01 0x20 0x00 0x63 0x00 0x7B 0xFE 0xFF 0x39

  ESP_LOGI(TAG, "Battery computer info1 data received");
  ESP_LOGVV(TAG, "i1  %s", format_hex_pretty(&data.front(), data.size()).c_str());

  // Byte Len Payload     Description                      Unit  Precision
  //   0   1  0xAA        Sync Byte
  //   1   1  0xCA        Frame Type
  //   2   2  0x03 0x05   Battery Voltage
  ESP_LOGI(TAG, "i1 Battery voltage: %.2f V", votronic_get_16bit(2) * 0.01f);
  float battery_voltage = votronic_get_16bit(2) * 0.01f;
  this->publish_state_(this->battery_voltage_sensor_, battery_voltage);
  //   4   2  0x0F 0x05   Second Battery Voltage
  ESP_LOGI(TAG, "i1 Second battery voltage: %.2f V", votronic_get_16bit(4) * 0.01f);
  this->publish_state_(this->secondary_battery_voltage_sensor_, votronic_get_16bit(4) * 0.01f);
  //   6   2  0xC7 0x01
  ESP_LOGI(TAG, "i1 Capacity remaining: %.0f Ah", votronic_get_16bit(6) * 1.0f);
  //   8   2  0x20 0x00
  ESP_LOGI(TAG, "i1 Byte   8-9: 0x%02X 0x%02X / %d %d / %d", data[8], data[9], data[8], data[9], votronic_get_16bit(8));
  //  10   2  0x63 0x00
  ESP_LOGI(TAG, "i1 State of charge: %.0f %%", votronic_get_16bit(10) * 1.0f);
  this->publish_state_(this->state_of_charge_sensor_, (float) data[10]);
  //  12   2  0x7B 0xFE
  ESP_LOGI(TAG, "i1 Current: %.3f A", ((int16_t) votronic_get_16bit(12)) * 0.001f);
  float current = ((int16_t) votronic_get_16bit(12)) * 0.001f;
  this->publish_state_(this->current_sensor_, current);
  this->publish_state_(this->power_sensor_, current * battery_voltage);
  this->publish_state_(this->charging_binary_sensor_, (current > 0.0f));
  this->publish_state_(this->discharging_binary_sensor_, (current < 0.0f));

  //  14   1  0xFF
  ESP_LOGI(TAG, "i1 Byte    14: 0x%02X / %d", data[14], data[14]);
  //  15   1  0x39        CRC
}

void Votronic::decode_battery_computer_info2_data_(const std::vector<uint8_t> &data) {
  const uint32_t now = millis();
  if (now - this->last_battery_computer_info2_data_ < this->throttle_) {
    return;
  }
  this->last_battery_computer_info2_data_ = now;

  auto votronic_get_16bit = [&](size_t i) -> uint16_t {
    return (uint16_t(data[i + 1]) << 8) | (uint16_t(data[i + 0]) << 0);
  };

  // Example frame of a Votronic Smart Shunt 400 S:
  // 0xAA 0xDA 0x00 0x00 0x00 0x00 0xF8 0x11 0x5E 0x07 0x00 0x00 0x2F 0x04 0x02 0x43

  ESP_LOGI(TAG, "Battery computer info2 data received");
  ESP_LOGVV(TAG, "i2  %s", format_hex_pretty(&data.front(), data.size()).c_str());

  // Byte Len Payload     Description                      Unit  Precision
  //   0   1  0xAA        Sync Byte
  //   1   1  0xDA        Frame Type
  //   2   2  0x00 0x00
  ESP_LOGI(TAG, "i2 Byte   2-3: 0x%02X 0x%02X / %d %d / %d", data[2], data[3], data[2], data[3], votronic_get_16bit(2));
  //   4   2  0x00 0x00
  ESP_LOGI(TAG, "i2 Byte   4-5: 0x%02X 0x%02X / %d %d / %d", data[4], data[5], data[4], data[5], votronic_get_16bit(4));
  //   6   2  0xF8 0x11
  ESP_LOGI(TAG, "i2 Nominal capacity: %.1f Ah", votronic_get_16bit(6) * 0.1f);
  //   8   2  0x5E 0x07
  ESP_LOGI(TAG, "i2 Byte   8-9: 0x%02X 0x%02X / %d %d / %d", data[8], data[9], data[8], data[9], votronic_get_16bit(8));
  //  10   2  0x00 0x00
  ESP_LOGI(TAG, "i2 Byte 10-11: 0x%02X 0x%02X / %d %d / %d", data[10], data[11], data[10], data[11],
           votronic_get_16bit(10));
  //  12   1  0x2F
  ESP_LOGI(TAG, "i2 Battery setting: 0x%02X / %d", data[12], data[12]);
  //  13   1  0x04
  ESP_LOGI(TAG, "i2 Byte 13: 0x%02X / %d", data[13], data[13]);
  //  14   1  0x02
  ESP_LOGI(TAG, "i2 Byte    14: 0x%02X / %d", data[14], data[14]);
  //  15   1  0x43        CRC
}

void Votronic::decode_battery_computer_info3_data_(const std::vector<uint8_t> &data) {
  const uint32_t now = millis();
  if (now - this->last_battery_computer_info3_data_ < this->throttle_) {
    return;
  }
  this->last_battery_computer_info3_data_ = now;

  auto votronic_get_16bit = [&](size_t i) -> uint16_t {
    return (uint16_t(data[i + 1]) << 8) | (uint16_t(data[i + 0]) << 0);
  };

  // Example frame of a Votronic Smart Shunt 400 S:
  // 0xAA 0xFA 0x2F 0x00 0x00 0x00 0xD2 0x02 0x00 0x0A 0x00 0x00 0x28 0xD0 0x00 0xF7

  ESP_LOGI(TAG, "Battery computer info3 data received");
  ESP_LOGVV(TAG, "i3  %s", format_hex_pretty(&data.front(), data.size()).c_str());

  // Byte Len Payload     Description                      Unit  Precision
  //   0   1  0xAA        Sync Byte
  //   1   1  0xFA        Frame Type
  //   2   2  0x2F 0x00
  ESP_LOGI(TAG, "i3 Byte   2-3: 0x%02X 0x%02X / %d %d / %d", data[2], data[3], data[2], data[3], votronic_get_16bit(2));
  //   4   2  0x00 0x00
  ESP_LOGI(TAG, "i3 Byte   4-5: 0x%02X 0x%02X / %d %d / %d", data[4], data[5], data[4], data[5], votronic_get_16bit(4));
  //   6   2  0xD2 0x02
  ESP_LOGI(TAG, "i3 Byte   6-7: 0x%02X 0x%02X / %d %d / %d", data[6], data[7], data[6], data[7], votronic_get_16bit(6));
  //   8   2  0x00 0x0A
  ESP_LOGI(TAG, "i3 Byte   8-9: 0x%02X 0x%02X / %d %d / %d", data[8], data[9], data[8], data[9], votronic_get_16bit(8));
  //  10   2  0x00 0x00
  ESP_LOGI(TAG, "i3 Byte 10-11: 0x%02X 0x%02X / %d %d / %d", data[10], data[11], data[10], data[11],
           votronic_get_16bit(10));
  //  12   2  0x28 0xD0
  ESP_LOGI(TAG, "i3 Byte 12-13: 0x%02X 0x%02X / %d %d / %d", data[12], data[13], data[12], data[13],
           votronic_get_16bit(12));
  //  14   1  0x00
  ESP_LOGI(TAG, "i3 Byte    14: 0x%02X / %d", data[14], data[14]);
  //  15   1  0xF7        CRC
}

void Votronic::dump_config() {
  ESP_LOGCONFIG(TAG, "Votronic:");
  ESP_LOGCONFIG(TAG, "  RX timeout: %d ms", this->rx_timeout_);

  LOG_BINARY_SENSOR("", "Charging", this->charging_binary_sensor_);
  LOG_BINARY_SENSOR("", "Discharging", this->discharging_binary_sensor_);
  LOG_BINARY_SENSOR("", "Controller active", this->controller_active_binary_sensor_);
  LOG_BINARY_SENSOR("", "Current reduction", this->current_reduction_binary_sensor_);
  LOG_BINARY_SENSOR("", "AES active", this->aes_active_binary_sensor_);

  LOG_SENSOR("", "Battery voltage", this->battery_voltage_sensor_);
  LOG_SENSOR("", "Secondary battery voltage", this->secondary_battery_voltage_sensor_);
  LOG_SENSOR("", "State of charge", this->state_of_charge_sensor_);
  LOG_SENSOR("", "Current", this->current_sensor_);
  LOG_SENSOR("", "Power", this->power_sensor_);
  LOG_SENSOR("", "PV voltage", this->pv_voltage_sensor_);
  LOG_SENSOR("", "PV current", this->pv_current_sensor_);
  LOG_SENSOR("", "PV power", this->pv_power_sensor_);
  LOG_SENSOR("", "Battery status bitmask", this->battery_status_bitmask_sensor_);
  LOG_SENSOR("", "Charging Controller status bitmask", this->charging_controller_status_bitmask_sensor_);
  LOG_SENSOR("", "PV Controller status bitmask", this->pv_controller_status_bitmask_sensor_);
  LOG_SENSOR("", "Charging mode setting ID", this->charging_mode_setting_id_sensor_);
  LOG_SENSOR("", "Controller Temperature", this->controller_temperature_sensor_);

  LOG_TEXT_SENSOR("", "Battery status", this->battery_status_text_sensor_);
  LOG_TEXT_SENSOR("", "Charging controller status", this->charging_controller_status_text_sensor_);
  LOG_TEXT_SENSOR("", "PV controller status", this->pv_controller_status_text_sensor_);
  LOG_TEXT_SENSOR("", "Charging mode setting", this->charging_mode_setting_text_sensor_);
}
float Votronic::get_setup_priority() const {
  // After UART bus
  return setup_priority::BUS - 1.0f;
}

void Votronic::publish_state_(binary_sensor::BinarySensor *binary_sensor, const bool &state) {
  if (binary_sensor == nullptr)
    return;

  binary_sensor->publish_state(state);
}

void Votronic::publish_state_(sensor::Sensor *sensor, float value) {
  if (sensor == nullptr)
    return;

  sensor->publish_state(value);
}

void Votronic::publish_state_(text_sensor::TextSensor *text_sensor, const std::string &state) {
  if (text_sensor == nullptr)
    return;

  text_sensor->publish_state(state);
}

std::string Votronic::charging_mode_setting_to_string_(const uint8_t charging_mode_setting) {
  switch (charging_mode_setting) {
    case 0x35:
      return "Lead GEL";
    case 0x22:
      return "Lead AGM1";
    case 0x2f:
      return "Lead AGM2";
    case 0x50:
      return "LiFePo4 13.9V";
    case 0x52:
      return "LiFePo4 14.2V";
    case 0x54:
      return "LiFePo4 14.4V";
    case 0x56:
      return "LiFePo4 14.6V";
    case 0x58:
      return "LiFePo4 14.8V";
  }

  return str_snprintf("Unknown (0x%02X)", 15, charging_mode_setting);
}

std::string Votronic::battery_status_bitmask_to_string_(const uint8_t mask) {
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

  return str_snprintf("Unknown (0x%02X)", 15, mask);
}

std::string Votronic::solar_charger_status_bitmask_to_string_(const uint8_t mask) {
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

  return str_snprintf("Unknown (0x%02X)", 15, mask);
}

std::string Votronic::charger_status_bitmask_to_string_(const uint8_t mask) {
  bool first = true;
  std::string errors_list = "";

  if (mask == 0x00) {
    return "Standby";
  }

  if (mask) {
    for (uint8_t i = 0; i < CHARGER_STATUS_SIZE; i++) {
      if (mask & (1 << i)) {
        if (first) {
          first = false;
        } else {
          errors_list.append(";");
        }
        errors_list.append(CHARGER_STATUS[i]);
      }
    }
  }

  return errors_list;
}

}  // namespace votronic
}  // namespace esphome
