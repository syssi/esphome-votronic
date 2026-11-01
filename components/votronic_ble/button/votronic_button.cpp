#include "votronic_button.h"
#include "esphome/core/log.h"

// Fallback for ESPHome < 2026.10.0
#ifndef ESPHOME_LOG_TAG
#define ESPHOME_LOG_TAG(name, tag) static const char *const name = tag
#endif

namespace esphome::votronic_ble {

ESPHOME_LOG_TAG(TAG, "votronic_ble.button");

void VotronicButton::dump_config() {
  LOG_BUTTON("", "VotronicBle Button", this);
  ESP_LOGCONFIG(TAG, "  Command: 0x%02X", this->command_);
}

void VotronicButton::press_action() { this->parent_->send_command(this->command_); }

}  // namespace esphome::votronic_ble
