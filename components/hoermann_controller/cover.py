import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import cover
from esphome.const import CONF_ID
from . import HoermannController, hoermann_controller_ns

HoermannCover = hoermann_controller_ns.class_("HoermannCover", cover.Cover, cg.Component)

CONFIG_SCHEMA = cover.cover_schema(HoermannCover).extend({
    cv.Required("hoermann_controller_id"): cv.use_id(HoermannController),
})

async def to_code(config):
    parent = await cg.get_variable(config["hoermann_controller_id"])
    var = cg.new_P(config[CONF_ID])
    await cg.register_component(var, config)
    await cover.register_cover(var, config)
    
    cg.add(var.set_parent(parent))
    cg.add(parent.register_cover(var))

