#pragma once

#include "esphome/core/component.h"
#include "esphome/core/hal.h"
#include "esphome/components/uart/uart.h"
#include "esphome/components/cover/cover.h"
#include "esphome/components/switch/switch.h"
#include "esphome/components/binary_sensor/binary_sensor.h"
#include "esphome/components/button/button.h"

#include <vector>

namespace esphome {
namespace hoermann_controller {

// Represents the response we will send when polled by the master
enum SlaveResponse : uint16_t {
    RESPONSE_DEFAULT = 0x1000,
    RESPONSE_EMERGENCY_STOP = 0x0000,
    RESPONSE_OPEN = 0x1001,
    RESPONSE_CLOSE = 0x1002,
    RESPONSE_VENTING = 0x1010,
    RESPONSE_TOGGLE_LIGHT = 0x1008,
    RESPONSE_IMPULSE = 0x1004,
    RESPONSE_STOP = 0x1004 // Stop is the same as impulse
};

class HoermannController : public Component, public uart::UARTDevice {
 public:
    void set_de_pin(GPIOPin *de_pin) { this->de_pin_ = de_pin; }

    void register_cover(cover::Cover *cover) { this->cover_ = cover; }
    void register_light_switch(switch_::Switch *sw) { this->light_switch_ = sw; }
    void register_venting_switch(switch_::Switch *sw) { this->venting_switch_ = sw; }
    void register_error_sensor(binary_sensor::BinarySensor *sensor) { this->error_sensor_ = sensor; }
    void register_prewarn_sensor(binary_sensor::BinarySensor *sensor) { this->prewarn_sensor_ = sensor; }
    void register_option_relay_sensor(binary_sensor::BinarySensor *sensor) { this->option_relay_sensor_ = sensor; }
    void register_impulse_button(button::Button *btn) { this->impulse_button_ = btn; }
    void register_estop_button(button::Button *btn) { this->estop_button_ = btn; }

    void setup() override;
    void loop() override;
    void dump_config() override;

    void trigger_action(SlaveResponse action);
    void trigger_stop_action();

 protected:
    void handle_message();
    void parse_broadcast(const std::vector<uint8_t> &data);
    void prepare_response(const std::vector<uint8_t> &data);
    void send_response();
    static uint8_t calc_crc8(const uint8_t *p_data, uint8_t length);

    GPIOPin *de_pin_;
    cover::Cover *cover_{nullptr};
    switch_::Switch *light_switch_{nullptr};
    switch_::Switch *venting_switch_{nullptr};
    binary_sensor::BinarySensor *error_sensor_{nullptr};
    binary_sensor::BinarySensor *prewarn_sensor_{nullptr};
    binary_sensor::BinarySensor *option_relay_sensor_{nullptr};
    button::Button *impulse_button_{nullptr};
    button::Button *estop_button_{nullptr};

    std::vector<uint8_t> rx_buffer_;
    std::vector<uint8_t> tx_buffer_;
    uint32_t last_byte_time_{0};
    bool tx_message_ready_{false};
    uint32_t response_due_time_{0};
    SlaveResponse slave_response_data_{RESPONSE_DEFAULT};
    uint16_t last_broadcast_status_{0};
};

// Custom Cover class - Updated to use new action enums
class HoermannCover : public cover::Cover, public Component {
 public:
    HoermannCover(HoermannController *parent) : parent_(parent) {}

    cover::CoverTraits get_traits() override {
        auto traits = cover::CoverTraits();
        traits.set_is_assumed_state(false);
        traits.set_supports_position(false);
        traits.set_supports_opening(true);
        traits.set_supports_closing(true);
        traits.set_supports_stop(true);
        return traits;
    }

    void control(const cover::CoverCall &call) override {
        if (call.get_stop()) {
            this->parent_->trigger_stop_action();
        }
        if (call.get_position().has_value()) {
            if (*call.get_position() == cover::COVER_OPEN) {
                this->parent_->trigger_action(RESPONSE_OPEN);
            } else if (*call.get_position() == cover::COVER_CLOSED) {
                this->parent_->trigger_action(RESPONSE_CLOSE);
            }
        }
    }

 protected:
    HoermannController *parent_;
};

// Custom Switch classes - Updated to use new action enums
class HoermannLightSwitch : public switch_::Switch, public Component {
 public:
    HoermannLightSwitch(HoermannController *parent) : parent_(parent) {}
    void write_state(bool state) override {
        this->parent_->trigger_action(RESPONSE_TOGGLE_LIGHT);
        publish_state(state);
    }
 protected:
    HoermannController *parent_;
};

class HoermannVentingSwitch : public switch_::Switch, public Component {
 public:
    HoermannVentingSwitch(HoermannController *parent) : parent_(parent) {}
    void write_state(bool state) override {
        if (state) {
            this->parent_->trigger_action(RESPONSE_VENTING);
        } else {
            this->parent_->trigger_action(RESPONSE_CLOSE);
        }
        publish_state(state);
    }
 protected:
    HoermannController *parent_;
};

// Custom Button classes
class HoermannImpulseButton : public button::Button, public Component {
 public:
    HoermannImpulseButton(HoermannController *parent) : parent_(parent) {}
    void press_action() override { this->parent_->trigger_action(RESPONSE_IMPULSE); }
 protected:
    HoermannController *parent_;
};

class HoermannEmergencyStopButton : public button::Button, public Component {
 public:
    HoermannEmergencyStopButton(HoermannController *parent) : parent_(parent) {}
    void press_action() override { this->parent_->trigger_action(RESPONSE_EMERGENCY_STOP); }
 protected:
    HoermannController *parent_;
};

} // namespace hoermann_controller
} // namespace esphome

