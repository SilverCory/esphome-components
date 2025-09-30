import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import binary_sensor
from esphome.const import CONF_ID, CONF_TYPE
from . import HoermannController

BINARY_SENSOR_TYPES = [
    "error",
    "prewarn",
    "option_relay",
]

CONFIG_SCHEMA = binary_sensor.binary_sensor_schema().extend({
    cv.Required("hoermann_controller_id"): cv.use_id(HoermannController),
    cv.Required(CONF_TYPE): cv.one_of(*BINARY_SENSOR_TYPES, lower=True),
})

async def to_code(config):
    parent = await cg.get_variable(config["hoermann_controller_id"])
    var = await binary_sensor.new_binary_sensor(config)
    
    cg.add(parent.register_binary_sensor(config[CONF_TYPE], var))

