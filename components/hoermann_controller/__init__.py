import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import uart, cover, switch, button, binary_sensor
from esphome.const import (
    CONF_ID,
    CONF_TYPE,
    CONF_NAME,
    CONF_DEVICE_CLASS,
    CONF_ICON,
)
from esphome import pins

# --- Constants ---
CONF_DE_PIN = "de_pin"
CONF_HOERMANN_CONTROLLER_ID = "hoermann_controller_id"

# --- Component Namespace and Classes ---
hoermann_controller_ns = cg.esphome_ns.namespace("hoermann_controller")
HoermannController = hoermann_controller_ns.class_("HoermannController", uart.UARTDevice)
HoermannCover = hoermann_controller_ns.class_("HoermannCover", cover.Cover)
HoermannLightSwitch = hoermann_controller_ns.class_("HoermannLightSwitch", switch.Switch)
HoermannVentingSwitch = hoermann_controller_ns.class_("HoermannVentingSwitch", switch.Switch)
HoermannImpulseButton = hoermann_controller_ns.class_("HoermannImpulseButton", button.Button)
HoermannEmergencyStopButton = hoermann_controller_ns.class_("HoermannEmergencyStopButton", button.Button)

# --- Type Mappings ---
SWITCH_TYPES = {
    "light": HoermannLightSwitch,
    "venting": HoermannVentingSwitch,
}
BUTTON_TYPES = {
    "impulse": HoermannImpulseButton,
    "emergency_stop": HoermannEmergencyStopButton,
}
BINARY_SENSOR_TYPES = {
    "error": "register_error_sensor",
    "prewarn": "register_prewarn_sensor",
    "option_relay": "register_option_relay_sensor",
}

# --- Schemas ---
# Main component schema
CONFIG_SCHEMA = uart.UART_DEVICE_SCHEMA.extend({
    cv.GenerateID(): cv.declare_id(HoermannController),
    cv.Required(CONF_DE_PIN): pins.gpio_output_pin_schema,
})

# Platform schemas defined directly
PLATFORM_SCHEMA_COVER = cover.cover_schema(HoermannCover).extend({
    cv.GenerateID(CONF_HOERMANN_CONTROLLER_ID): cv.use_id(HoermannController),
})
PLATFORM_SCHEMA_SWITCH = switch.switch_schema(switch.Switch).extend({
    cv.GenerateID(CONF_HOERMANN_CONTROLLER_ID): cv.use_id(HoermannController),
    cv.Required(CONF_TYPE): cv.enum(SWITCH_TYPES, lower=True),
})
PLATFORM_SCHEMA_BUTTON = button.button_schema(button.Button).extend({
    cv.GenerateID(CONF_HOERMANN_CONTROLLER_ID): cv.use_id(HoermannController),
    cv.Required(CONF_TYPE): cv.enum(BUTTON_TYPES, lower=True),
})
PLATFORM_SCHEMA_BINARY_SENSOR = binary_sensor.binary_sensor_schema().extend({
    cv.GenerateID(): cv.declare_id(binary_sensor.BinarySensor),
    cv.GenerateID(CONF_HOERMANN_CONTROLLER_ID): cv.use_id(HoermannController),
    cv.Required(CONF_TYPE): cv.enum(BINARY_SENSOR_TYPES, lower=True),
})


# --- Code Generation ---
async def to_code(config):
    var = await cg.get_variable(config[CONF_ID])
    await uart.register_uart_device(var, config)

    de_pin = await cg.gpio_pin_expression(config[CONF_DE_PIN])
    cg.add(var.set_de_pin(de_pin))

