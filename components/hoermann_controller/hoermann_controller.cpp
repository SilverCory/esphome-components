#include "hoermann_controller.h"
#include "esphome/core/log.h"
#include "esphome/core/hal.h"

namespace esphome {
namespace hoermann_controller {

static const uint8_t BROADCAST_ADDR = 0x00;
static const uint8_t MASTER_ADDR = 0x80;
static const uint8_t UAP1_ADDR = 0x28;
static const uint8_t UAP1_TYPE = 0x14;
static const uint8_t CMD_SLAVE_SCAN = 0x01;
static const uint8_t CMD_SLAVE_STATUS_REQUEST = 0x20;
static const uint8_t CMD_SLAVE_STATUS_RESPONSE = 0x29;
static const uint8_t CRC8_INITIAL_VALUE = 0xF3;

static const uint8_t CRCTABLE[256] = {
    0x00, 0x07, 0x0E, 0x09, 0x1C, 0x1B, 0x12, 0x15, 0x38, 0x3F, 0x36, 0x31, 0x24, 0x23, 0x2A, 0x2D,
    0x70, 0x77, 0x7E, 0x79, 0x6C, 0x6B, 0x62, 0x65, 0x48, 0x4F, 0x46, 0x41, 0x54, 0x53, 0x5A, 0x5D,
    0xE0, 0xE7, 0xEE, 0xE9, 0xFC, 0xFB, 0xF2, 0xF5, 0xD8, 0xDF, 0xD6, 0xD1, 0xC4, 0xC3, 0xCA, 0xCD,
    0x90, 0x97, 0x9E, 0x99, 0x8C, 0x8B, 0x82, 0x85, 0xA8, 0xAF, 0xA6, 0xA1, 0xB4, 0xB3, 0xBA, 0xBD,
    0xC7, 0xC0, 0xC9, 0xCE, 0xDB, 0xDC, 0xD5, 0xD2, 0xFF, 0xF8, 0xF1, 0xF6, 0xE3, 0xE4, 0xED, 0xEA,
    0xB7, 0xB0, 0xB9, 0xBE, 0xAB, 0xAC, 0xA5, 0xA2, 0x8F, 0x88, 0x81, 0x86, 0x93, 0x94, 0x9D, 0x9A,
    0x27, 0x20, 0x29, 0x2E, 0x3B, 0x3C, 0x35, 0x32, 0x1F, 0x18, 0x11, 0x16, 0x03, 0x04, 0x0D, 0x0A,
    0x57, 0x50, 0x59, 0x5E, 0x4B, 0x4C, 0x45, 0x42, 0x6F, 0x68, 0x61, 0x66, 0x73, 0x74, 0x7D, 0x7A,
    0x89, 0x8E, 0x87, 0x80, 0x95, 0x92, 0x9B, 0x9C, 0xB1, 0xB6, 0xBF, 0xB8, 0xAD, 0xAA, 0xA3, 0xA4,
    0xF9, 0xFE, 0xF7, 0xF0, 0xE5, 0xE2, 0xEB, 0xEC, 0xC1, 0xC6, 0xCF, 0xC8, 0xDD, 0xDA, 0xD3, 0xD4,
    0x69, 0x6E, 0x67, 0x60, 0x75, 0x72, 0x7B, 0x7C, 0x51, 0x56, 0x5F, 0x58, 0x4D, 0x4A, 0x43, 0x44,
    0x19, 0x1E, 0x17, 0x10, 0x05, 0x02, 0x0B, 0x0C, 0x21, 0x26, 0x2F, 0x28, 0x3D, 0x3A, 0x33, 0x34,
    0x4E, 0x49, 0x40, 0x47, 0x52, 0x55, 0x5C, 0x5B, 0x76, 0x71, 0x78, 0x7F, 0x6A, 0x6D, 0x64, 0x63,
    0x3E, 0x39, 0x30, 0x37, 0x22, 0x25, 0x2C, 0x2B, 0x06, 0x01, 0x08, 0x0F, 0x1A, 0x1D, 0x14, 0x13,
    0xAE, 0xA9, 0xA0, 0xA7, 0xB2, 0xB5, 0xBC, 0xBB, 0x96, 0x91, 0x98, 0x9F, 0x8A, 0x8D, 0x84, 0x83,
    0xDE, 0xD9, 0xD0, 0xD7, 0xC2, 0xC5, 0xCC, 0xCB, 0xE6, 0xE1, 0xE8, 0xEF, 0xFA, 0xFD, 0xF4, 0xF3
};

void HoermannController::setup() {
    this->de_pin_->setup();
    this->de_pin_->digital_write(false); // Set to receive mode
    this->last_message_received_ms_ = millis();
}

void HoermannController::dump_config() {
    ESP_LOGCONFIG(TAG, "Hörmann Controller:");
    LOG_PIN("  DE Pin: ", this->de_pin_);
    ESP_LOGCONFIG(TAG, "  UART Bus configured");
}

void HoermannController::loop() {
    uint8_t buffer[32];
    
    // Simple timing-based message separation
    if (this->available() && (millis() - this->last_message_received_ms_ > 10)) {
        int len = this->read_array(buffer, sizeof(buffer));
        if (len > 0) {
            this->last_message_received_ms_ = millis();
            // Assuming the first byte is often 0 due to framing break
            uint8_t *start_of_message = buffer;
            uint8_t msg_len = len;
            if (buffer[0] == 0x00 && len > 1) {
                start_of_message = buffer + 1;
                msg_len = len - 1;
            }

            if (this->crc8(start_of_message, msg_len) == 0) {
                parse_message(start_of_message, msg_len);
            } else {
                ESP_LOGW(TAG, "Received invalid checksum");
            }
        }
    }

    if (this->tx_message_ready_) {
        // Wait a few ms before replying
        if (millis() - this->last_message_received_ms_ > 3) {
            this->send_response();
            this->tx_message_ready_ = false;
        }
    }
}

void HoermannController::parse_message(const uint8_t *buffer, uint8_t len) {
    uint8_t msg_len = buffer[1] & 0x0F;
    
    if (buffer[0] == BROADCAST_ADDR && msg_len == 0x02) {
        uint16_t status = (buffer[3] << 8) | buffer[2];
        
        bool is_open = (status & 0x01) != 0;
        bool is_closed = (status & 0x02) != 0;
        bool is_opening = (status & 0x60) == 0x40;
        bool is_closing = (status & 0x60) == 0x60;

        if (this->cover_) {
            if (is_open) this->cover_->position = 1.0;
            else if (is_closed) this->cover_->position = 0.0;
            else if (is_opening) this->cover_->position = 0.5;
            else if (is_closing) this->cover_->position = 0.5;
            else this->cover_->position = 0.0;
            this->cover_->current_operation = is_opening ? cover::COVER_OPERATION_OPENING :
                                              is_closing ? cover::COVER_OPERATION_CLOSING :
                                              cover::COVER_OPERATION_IDLE;
            this->cover_->publish_state();
        }

        if (this->light_switch_) this->light_switch_->publish_state((status & 0x08) != 0);
        if (this->venting_switch_) this->venting_switch_->publish_state((status & 0x80) != 0);
        if (this->error_sensor_) this->error_sensor_->publish_state((status & 0x10) != 0);
        if (this->prewarn_sensor_) this->prewarn_sensor_->publish_state((buffer[3] & 0x01) != 0);
        if (this->option_relay_sensor_) this->option_relay_sensor_->publish_state((status & 0x04) != 0);

    } else if (buffer[0] == UAP1_ADDR) {
        if (msg_len == 0x02 && buffer[2] == CMD_SLAVE_SCAN) {
            ESP_LOGD(TAG, "Responding to bus scan");
            this->tx_message_ready_ = true;
        } else if (msg_len == 0x01 && buffer[2] == CMD_SLAVE_STATUS_REQUEST) {
            ESP_LOGD(TAG, "Responding to status request with action: 0x%04X", this->slave_response_data_);
            this->tx_message_ready_ = true;
        }
    }
}

void HoermannController::send_response() {
    uint8_t tx_buffer[8];
    uint8_t len = 0;

    if (this->slave_response_data_ != RESPONSE_DEFAULT) {
        tx_buffer[0] = MASTER_ADDR;
        tx_buffer[1] = 0x03 | 0x10;
        tx_buffer[2] = CMD_SLAVE_STATUS_RESPONSE;
        tx_buffer[3] = (uint8_t)this->slave_response_data_;
        tx_buffer[4] = (uint8_t)(this->slave_response_data_ >> 8);
        tx_buffer[5] = this->crc8(tx_buffer, 5);
        len = 6;
        this->slave_response_data_ = RESPONSE_DEFAULT;
    } else { // Bus scan response
        tx_buffer[0] = MASTER_ADDR;
        tx_buffer[1] = 0x02 | 0x10;
        tx_buffer[2] = UAP1_TYPE;
        tx_buffer[3] = UAP1_ADDR;
        tx_buffer[4] = this->crc8(tx_buffer, 4);
        len = 5;
    }

    // Set DE pin to enable transmit
    this->de_pin_->digital_write(true);
    
    // Manual sync break: Pull TX low for ~1ms. This is a robust way to create a break.
    this->tx_pin_->pin_mode(gpio::FLAG_OUTPUT);
    this->tx_pin_->digital_write(false);
    delay(2);
    this->tx_pin_->pin_mode(gpio::FLAG_INPUT); // Let UART take over again

    this->write_array(tx_buffer, len);
    this->flush();

    // Set DE pin back to receive
    this->de_pin_->digital_write(false);
}

void HoermannController::trigger_action(HoermannAction action) {
    switch (action) {
        case ACTION_STOP:
            this->slave_response_data_ = RESPONSE_IMPULSE;
            break;
        case ACTION_OPEN:
            this->slave_response_data_ = RESPONSE_OPEN;
            break;
        case ACTION_CLOSE:
            this->slave_response_data_ = RESPONSE_CLOSE;
            break;
        case ACTION_VENTING:
            this->slave_response_data_ = RESPONSE_VENTING;
            break;
        case ACTION_TOGGLE_LIGHT:
            this->slave_response_data_ = RESPONSE_TOGGLE_LIGHT;
            break;
        case ACTION_EMERGENCY_STOP:
            this->slave_response_data_ = RESPONSE_EMERGENCY_STOP;
            break;
        case ACTION_IMPULSE:
            this->slave_response_data_ = RESPONSE_IMPULSE;
            break;
    }
}

uint8_t HoermannController::crc8(const uint8_t *data, uint8_t len) {
    uint8_t crc = CRC8_INITIAL_VALUE;
    for (uint8_t i = 0; i < len; i++) {
        crc = CRCTABLE[data[i] ^ crc];
    }
    return crc;
}

void HoermannController::register_binary_sensor(const std::string &type, binary_sensor::BinarySensor *sensor) {
    if (type == "error") this->error_sensor_ = sensor;
    else if (type == "prewarn") this->prewarn_sensor_ = sensor;
    else if (type == "option_relay") this->option_relay_sensor_ = sensor;
}

// --- Cover Methods ---
void HoermannCover::setup() {}
void HoermannCover::loop() {}
void HoermannCover::dump_config() { ESP_LOGCONFIG(TAG, "Hörmann Cover"); }
void HoermannCover::control(const cover::CoverCall &call) {
    if (call.get_stop()) this->parent_->trigger_action(ACTION_STOP);
    if (call.get_position().has_value()) {
        if (*call.get_position() == cover::COVER_OPEN) this->parent_->trigger_action(ACTION_OPEN);
        else if (*call.get_position() == cover::COVER_CLOSED) this->parent_->trigger_action(ACTION_CLOSE);
    }
}

// --- Switch Methods ---
void HoermannLightSwitch::write_state(bool state) {
    this->parent_->trigger_action(ACTION_TOGGLE_LIGHT);
    publish_state(this->state);
}

void HoermannVentingSwitch::write_state(bool state) {
    if (state) this->parent_->trigger_action(ACTION_VENTING);
    else this->parent_->trigger_action(ACTION_CLOSE);
    publish_state(this->state);
}

// --- Button Methods ---
void HoermannImpulseButton::press_action() {
    this->parent_->trigger_action(ACTION_IMPULSE);
}

void HoermannEmergencyStopButton::press_action() {
    this->parent_->trigger_action(ACTION_EMERGENCY_STOP);
}


}  // namespace hoermann_controller
}  // namespace esphome

