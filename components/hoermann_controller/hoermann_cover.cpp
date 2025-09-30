#include "hoermann_cover.h"
#include "esphome/core/log.h"

namespace esphome {
namespace hoermann_controller {

static const char *const TAG = "hoermann_controller.cover";

void HoermannCover::setup() {
  this->controller_->register_cover(this);
}

void HoermannCover::dump_config() {
  LOG_COVER("", "Hörmann Cover", this);
}

cover::CoverTraits HoermannCover::get_traits() {
  auto traits = cover::CoverTraits();
  traits.set_is_assumed_state(false);
  traits.set_supports_position(true);
  traits.set_supports_stop(true);
  return traits;
}

void HoermannCover::control(const cover::CoverCall &call) {
  if (call.get_stop())
    this->controller_->trigger_action(ACTION_STOP);
  if (call.get_position().has_value()) {
    if (*call.get_position() == cover::COVER_OPEN)
      this->controller_->trigger_action(ACTION_OPEN);
    else if (*call.get_position() == cover::COVER_CLOSED)
      this->controller_->trigger_action(ACTION_CLOSE);
  }
}

}  // namespace hoermann_controller
}  // namespace esphome
