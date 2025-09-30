#pragma once

#include "esphome/core/component.h"
#include "esphome/components/uart/uart.h"
#include "esphome/components/cover/cover.h"
#include "esphome/components/switch/switch.h"
#include "esphome/components/binary_sensor/binary_sensor.h"
#include "esphome/components/button/button.h"

namespace esphome {
namespace hoermann_controller {

static const char *const TAG = "hoermann_controller";

enum HoermannAction {
    ACTION_STOP = 0,
    ACTION_OPEN = 1,
    ACTION_CLOSE = 2,
    ACTION_VENTING = 3,
    ACTION_TOGGLE_LIGHT = 4,
    ACTION_EMERGENCY_STOP = 5,
    ACTION_IMPULSE = 6,
};

enum HoermannResponseType {
    RESPONSE_DEFAULT = 0x1000,
    RESPONSE_EMERGENCY_STOP = 0x0000,
    RESPONSE_OPEN = 0x1001,
    RESPONSE_CLOSE = 0x1002,
    RESPONSE_VENTING = 0x1010,
    RESPONSE_TOGGLE_LIGHT = 0x1008,
    RESPONSE_IMPULSE = 0x1004,
};

class HoermannController;

class HoermannCover : public cover::Cover, public Component {
public:
    HoermannCover(HoermannController *parent) : parent_(parent) {}
    void setup() override;
    void loop() override;
    void dump_config() override;
    cover::CoverTraits get_traits() override;

protected:
    void control(const cover::CoverCall &call) override;
    HoermannController *parent_;
};

class HoermannLightSwitch : public switch_::Switch, public Component {
public:
    HoermannLightSwitch(HoermannController *parent) : parent_(parent) {}
protected:
    void write_state(bool state) override;
};

class HoermannVentingSwitch : public switch_::Switch, public Component {
public:
    HoermannVentingSwitch(HoermannController *parent) : parent_(parent) {}
protected:
    void write_state(bool state) override;
};

class HoermannImpulseButton : public button::Button, public Component {
public:
    HoermannImpulseButton(HoermannController *parent) : parent_(parent) {}
protected:
    void press_action() override;
};

class HoermannEmergencyStopButton : public button::Button, public Component {
public:
    HoermannEmergencyStopButton(HoermannController *parent) : parent_(parent) {}
protected:
    void press_action() override;
};


class HoermannController : public uart::UARTDevice, public Component {
public:
    void setup() override;
    void loop() override;
    void dump_config() override;

    void register_cover(HoermannCover *cover) { this->cover_ = cover; }
    void register_light_switch(HoermannLightSwitch *sw) { this->light_switch_ = sw; }
    void register_venting_switch(HoermannVentingSwitch *sw) { this->venting_switch_ = sw; }
    void register_impulse_button(HoermannImpulseButton *btn) { this->impulse_button_ = btn; }
    void register_emergency_stop_button(HoermannEmergencyStopButton *btn) { this->emergency_stop_button_ = btn; }
    
    void register_binary_sensor(const std::string &type, binary_sensor::BinarySensor *sensor);
    void trigger_action(HoermannAction action);
    void set_de_pin(GPIOPin *de_pin) { this->de_pin_ = de_pin; }

protected:
    void send_response();
    void parse_message(const uint8_t *buffer, uint8_t len);
    uint8_t crc8(const uint8_t *data, uint8_t len);

    HoermannCover *cover_{nullptr};
    HoermannLightSwitch *light_switch_{nullptr};
    HoermannVentingSwitch *venting_switch_{nullptr};
    HoermannImpulseButton *impulse_button_{nullptr};
    HoermannEmergencyStopButton *emergency_stop_button_{nullptr};
    binary_sensor::BinarySensor *error_sensor_{nullptr};
    binary_sensor::BinarySensor *prewarn_sensor_{nullptr};
    binary_sensor::BinarySensor *option_relay_sensor_{nullptr};
    
    GPIOPin *de_pin_;
    uint16_t slave_response_data_{RESPONSE_DEFAULT};
    bool tx_message_ready_{false};
    uint32_t last_message_received_ms_{0};
};


// Cover Implementation
inline cover::CoverTraits HoermannCover::get_traits() {
    auto traits = cover::CoverTraits();
    traits.set_supports_stop(true);
    return traits;
}

}  // namespace hoermann_controller
}  // namespace esphome

