import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import switch
from esphome.const import CONF_ID, CONF_TYPE
from .. import hoermann_controller_ns, HoermannController

CONF_HOERMANN_CONTROLLER_ID = "hoermann_controller_id"

HoermannLightSwitch = hoermann_controller_ns.class_("HoermannLightSwitch", switch.Switch, cg.Component)
HoermannVentingSwitch = hoermann_controller_ns.class_("HoermannVentingSwitch", switch.Switch, cg.Component)

SWITCH_TYPES = {
    "light": HoermannLightSwitch,
    "venting": HoermannVentingSwitch,
}

CONFIG_SCHEMA = cv.typed_schema(
    {
        "light": switch.switch_schema(HoermannLightSwitch).extend({
            cv.GenerateID(CONF_HOERMANN_CONTROLLER_ID): cv.use_id(HoermannController),
        }),
        "venting": switch.switch_schema(HoermannVentingSwitch).extend({
            cv.GenerateID(CONF_HOERMANN_CONTROLLER_ID): cv.use_id(HoermannController),
        }),
    },
    key=CONF_TYPE,
)

async def to_code(config):
    var = await switch.new_switch(config)
    await cg.register_component(var, config)

    controller = await cg.get_variable(config[CONF_HOERMANN_CONTROLLER_ID])
    cg.add(var.set_controller(controller))
