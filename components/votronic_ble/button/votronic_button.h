#pragma once

#include "../votronic_ble.h"
#include "esphome/core/component.h"
#include "esphome/components/button/button.h"

namespace esphome::votronic_ble {

class VotronicButton : public button::Button, public Component {
 public:
  void set_parent(VotronicBle *parent) { this->parent_ = parent; }
  void set_command(uint8_t command) { this->command_ = command; }
  void dump_config() override;
  float get_setup_priority() const override { return setup_priority::DATA; }

 protected:
  void press_action() override;
  VotronicBle *parent_{nullptr};
  uint8_t command_{0};
};

}  // namespace esphome::votronic_ble
