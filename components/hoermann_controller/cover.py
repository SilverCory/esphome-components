import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import cover
from esphome.const import CONF_ID
from . import HoermannController, hoermann_controller_ns

HoermannCover = hoermann_controller_ns.class_("HoermannCover", cover.Cover)

CONFIG_SCHEMA = cover.cover_schema(HoermannCover).extend({
    cv.GenerateID(): cv.declare_id(HoermannCover),
    cv.Required("hoermann_controller_id"): cv.use_id(HoermannController),
})

async def to_code(config):
    parent = await cg.get_variable(config["hoermann_controller_id"])
    var = cg.new_P(config[CONF_ID], parent)
    await cover.register_cover(var, config)
    cg.add(parent.set_cover(var))

