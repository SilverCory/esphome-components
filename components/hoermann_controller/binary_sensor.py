import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import binary_sensor
from esphome.const import CONF_ID, CONF_TYPE
from . import hoermann_controller_ns, HoermannController

# The cv.enum validator expects a dictionary.
BINARY_SENSOR_TYPES = {
    "error": None,
    "prewarn": None,
    "option_relay": None,
}

CONFIG_SCHEMA = (
    binary_sensor.binary_sensor_schema()
    .extend(
        {
            cv.GenerateID(): cv.declare_id(binary_sensor.BinarySensor),
            cv.Required("hoermann_controller_id"): cv.use_id(HoermannController),
            cv.Required(CONF_TYPE): cv.enum(BINARY_SENSOR_TYPES, lower=True),
        }
    )
    .extend(cv.COMPONENT_SCHEMA)
)


async def to_code(config):
    hub = await cg.get_variable(config["hoermann_controller_id"])
    var = cg.new_Pvariable(config[CONF_ID])
    await binary_sensor.register_binary_sensor(var, config)

    if config[CONF_TYPE] == "error":
        cg.add(hub.register_error_sensor(var))
    elif config[CONF_TYPE] == "prewarn":
        cg.add(hub.register_prewarn_sensor(var))
    elif config[CONF_TYPE] == "option_relay":
        cg.add(hub.register_option_relay_sensor(var))

