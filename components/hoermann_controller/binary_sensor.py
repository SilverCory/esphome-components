import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import binary_sensor
from esphome.const import CONF_TYPE

from . import hoermann_controller_ns, HoermannController

CONF_HOERMANN_CONTROLLER_ID = "hoermann_controller_id"

# The cv.enum validator requires a dictionary
BINARY_SENSOR_TYPES = {
    "error": None,
    "prewarn": None,
    "option_relay": None,
}

CONFIG_SCHEMA = (
    binary_sensor.binary_sensor_schema()
    .extend(
        {
            cv.GenerateID(CONF_HOERMANN_CONTROLLER_ID): cv.use_id(HoermannController),
            cv.Required(CONF_TYPE): cv.enum(BINARY_SENSOR_TYPES, lower=True),
        }
    )
    .extend(cv.COMPONENT_SCHEMA)
)

async def to_code(config):
    parent = await cg.get_variable(config[CONF_HOERMANN_CONTROLLER_ID])
    var = await binary_sensor.new_binary_sensor(config)
    cg.add(parent.register_binary_sensor(config[CONF_TYPE], var))

