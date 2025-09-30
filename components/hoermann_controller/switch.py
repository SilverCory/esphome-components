import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import switch_
from esphome.const import CONF_ID, CONF_TYPE
from . import hoermann_controller_ns, HoermannController

CONF_HOERMANN_CONTROLLER_ID = "hoermann_controller_id"

HoermannLightSwitch = hoermann_controller_ns.class_("HoermannLightSwitch", switch_.Switch)
HoermannVentingSwitch = hoermann_controller_ns.class_("HoermannVentingSwitch", switch_.Switch)

SWITCH_TYPES = {
    "light": HoermannLightSwitch,
    "venting": HoermannVentingSwitch,
}

CONFIG_SCHEMA = switch_.switch_schema(switch_.Switch).extend({
    cv.GenerateID(CONF_HOERMANN_CONTROLLER_ID): cv.use_id(HoermannController),
    cv.Required(CONF_TYPE): cv.enum(SWITCH_TYPES, lower=True),
})

async def to_code(config):
    parent = await cg.get_variable(config[CONF_HOERMANN_CONTROLLER_ID])
    
    switch_class = SWITCH_TYPES[config[CONF_TYPE]]
    var = cg.new_P(config[CONF_ID], parent)
    await switch_.register_switch(var, config)

    if config[CONF_TYPE] == "light":
        cg.add(parent.register_light_switch(var))
    elif config[CONF_TYPE] == "venting":
        cg.add(parent.register_venting_switch(var))

