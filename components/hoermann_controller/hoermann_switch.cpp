#include "hoermann_switch.h"
#include "esphome/core/log.h"

namespace esphome {
namespace hoermann_controller {

static const char *TAG = "hoermann_controller.switch";

void HoermannLightSwitch::setup() {
  this->controller_->register_light_switch(this);
}

void HoermannLightSwitch::dump_config() {
  LOG_SWITCH("", "Hörmann Light Switch", this);
}

void HoermannLightSwitch::write_state(bool state) {
  this->controller_->trigger_action(ACTION_TOGGLE_LIGHT);
  this->publish_state(state);
}

void HoermannVentingSwitch::setup() {
  this->controller_->register_venting_switch(this);
}

void HoermannVentingSwitch::dump_config() {
  LOG_SWITCH("", "Hörmann Venting Switch", this);
}

void HoermannVentingSwitch::write_state(bool state) {
  if (state)
    this->controller_->trigger_action(ACTION_VENTING);
  else
    this->controller_->trigger_action(ACTION_CLOSE);
  this->publish_state(state);
}

}  // namespace hoermann_controller
}  // namespace esphome
