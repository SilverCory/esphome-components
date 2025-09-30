import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import button
from esphome.const import CONF_ID, CONF_TYPE
from .. import hoermann_controller_ns, HoermannController

CONF_HOERMANN_CONTROLLER_ID = "hoermann_controller_id"

HoermannImpulseButton = hoermann_controller_ns.class_("HoermannImpulseButton", button.Button, cg.Component)
HoermannEmergencyStopButton = hoermann_controller_ns.class_("HoermannEmergencyStopButton", button.Button, cg.Component)

BUTTON_TYPES = {
    "impulse": HoermannImpulseButton,
    "emergency_stop": HoermannEmergencyStopButton,
}

CONFIG_SCHEMA = cv.typed_schema(
    {
        "impulse": button.button_schema(HoermannImpulseButton).extend({
            cv.GenerateID(CONF_HOERMANN_CONTROLLER_ID): cv.use_id(HoermannController),
        }),
        "emergency_stop": button.button_schema(HoermannEmergencyStopButton).extend({
            cv.GenerateID(CONF_HOERMANN_CONTROLLER_ID): cv.use_id(HoermannController),
        }),
    },
    key=CONF_TYPE,
)

async def to_code(config):
    var = await button.new_button(config)
    await cg.register_component(var, config)

    controller = await cg.get_variable(config[CONF_HOERMANN_CONTROLLER_ID])
    cg.add(var.set_controller(controller))
