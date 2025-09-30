import esphome.codegen as cg
import esphome.config_validation as cv
from esphome import pins
from esphome.components import uart  # Import the uart component
from esphome.const import CONF_ID, CONF_UART_ID

# Define the namespace for our component
hoermann_controller_ns = cg.esphome_ns.namespace("hoermann_controller")

# Define the main controller class from our C++ code
HoermannController = hoermann_controller_ns.class_(
    "HoermannController", cg.Component, cg.PollingComponent
)

# Define configuration variables
CONF_DE_PIN = "de_pin"

# Define the component's configuration schema
CONFIG_SCHEMA = (
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(HoermannController),
            # Use the correct reference to UARTComponent
            cv.Required(CONF_UART_ID): cv.use_id(uart.UARTComponent),
            cv.Required(CONF_DE_PIN): pins.gpio_output_pin_schema,
        }
    )
    .extend(cv.COMPONENT_SCHEMA)
)

# The function that writes the C++ code based on the YAML config
async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    
    # Get the UART bus and register this component as a device on it
    uart_bus = await cg.get_variable(config[CONF_UART_ID])
    cg.add(var.set_uart(uart_bus))

    # Get the DE pin and set it for the component
    de_pin = await cg.gpio_pin_expression(config[CONF_DE_PIN])
    cg.add(var.set_de_pin(de_pin))

