#pragma once

#include "esphome/core/component.h"
#include "esphome/components/uart/uart.h"
#include "esphome/components/cover/cover.h"
#include "esphome/components/switch/switch.h"
#include "esphome/components/binary_sensor/binary_sensor.h"
#include "esphome/components/button/button.h"
#include "esphome/core/hal.h"

namespace esphome {
namespace hoermann_controller {

static const char *const TAG = "hoermann_controller";

// Forward declarations
class HoermannController;
class HoermannCover;
class HoermannLightSwitch;
class HoermannVentingSwitch;
class HoermannImpulseButton;
class HoermannEmergencyStopButton;

enum HoermannAction {
    ACTION_STOP,
    ACTION_OPEN,
    ACTION_CLOSE,
    ACTION_VENTING,
    ACTION_TOGGLE_LIGHT,
    ACTION_EMERGENCY_STOP,
    ACTION_IMPULSE
};

class HoermannController : public uart::UARTDevice, public Component {
public:
    void setup() override;
    void loop() override;
    void dump_config() override;

    void set_de_pin(GPIOPin *de_pin) { this->de_pin_ = de_pin; }
    void register_cover(HoermannCover *cover) { this->cover_ = cover; }
    void register_light_switch(HoermannLightSwitch *sw) { this->light_switch_ = sw; }
    void register_venting_switch(HoermannVentingSwitch *sw) { this->venting_switch_ = sw; }
    void register_impulse_button(HoermannImpulseButton *btn) { this->impulse_button_ = btn; }
    void register_emergency_stop_button(HoermannEmergencyStopButton *btn) { this->emergency_stop_button_ = btn; }
    void register_binary_sensor(const std::string &type, binary_sensor::BinarySensor *sensor);

    void trigger_action(HoermannAction action);

protected:
    void parse_message(const uint8_t *buffer, uint8_t len);
    void send_response();
    uint8_t crc8(const uint8_t *data, uint8_t len);

    GPIOPin *de_pin_;
    HoermannCover *cover_{nullptr};
    HoermannLightSwitch *light_switch_{nullptr};
    HoermannVentingSwitch *venting_switch_{nullptr};
    HoermannImpulseButton *impulse_button_{nullptr};
    HoermannEmergencyStopButton *emergency_stop_button_{nullptr};
    binary_sensor::BinarySensor *error_sensor_{nullptr};
    binary_sensor::BinarySensor *prewarn_sensor_{nullptr};
    binary_sensor::BinarySensor *option_relay_sensor_{nullptr};

    bool tx_message_ready_{false};
    uint32_t last_message_received_ms_{0};
    uint16_t slave_response_data_{0x1000}; // RESPONSE_DEFAULT
};

class HoermannLightSwitch : public switch_::Switch {
public:
    HoermannLightSwitch(HoermannController *parent) : parent_(parent) {}
protected:
    void write_state(bool state) override;
    HoermannController *parent_;
};

class HoermannVentingSwitch : public switch_::Switch {
public:
    HoermannVentingSwitch(HoermannController *parent) : parent_(parent) {}
protected:
    void write_state(bool state) override;
    HoermannController *parent_;
};

class HoermannImpulseButton : public button::Button {
public:
    HoermannImpulseButton(HoermannController *parent) : parent_(parent) {}
protected:
    void press_action() override;
    HoermannController *parent_;
};

class HoermannEmergencyStopButton : public button::Button {
public:
    HoermannEmergencyStopButton(HoermannController *parent) : parent_(parent) {}
protected:
    void press_action() override;
    HoermannController *parent_;
};

class HoermannCover : public cover::Cover, public Component {
public:
    void setup() override {}
    void set_parent(HoermannController *parent) { this->parent_ = parent; }
    cover::CoverTraits get_traits() override;

protected:
    void control(const cover::CoverCall &call) override;
    HoermannController *parent_;
};

}  // namespace hoermann_controller
}  // namespace esphome

