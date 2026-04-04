import esphome.codegen as cg
from esphome.components import binary_sensor
import esphome.config_validation as cv
from esphome.const import ENTITY_CATEGORY_DIAGNOSTIC

from . import CONF_VOTRONIC_BLE_ID, VOTRONIC_BLE_SCHEMA

DEPENDENCIES = ["votronic_ble"]

CODEOWNERS = ["@syssi"]

CONF_CHARGING = "charging"
CONF_DISCHARGING = "discharging"
CONF_CONTROLLER_ACTIVE = "controller_active"
CONF_CURRENT_REDUCTION = "current_reduction"
CONF_AES_ACTIVE = "aes_active"

ICON_CHARGING = "mdi:battery-charging"
ICON_DISCHARGING = "mdi:power-plug"
ICON_CONTROLLER_ACTIVE = "mdi:power"
ICON_CURRENT_REDUCTION = "mdi:car-speed-limiter"
ICON_AES_ACTIVE = "mdi:export"

BINARY_SENSOR_DEFS = {
    CONF_CHARGING: {
        "icon": ICON_CHARGING,
        "entity_category": ENTITY_CATEGORY_DIAGNOSTIC,
    },
    CONF_DISCHARGING: {
        "icon": ICON_DISCHARGING,
        "entity_category": ENTITY_CATEGORY_DIAGNOSTIC,
    },
    CONF_CONTROLLER_ACTIVE: {
        "icon": ICON_CONTROLLER_ACTIVE,
        "entity_category": ENTITY_CATEGORY_DIAGNOSTIC,
    },
    CONF_CURRENT_REDUCTION: {
        "icon": ICON_CURRENT_REDUCTION,
        "entity_category": ENTITY_CATEGORY_DIAGNOSTIC,
    },
    CONF_AES_ACTIVE: {
        "icon": ICON_AES_ACTIVE,
        "entity_category": ENTITY_CATEGORY_DIAGNOSTIC,
    },
}

CONFIG_SCHEMA = VOTRONIC_BLE_SCHEMA.extend(
    {
        cv.Optional(key): binary_sensor.binary_sensor_schema(**kwargs)
        for key, kwargs in BINARY_SENSOR_DEFS.items()
    }
)


async def to_code(config):
    hub = await cg.get_variable(config[CONF_VOTRONIC_BLE_ID])
    for key in BINARY_SENSOR_DEFS:
        if key in config:
            conf = config[key]
            sens = await binary_sensor.new_binary_sensor(conf)
            cg.add(getattr(hub, f"set_{key}_binary_sensor")(sens))
