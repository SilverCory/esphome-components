import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import binary_sensor
from esphome.const import CONF_ID, CONF_TYPE
from . import hoermann_controller_ns, HoermannController

CONF_HOERMANN_CONTROLLER_ID = "hoermann_controller_id"

BINARY_SENSOR_TYPES = {
    "error": "register_error_sensor",
    "prewarn": "register_prewarn_sensor",
    "option_relay": "register_option_relay_sensor",
}

CONFIG_SCHEMA = binary_sensor.binary_sensor_schema().extend({
    cv.GenerateID(): cv.declare_id(binary_sensor.BinarySensor),
    cv.GenerateID(CONF_HOERMANN_CONTROLLER_ID): cv.use_id(HoermannController),
    cv.Required(CONF_TYPE): cv.enum(BINARY_SENSOR_TYPES, lower=True),
})

async def to_code(config):
    parent = await cg.get_variable(config[CONF_HOERMANN_CONTROLLER_ID])
    var = await binary_sensor.new_binary_sensor(config)
    
    registration_func = getattr(parent, BINARY_SENSOR_TYPES[config[CONF_TYPE]])
    cg.add(registration_func(var))

