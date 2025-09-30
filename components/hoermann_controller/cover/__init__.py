import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import cover
from esphome.const import CONF_ID
from .. import hoermann_controller_ns, HoermannController

CONF_HOERMANN_CONTROLLER_ID = "hoermann_controller_id"

HoermannCover = hoermann_controller_ns.class_("HoermannCover", cover.Cover, cg.Component)

CONFIG_SCHEMA = cover.cover_schema(HoermannCover).extend({
    cv.GenerateID(CONF_HOERMANN_CONTROLLER_ID): cv.use_id(HoermannController),
})

async def to_code(config):
    var = await cover.new_cover(config)
    await cg.register_component(var, config)

    controller = await cg.get_variable(config[CONF_HOERMANN_CONTROLLER_ID])
    cg.add(var.set_controller(controller))
