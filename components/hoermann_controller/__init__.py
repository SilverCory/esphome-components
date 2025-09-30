import esphome.codegen as cg
import esphome.config_validation as cv
from esphome import pins
from esphome.components import uart
from esphome.const import CONF_ID

# Define the namespace for our component
hoermann_controller_ns = cg.esphome_ns.namespace("hoermann_controller")

# Define the main controller class from our C++ code.
# It inherits from UARTDevice, which handles the UART communication loop.
HoermannController = hoermann_controller_ns.class_(
    "HoermannController", cg.Component, uart.UARTDevice
)

# Define configuration variables
CONF_DE_PIN = "de_pin"

# Define the component's configuration schema
# We extend the UART device schema to get all the UART settings for free.
CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(HoermannController),
    cv.Required(CONF_DE_PIN): pins.gpio_output_pin_schema,
}).extend(uart.UART_DEVICE_SCHEMA)

# The function that writes the C++ code based on the YAML config
async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await uart.register_uart_device(var, config)

    # Get the DE pin and set it for the component
    de_pin = await cg.gpio_pin_expression(config[CONF_DE_PIN])
    cg.add(var.set_de_pin(de_pin))

