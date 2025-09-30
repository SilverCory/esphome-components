#pragma once

#include "esphome/core/component.h"
#include "esphome/components/button/button.h"
#include "../hoermann_controller.h"

namespace esphome {
namespace hoermann_controller {

class HoermannImpulseButton : public button::Button, public Component {
 public:
  void set_controller(HoermannController *controller) { this->controller_ = controller; }
  void setup() override;
  void dump_config() override;
  void press_action() override;

 protected:
  HoermannController *controller_;
};

class HoermannEmergencyStopButton : public button::Button, public Component {
 public:
  void set_controller(HoermannController *controller) { this->controller_ = controller; }
  void setup() override;
  void dump_config() override;
  void press_action() override;

 protected:
  HoermannController *controller_;
};

}  // namespace hoermann_controller
}  // namespace esphome
