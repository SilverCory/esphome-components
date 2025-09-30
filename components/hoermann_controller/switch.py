import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import switch
from esphome.const import CONF_ID, CONF_TYPE
from . import hoermann_controller_ns, HoermannController

HoermannLightSwitch = hoermann_controller_ns.class_("HoermannLightSwitch", switch.Switch, cg.Component)
HoermannVentingSwitch = hoermann_controller_ns.class_("HoermannVentingSwitch", switch.Switch, cg.Component)

SWITCH_TYPES = {
    "light": HoermannLightSwitch,
    "venting": HoermannVentingSwitch,
}

CONFIG_SCHEMA = (
    switch.switch_schema(switch.Switch)
    .extend(
        {
            cv.GenerateID(): cv.declare_id(switch.Switch),
            cv.Required("hoermann_controller_id"): cv.use_id(HoermannController),
            cv.Required(CONF_TYPE): cv.enum(SWITCH_TYPES, lower=True),
        }
    )
    .extend(cv.COMPONENT_SCHEMA)
)


async def to_code(config):
    hub = await cg.get_variable(config["hoermann_controller_id"])
    switch_class = SWITCH_TYPES[config[CONF_TYPE]]
    var = cg.new_Pvariable(config[CONF_ID], hub)

    await switch.register_switch(var, config)

    if config[CONF_TYPE] == "light":
        cg.add(hub.register_light_switch(var))
    elif config[CONF_TYPE] == "venting":
        cg.add(hub.register_venting_switch(var))

