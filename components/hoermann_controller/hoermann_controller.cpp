#include "hoermann_controller.h"
#include "esphome/core/log.h"

namespace esphome {
namespace hoermann_controller {

static const char *const TAG = "hoermann_controller";

// Constants from the PIC firmware
static const uint8_t BROADCAST_ADDR = 0x00;
static const uint8_t MASTER_ADDR = 0x80;
static const uint8_t UAP1_ADDR = 0x28;
static const uint8_t UAP1_TYPE = 0x14;
static const uint8_t CMD_SLAVE_SCAN = 0x01;
static const uint8_t CMD_SLAVE_STATUS_REQUEST = 0x20;
static const uint8_t CMD_SLAVE_STATUS_RESPONSE = 0x29;
static const uint32_t INTER_BYTE_TIMEOUT_MS = 5;
static const uint32_t RESPONSE_DELAY_MS = 3;

// CRC8 lookup table for polynomial 0x07, from the original PIC firmware
static const uint8_t CRC8_TABLE[256] = {
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
    0xDE, 0xD9, 0xD0, 0xD7, 0xC2, 0xC5, 0xCC, 0xCB, 0E6, 0xE1, 0xE8, 0xEF, 0xFA, 0xFD, 0xF4, 0xF3
};

void HoermannController::setup() {
    ESP_LOGCONFIG(TAG, "Setting up Hörmann RS-485 Controller...");
    this->de_pin_->setup();
    this->de_pin_->digital_write(false); // Set to receive mode
    this->rx_buffer_.reserve(20);
    this->tx_buffer_.reserve(20);
    this->last_byte_time_ = millis();
}

void HoermannController::dump_config() {
    ESP_LOGCONFIG(TAG, "Hörmann RS-485 Controller:");
    LOG_PIN("  DE Pin: ", this->de_pin_);
    LOG_UART_DEVICE(this);
    LOG_COVER("  ", "Cover", this->cover_);
    LOG_SWITCH("  ", "Light Switch", this->light_switch_);
    LOG_SWITCH("  ", "Venting Switch", this->venting_switch_);
    LOG_BINARY_SENSOR("  ", "Error Sensor", this->error_sensor_);
    LOG_BINARY_SENSOR("  ", "Prewarn Sensor", this->prewarn_sensor_);
    LOG_BINARY_SENSOR("  ", "Option Relay Sensor", this->option_relay_sensor_);
    LOG_BUTTON("  ", "Impulse Button", this->impulse_button_);
    LOG_BUTTON("  ", "Emergency Stop Button", this->estop_button_);
}

void HoermannController::loop() {
    // Check for pending response and if it's time to send it
    if (this->tx_message_ready_ && millis() >= this->response_due_time_) {
        send_response();
        this->tx_message_ready_ = false;
    }

    // If we are transmitting, don't read
    if (this->tx_message_ready_ || this->de_pin_->digital_read()) {
        return;
    }

    // Message framing detection using inter-byte timeout
    if (!this->rx_buffer_.empty() && (millis() - this->last_byte_time_ > INTER_BYTE_TIMEOUT_MS)) {
        ESP_LOGD(TAG, "Message timeout, processing buffer of size %d", this->rx_buffer_.size());
        handle_message();
        this->rx_buffer_.clear();
    }
    
    // Read all available bytes into buffer
    while (available()) {
        uint8_t byte;
        read_byte(&byte);
        this->rx_buffer_.push_back(byte);
        this->last_byte_time_ = millis();
    }
}

void HoermannController::handle_message() {
    if (this->rx_buffer_.size() < 3) {
        ESP_LOGW(TAG, "Ignoring runt frame of size %d", this->rx_buffer_.size());
        return;
    }

    // Verify CRC
    if (calc_crc8(this->rx_buffer_.data(), this->rx_buffer_.size()) != 0x00) {
        ESP_LOGW(TAG, "CRC8 check failed for received message.");
        return;
    }
    
    // Process based on destination address
    uint8_t addr = this->rx_buffer_[0];
    if (addr == BROADCAST_ADDR) {
        parse_broadcast(this->rx_buffer_);
    } else if (addr == UAP1_ADDR) {
        prepare_response(this->rx_buffer_);
    }
}

void HoermannController::parse_broadcast(const std::vector<uint8_t> &data) {
    uint8_t length = data[1] & 0x0F;
    if (length != 0x02 || data.size() < 4) return;

    uint16_t status = (uint16_t)data[3] << 8 | data[2];
    if (status == this->last_broadcast_status_) return; // No change
    
    this->last_broadcast_status_ = status;
    ESP_LOGD(TAG, "New Broadcast Status: 0x%04X", status);

    // Update cover state
    if (this->cover_) {
        if ((status & 0x0001) == 0x0001) this->cover_->position = cover::COVER_OPEN;
        else if ((status & 0x0002) == 0x0002) this->cover_->position = cover::COVER_CLOSED;
        else this->cover_->position = 0.5f; // Partially open/stopped

        if ((status & 0x0060) == 0x0040) this->cover_->current_operation = cover::COVER_OPERATION_OPENING;
        else if ((status & 0x0060) == 0x0060) this->cover_->current_operation = cover::COVER_OPERATION_CLOSING;
        else this->cover_->current_operation = cover::COVER_OPERATION_IDLE;
        
        this->cover_->publish_state(false);
    }
    // Update other sensors
    if (this->light_switch_) this->light_switch_->publish_state((status & 0x0008) != 0);
    if (this->venting_switch_) this->venting_switch_->publish_state((status & 0x0080) != 0);
    if (this->error_sensor_) this->error_sensor_->publish_state((status & 0x0010) != 0);
    if (this->option_relay_sensor_) this->option_relay_sensor_->publish_state((status & 0x0004) != 0);
    if (this->prewarn_sensor_) this->prewarn_sensor_->publish_state((status & 0x0100) != 0);
}

void HoermannController::prepare_response(const std::vector<uint8_t> &data) {
    uint8_t length = data[1] & 0x0F;
    uint8_t counter = (data[1] & 0xF0) + 0x10;
    this->tx_buffer_.clear();

    if (length == 0x02 && data[2] == CMD_SLAVE_SCAN) {
        this->tx_buffer_ = {MASTER_ADDR, (uint8_t)(0x02 | counter), UAP1_TYPE, UAP1_ADDR};
    } else if (length == 0x01 && data[2] == CMD_SLAVE_STATUS_REQUEST) {
        uint8_t resp_l = this->slave_response_data_ & 0xFF;
        uint8_t resp_h = (this->slave_response_data_ >> 8) & 0xFF;
        this->tx_buffer_ = {MASTER_ADDR, (uint8_t)(0x03 | counter), CMD_SLAVE_STATUS_RESPONSE, resp_l, resp_h};
        this->slave_response_data_ = RESPONSE_DEFAULT; // Reset after preparing response
    } else {
        return; // Not a command we handle
    }

    this->tx_buffer_.push_back(calc_crc8(this->tx_buffer_.data(), this->tx_buffer_.size()));
    this->tx_message_ready_ = true;
    this->response_due_time_ = millis() + RESPONSE_DELAY_MS; // Schedule response
}

void HoermannController::send_response() {
    this->de_pin_->digital_write(true); // Set to transmit mode
    delayMicroseconds(100); // Allow DE pin to settle

    // Send Sync Break: A low pulse longer than a character frame
    // 1 char = 10 bits (start + 8 data + stop). At 19200 bps, bit time = ~52us. Frame time = ~520us
    pinMode(this->get_tx_pin()->get_pin(), OUTPUT);
    digitalWrite(this->get_tx_pin()->get_pin(), LOW);
    delayMicroseconds(600);
    
    // UART write re-initializes the pin for us
    this->write_array(this->tx_buffer_);
    this->flush(); // Wait for all bytes to be sent

    this->de_pin_->digital_write(false); // Set back to receive mode
    ESP_LOGD(TAG, "Sent response: %s", format_hex_pretty(this->tx_buffer_).c_str());
}

void HoermannController::trigger_action(SlaveResponse action) {
    ESP_LOGD(TAG, "Action queued: 0x%04X", action);
    this->slave_response_data_ = action;
}

void HoermannController::trigger_stop_action() {
    // Stop is only triggered if the door is currently moving
    if (this->cover_ && (this->cover_->current_operation != cover::COVER_OPERATION_IDLE)) {
        this->slave_response_data_ = RESPONSE_STOP;
        ESP_LOGD(TAG, "Stop action queued because door is moving.");
    }
}

uint8_t HoermannController::calc_crc8(const uint8_t *p_data, uint8_t length) {
    uint8_t crc = 0xF3; // Initial value
    for (uint8_t i = 0; i < length; i++) {
        crc = CRC8_TABLE[p_data[i] ^ crc];
    }
    return crc;
}

} // namespace hoermann_controller
} // namespace esphome

