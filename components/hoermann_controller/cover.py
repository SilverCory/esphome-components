import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import cover
from esphome.const import CONF_ID
from . import hoermann_controller_ns, HoermannController

HoermannCover = hoermann_controller_ns.class_("HoermannCover", cover.Cover, cg.Component)

CONFIG_SCHEMA = (
    cover.cover_schema(HoermannCover)
    .extend(
        {
            cv.Required("hoermann_controller_id"): cv.use_id(HoermannController),
        }
    )
    .extend(cv.COMPONENT_SCHEMA)
)

async def to_code(config):
    hub = await cg.get_variable(config["hoermann_controller_id"])
    var = cg.new_Pvariable(config[CONF_ID], hub)
    await cover.register_cover(var, config)
    cg.add(hub.register_cover(var))

