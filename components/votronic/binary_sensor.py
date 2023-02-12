import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import binary_sensor
from esphome.const import CONF_ICON, CONF_ID

from . import CONF_VOTRONIC_ID, Votronic

DEPENDENCIES = ["votronic"]

CODEOWNERS = ["@syssi"]

CONF_CHARGING = "charging"
CONF_DISCHARGING = "discharging"
CONF_AES_ACTIVE = "aes_active"
CONF_CURRENT_REDUCTION = "current_reduction"

ICON_CHARGING = "mdi:battery-charging"
ICON_DISCHARGING = "mdi:power-plug"
ICON_AES_ACTIVE = "mdi:export"
ICON_CURRENT_REDUCTION = "mdi:car-speed-limiter"

BINARY_SENSORS = [
    CONF_CHARGING,
    CONF_DISCHARGING,
    CONF_AES_ACTIVE,
    CONF_CURRENT_REDUCTION,
]

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(CONF_VOTRONIC_ID): cv.use_id(Votronic),
        cv.Optional(CONF_CHARGING): binary_sensor.BINARY_SENSOR_SCHEMA.extend(
            {
                cv.GenerateID(): cv.declare_id(binary_sensor.BinarySensor),
                cv.Optional(CONF_ICON, default=ICON_CHARGING): cv.icon,
            }
        ),
        cv.Optional(CONF_DISCHARGING): binary_sensor.BINARY_SENSOR_SCHEMA.extend(
            {
                cv.GenerateID(): cv.declare_id(binary_sensor.BinarySensor),
                cv.Optional(CONF_ICON, default=ICON_DISCHARGING): cv.icon,
            }
        ),
        cv.Optional(CONF_AES_ACTIVE): binary_sensor.BINARY_SENSOR_SCHEMA.extend(
            {
                cv.GenerateID(): cv.declare_id(binary_sensor.BinarySensor),
                cv.Optional(CONF_ICON, default=ICON_AES_ACTIVE): cv.icon,
            }
        ),
        cv.Optional(CONF_CURRENT_REDUCTION): binary_sensor.BINARY_SENSOR_SCHEMA.extend(
            {
                cv.GenerateID(): cv.declare_id(binary_sensor.BinarySensor),
                cv.Optional(CONF_ICON, default=ICON_CURRENT_REDUCTION): cv.icon,
            }
        ),
    }
)


async def to_code(config):
    hub = await cg.get_variable(config[CONF_VOTRONIC_ID])
    for key in BINARY_SENSORS:
        if key in config:
            conf = config[key]
            sens = cg.new_Pvariable(conf[CONF_ID])
            await binary_sensor.register_binary_sensor(sens, conf)
            cg.add(getattr(hub, f"set_{key}_binary_sensor")(sens))
