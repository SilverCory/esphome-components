import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import uart
from esphome.const import CONF_ID
from esphome import pins

# Define a local constant for the DE pin configuration key
CONF_DE_PIN = "de_pin"

hoermann_controller_ns = cg.esphome_ns.namespace("hoermann_controller")
# Corrected inheritance: A UARTDevice is already a Component.
# The redundant inheritance was causing the circular dependency.
HoermannController = hoermann_controller_ns.class_("HoermannController", uart.UARTDevice)

# The schema for a UART device. uart.UART_DEVICE_SCHEMA already includes
# the necessary component schema, so extending it again is not needed.
CONFIG_SCHEMA = uart.UART_DEVICE_SCHEMA.extend({
    cv.GenerateID(): cv.declare_id(HoermannController),
    cv.Required(CONF_DE_PIN): pins.gpio_output_pin_schema,
})


async def to_code(config):
    var = await cg.get_variable(config[CONF_ID])
    await uart.register_uart_device(var, config)

    de_pin = await cg.gpio_pin_expression(config[CONF_DE_PIN])
    cg.add(var.set_de_pin(de_pin))

