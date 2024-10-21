import esphome.codegen as cg
from esphome.components import binary_sensor
import esphome.config_validation as cv
from esphome.const import CONF_ID

from . import CONF_VOTRONIC_ID, Votronic

DEPENDENCIES = ["votronic"]

CODEOWNERS = ["@syssi"]

CONF_CHARGING = "charging"
CONF_DISCHARGING = "discharging"
CONF_CONTROLLER_ACTIVE = "controller_active"
CONF_CURRENT_REDUCTION = "current_reduction"
CONF_AES_ACTIVE = "aes_active"
CONF_PV_CONTROLLER_ACTIVE = "pv_controller_active"
CONF_PV_CURRENT_REDUCTION = "pv_current_reduction"
CONF_PV_AES_ACTIVE = "pv_aes_active"

BINARY_SENSORS = [
    CONF_CHARGING,
    CONF_DISCHARGING,
    CONF_CONTROLLER_ACTIVE,
    CONF_CURRENT_REDUCTION,
    CONF_AES_ACTIVE,
    CONF_PV_CONTROLLER_ACTIVE,
    CONF_PV_CURRENT_REDUCTION,
    CONF_PV_AES_ACTIVE,
]

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(CONF_VOTRONIC_ID): cv.use_id(Votronic),
        cv.Optional(CONF_CHARGING): binary_sensor.binary_sensor_schema(
            icon="mdi:battery-charging",
        ),
        cv.Optional(CONF_DISCHARGING): binary_sensor.binary_sensor_schema(
            icon="mdi:power-plug",
        ),
        cv.Optional(CONF_CONTROLLER_ACTIVE): binary_sensor.binary_sensor_schema(
            icon="mdi:power",
        ),
        cv.Optional(CONF_CURRENT_REDUCTION): binary_sensor.binary_sensor_schema(
            icon="mdi:car-speed-limiter",
        ),
        cv.Optional(CONF_AES_ACTIVE): binary_sensor.binary_sensor_schema(
            icon="mdi:export",
        ),
        cv.Optional(CONF_PV_CONTROLLER_ACTIVE): binary_sensor.binary_sensor_schema(
            icon="mdi:power",
        ),
        cv.Optional(CONF_PV_CURRENT_REDUCTION): binary_sensor.binary_sensor_schema(
            icon="mdi:car-speed-limiter",
        ),
        cv.Optional(CONF_PV_AES_ACTIVE): binary_sensor.binary_sensor_schema(
            icon="mdi:export",
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
