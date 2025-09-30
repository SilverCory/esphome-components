import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import button
from esphome.const import CONF_ID, CONF_TYPE
from . import hoermann_controller_ns, HoermannController

CONF_HOERMANN_CONTROLLER_ID = "hoermann_controller_id"

HoermannImpulseButton = hoermann_controller_ns.class_("HoermannImpulseButton", button.Button)
HoermannEmergencyStopButton = hoermann_controller_ns.class_("HoermannEmergencyStopButton", button.Button)

BUTTON_TYPES = {
    "impulse": HoermannImpulseButton,
    "emergency_stop": HoermannEmergencyStopButton,
}

CONFIG_SCHEMA = button.button_schema(button.Button).extend({
    cv.GenerateID(CONF_HOERMANN_CONTROLLER_ID): cv.use_id(HoermannController),
    cv.Required(CONF_TYPE): cv.enum(BUTTON_TYPES, lower=True),
})

async def to_code(config):
    parent = await cg.get_variable(config[CONF_HOERMANN_CONTROLLER_ID])
    button_class = BUTTON_TYPES[config[CONF_TYPE]]
    var = cg.new_P(config[CONF_ID], parent)
    await button.register_button(var, config)

    if config[CONF_TYPE] == "impulse":
        cg.add(parent.register_impulse_button(var))
    elif config[CONF_TYPE] == "emergency_stop":
        cg.add(parent.register_emergency_stop_button(var))

