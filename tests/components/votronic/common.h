#pragma once
#include <cstdint>
#include <vector>
#include "esphome/components/votronic/votronic.h"

namespace esphome::votronic::testing {

class TestableVotronic : public Votronic {
 public:
  TestableVotronic() { set_throttle(0); }
  using Votronic::decode_solar_charger_data_;
  using Votronic::decode_charger_data_;
  using Votronic::decode_charging_converter_data_;
  using Votronic::decode_battery_computer_info1_data_;
  using Votronic::decode_battery_computer_info2_data_;
  using Votronic::on_votronic_data;
};

}  // namespace esphome::votronic::testing
