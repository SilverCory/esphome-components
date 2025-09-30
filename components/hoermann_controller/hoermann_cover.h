#pragma once

#include "esphome/core/component.h"
#include "esphome/components/cover/cover.h"
#include "hoermann_controller.h"

namespace esphome {
namespace hoermann_controller {

class HoermannCover : public cover::Cover, public Component {
 public:
  void set_controller(HoermannController *controller) { this->controller_ = controller; }
  void setup() override;
  void dump_config() override;
  cover::CoverTraits get_traits() override;
  void control(const cover::CoverCall &call) override;

 protected:
  HoermannController *controller_;
};

}  // namespace hoermann_controller
}  // namespace esphome
