#pragma once
#include <cstdint>
#include <vector>
#include "esphome/components/votronic_ble/votronic_ble.h"

namespace esphome::votronic_ble::testing {

class TestableVotronicBle : public VotronicBle {
 public:
  TestableVotronicBle() { set_throttle(0); }
  void update() override {}
  using VotronicBle::decode_solar_charger_data_;
  using VotronicBle::decode_battery_computer_data_;
  using VotronicBle::battery_status_bitmask_to_string_;
  using VotronicBle::solar_charger_status_bitmask_to_string_;
};

}  // namespace esphome::votronic_ble::testing
