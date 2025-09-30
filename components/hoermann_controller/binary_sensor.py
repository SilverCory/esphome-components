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
    cv.GenerateID(): cv.declare_id(binary_sensor.BinarySensor),
    cv.Required("hoermann_controller_id"): cv.use_id(HoermannController),
    cv.Required(CONF_TYPE): cv.enum(BINARY_SENSOR_TYPES, lower=True),
})

async def to_code(config):
    parent = await cg.get_variable(config["hoermann_controller_id"])
    var = cg.new_P(config[CONF_ID])
    await binary_sensor.register_binary_sensor(var, config)
    
    sensor_type = config[CONF_TYPE]
    cg.add(parent.register_binary_sensor(sensor_type, var))

