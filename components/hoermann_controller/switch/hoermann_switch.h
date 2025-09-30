#pragma once

#include "esphome/core/component.h"
#include "esphome/components/switch/switch.h"
#include "hoermann_controller.h"

namespace esphome {
namespace hoermann_controller {

class HoermannLightSwitch : public switch_::Switch, public Component {
 public:
  void set_controller(HoermannController *controller) { this->controller_ = controller; }
  void setup() override;
  void dump_config() override;
  void write_state(bool state) override;

 protected:
  HoermannController *controller_;
};

class HoermannVentingSwitch : public switch_::Switch, public Component {
 public:
  void set_controller(HoermannController *controller) { this->controller_ = controller; }
  void setup() override;
  void dump_config() override;
  void write_state(bool state) override;

 protected:
  HoermannController *controller_;
};

}  // namespace hoermann_controller
}  // namespace esphome
