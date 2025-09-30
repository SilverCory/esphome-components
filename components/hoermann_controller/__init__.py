import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import uart
from esphome.const import CONF_ID

# Define a local constant for the DE pin configuration key
CONF_DE_PIN = "de_pin"

hoermann_controller_ns = cg.esphome_ns.namespace("hoermann_controller")
HoermannController = hoermann_controller_ns.class_("HoermannController", cg.Component, uart.UARTDevice)

CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(HoermannController),
    cv.Required(CONF_DE_PIN): cv.gpio_pin_expression,
}).extend(uart.UART_DEVICE_SCHEMA)

async def to_code(config):
    var = cg.new_P(config[CONF_ID])
    await cg.register_component(var, config)
    await uart.register_uart_device(var, config)
    
    de_pin = await cg.gpio_pin_expression(config[CONF_DE_PIN])
    cg.add(var.set_de_pin(de_pin))

