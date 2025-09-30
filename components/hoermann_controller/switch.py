import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import switch_
from esphome.const import CONF_ID, CONF_TYPE
from . import HoermannController, hoermann_controller_ns

HoermannLightSwitch = hoermann_controller_ns.class_("HoermannLightSwitch", switch_.Switch)
HoermannVentingSwitch = hoermann_controller_ns.class_("HoermannVentingSwitch", switch_.Switch)

SWITCH_TYPES = {
    "light": HoermannLightSwitch,
    "venting": HoermannVentingSwitch,
}

CONFIG_SCHEMA = switch_.switch_schema().extend({
    cv.Required("hoermann_controller_id"): cv.use_id(HoermannController),
    cv.Required(CONF_TYPE): cv.enum(SWITCH_TYPES, lower=True),
})

async def to_code(config):
    parent = await cg.get_variable(config["hoermann_controller_id"])
    switch_class = SWITCH_TYPES[config[CONF_TYPE]]
    var = cg.new_P(config[CONF_ID], parent)
    await switch_.register_switch(var, config)

    if config[CONF_TYPE] == "light":
        cg.add(parent.register_light_switch(var))
    elif config[CONF_TYPE] == "venting":
        cg.add(parent.register_venting_switch(var))

