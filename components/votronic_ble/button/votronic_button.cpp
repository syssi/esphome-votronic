#include "votronic_button.h"
#include "esphome/core/log.h"
#include "esphome/core/application.h"

namespace esphome {
namespace votronic_ble {

static const char *const TAG = "votronic_ble.button";

void VotronicButton::dump_config() { LOG_BUTTON("", "VotronicBle Button", this); }
void VotronicButton::press_action() { this->parent_->send_command(this->holding_register_); }

}  // namespace votronic_ble
}  // namespace esphome
