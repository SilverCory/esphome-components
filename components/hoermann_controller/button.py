import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import button
from esphome.const import CONF_ID, CONF_TYPE
from . import hoermann_controller_ns, HoermannController

HoermannImpulseButton = hoermann_controller_ns.class_("HoermannImpulseButton", button.Button, cg.Component)
HoermannEmergencyStopButton = hoermann_controller_ns.class_("HoermannEmergencyStopButton", button.Button, cg.Component)

BUTTON_TYPES = {
    "impulse": HoermannImpulseButton,
    "emergency_stop": HoermannEmergencyStopButton,
}

CONFIG_SCHEMA = (
    button.button_schema(button.Button)
    .extend(
        {
            cv.GenerateID(): cv.declare_id(button.Button),
            cv.Required("hoermann_controller_id"): cv.use_id(HoermannController),
            cv.Required(CONF_TYPE): cv.enum(BUTTON_TYPES, lower=True),
        }
    )
    .extend(cv.COMPONENT_SCHEMA)
)

async def to_code(config):
    hub = await cg.get_variable(config["hoermann_controller_id"])
    button_class = BUTTON_TYPES[config[CONF_TYPE]]
    var = cg.new_Pvariable(config[CONF_ID], hub)

    await button.register_button(var, config)

    if config[CONF_TYPE] == "impulse":
        cg.add(hub.register_impulse_button(var))
    elif config[CONF_TYPE] == "emergency_stop":
        cg.add(hub.register_emergency_stop_button(var))

