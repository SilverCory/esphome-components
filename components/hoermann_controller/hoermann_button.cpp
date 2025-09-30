#include "hoermann_button.h"
#include "esphome/core/log.h"

namespace esphome {
namespace hoermann_controller {

static const char *const TAG = "hoermann_controller.button";

void HoermannImpulseButton::setup() {
  this->controller_->register_impulse_button(this);
}

void HoermannImpulseButton::dump_config() {
  LOG_BUTTON("", "Hörmann Impulse Button", this);
}

void HoermannImpulseButton::press_action() {
  this->controller_->trigger_action(ACTION_IMPULSE);
}

void HoermannEmergencyStopButton::setup() {
  this->controller_->register_emergency_stop_button(this);
}

void HoermannEmergencyStopButton::dump_config() {
  LOG_BUTTON("", "Hörmann Emergency Stop Button", this);
}

void HoermannEmergencyStopButton::press_action() {
  this->controller_->trigger_action(ACTION_EMERGENCY_STOP);
}

}  // namespace hoermann_controller
}  // namespace esphome
