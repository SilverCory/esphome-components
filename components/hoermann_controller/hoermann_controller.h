#pragma once

#include "esphome/core/component.h"
#include "esphome/core/hal.h"
#include "esphome/components/uart/uart.h"
#include "esphome/components/cover/cover.h"
#include "esphome/components/switch/switch.h"
#include "esphome/components/button/button.h"
#include "esphome/components/binary_sensor/binary_sensor.h"

namespace esphome {
namespace hoermann_controller {

static const char *const TAG = "hoermann_controller";

enum HoermannAction {
  ACTION_STOP,
  ACTION_OPEN,
  ACTION_CLOSE,
  ACTION_VENTING,
  ACTION_TOGGLE_LIGHT,
  ACTION_EMERGENCY_STOP,
  ACTION_IMPULSE,
};

class HoermannController : public Component, public uart::UARTDevice {
 public:
  void setup() override;
  void loop() override;
  void dump_config() override;
  float get_setup_priority() const override { return setup_priority::DATA; }

  void set_de_pin(GPIOPin *pin) { this->de_pin_ = pin; }

  void register_cover(cover::Cover *cover) { this->cover_ = cover; }
  void register_light_switch(switch_::Switch *sw) { this->light_switch_ = sw; }
  void register_venting_switch(switch_::Switch *sw) { this->venting_switch_ = sw; }
  void register_impulse_button(button::Button *btn) { this->impulse_button_ = btn; }
  void register_emergency_stop_button(button::Button *btn) { this->emergency_stop_button_ = btn; }
  void register_error_sensor(binary_sensor::BinarySensor *sensor) { this->error_sensor_ = sensor; }
  void register_prewarn_sensor(binary_sensor::BinarySensor *sensor) { this->prewarn_sensor_ = sensor; }
  void register_option_relay_sensor(binary_sensor::BinarySensor *sensor) { this->option_relay_sensor_ = sensor; }

  void trigger_action(HoermannAction action);

 protected:
  void parse_message(const uint8_t *buffer, uint8_t len);
  void send_response();
  uint8_t crc8(const uint8_t *data, uint8_t len);

  GPIOPin *de_pin_;

  cover::Cover *cover_{nullptr};
  switch_::Switch *light_switch_{nullptr};
  switch_::Switch *venting_switch_{nullptr};
  button::Button *impulse_button_{nullptr};
  button::Button *emergency_stop_button_{nullptr};
  binary_sensor::BinarySensor *error_sensor_{nullptr};
  binary_sensor::BinarySensor *prewarn_sensor_{nullptr};
  binary_sensor::BinarySensor *option_relay_sensor_{nullptr};

  uint32_t last_message_received_ms_{0};
  bool tx_message_ready_{false};
  uint16_t slave_response_data_{0x1000};
};

}  // namespace hoermann_controller
}  // namespace esphome