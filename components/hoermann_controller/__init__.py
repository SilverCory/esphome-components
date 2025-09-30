import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import uart
from esphome.const import CONF_ID
from esphome import pins

# Define a local constant for the DE pin configuration key
CONF_DE_PIN = "de_pin"

hoermann_controller_ns = cg.esphome_ns.namespace("hoermann_controller")
HoermannController = hoermann_controller_ns.class_("HoermannController", cg.Component, uart.UARTDevice)

CONFIG_SCHEMA = cv.All(
    uart.UART_DEVICE_SCHEMA.extend(
        {
            cv.GenerateID(): cv.declare_id(HoermannController),
            cv.Required(CONF_DE_PIN): pins.gpio_output_pin_schema,
        }
    )
)

async def to_code(config):
    # Retrieve the component instance that was declared by the schema
    var = await cg.get_variable(config[CONF_ID])
    
    # The component is already registered by the schema, so we just need to
    # register it as a UART device.
    await uart.register_uart_device(var, config)
    
    de_pin = await cg.gpio_pin_expression(config[CONF_DE_PIN])
    cg.add(var.set_de_pin(de_pin))

