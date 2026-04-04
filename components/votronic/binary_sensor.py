import esphome.codegen as cg
from esphome.components import binary_sensor
import esphome.config_validation as cv
from esphome.const import ENTITY_CATEGORY_DIAGNOSTIC

from . import CONF_VOTRONIC_ID, VOTRONIC_COMPONENT_SCHEMA

DEPENDENCIES = ["votronic"]

CODEOWNERS = ["@syssi"]

CONF_CHARGING = "charging"
CONF_DISCHARGING = "discharging"

CONF_CHARGER_CHARGING = "charger_charging"
CONF_CHARGER_DISCHARGING = "charger_discharging"
CONF_CHARGER_CONTROLLER_ACTIVE = "charger_controller_active"
CONF_CHARGER_CURRENT_REDUCTION = "charger_current_reduction"
CONF_CHARGER_AES_ACTIVE = "charger_aes_active"

CONF_CHARGING_CONVERTER_CHARGING = "charging_converter_charging"
CONF_CHARGING_CONVERTER_DISCHARGING = "charging_converter_discharging"
CONF_CHARGING_CONVERTER_CONTROLLER_ACTIVE = "charging_converter_controller_active"
CONF_CHARGING_CONVERTER_CURRENT_REDUCTION = "charging_converter_current_reduction"
CONF_CHARGING_CONVERTER_AES_ACTIVE = "charging_converter_aes_active"

CONF_PV_CONTROLLER_ACTIVE = "pv_controller_active"
CONF_PV_CURRENT_REDUCTION = "pv_current_reduction"
CONF_PV_AES_ACTIVE = "pv_aes_active"

BINARY_SENSOR_DEFS = {
    CONF_CHARGING: {
        "icon": "mdi:battery-charging",
        "entity_category": ENTITY_CATEGORY_DIAGNOSTIC,
    },
    CONF_DISCHARGING: {
        "icon": "mdi:power-plug",
        "entity_category": ENTITY_CATEGORY_DIAGNOSTIC,
    },
    CONF_CHARGER_CHARGING: {
        "icon": "mdi:battery-charging",
        "entity_category": ENTITY_CATEGORY_DIAGNOSTIC,
    },
    CONF_CHARGER_DISCHARGING: {
        "icon": "mdi:power-plug",
        "entity_category": ENTITY_CATEGORY_DIAGNOSTIC,
    },
    CONF_CHARGER_CONTROLLER_ACTIVE: {
        "icon": "mdi:power",
        "entity_category": ENTITY_CATEGORY_DIAGNOSTIC,
    },
    CONF_CHARGER_CURRENT_REDUCTION: {
        "icon": "mdi:car-speed-limiter",
        "entity_category": ENTITY_CATEGORY_DIAGNOSTIC,
    },
    CONF_CHARGER_AES_ACTIVE: {
        "icon": "mdi:export",
        "entity_category": ENTITY_CATEGORY_DIAGNOSTIC,
    },
    CONF_CHARGING_CONVERTER_CHARGING: {
        "icon": "mdi:battery-charging",
        "entity_category": ENTITY_CATEGORY_DIAGNOSTIC,
    },
    CONF_CHARGING_CONVERTER_DISCHARGING: {
        "icon": "mdi:power-plug",
        "entity_category": ENTITY_CATEGORY_DIAGNOSTIC,
    },
    CONF_CHARGING_CONVERTER_CONTROLLER_ACTIVE: {
        "icon": "mdi:power",
        "entity_category": ENTITY_CATEGORY_DIAGNOSTIC,
    },
    CONF_CHARGING_CONVERTER_CURRENT_REDUCTION: {
        "icon": "mdi:car-speed-limiter",
        "entity_category": ENTITY_CATEGORY_DIAGNOSTIC,
    },
    CONF_CHARGING_CONVERTER_AES_ACTIVE: {
        "icon": "mdi:export",
        "entity_category": ENTITY_CATEGORY_DIAGNOSTIC,
    },
    CONF_PV_CONTROLLER_ACTIVE: {
        "icon": "mdi:power",
        "entity_category": ENTITY_CATEGORY_DIAGNOSTIC,
    },
    CONF_PV_CURRENT_REDUCTION: {
        "icon": "mdi:car-speed-limiter",
        "entity_category": ENTITY_CATEGORY_DIAGNOSTIC,
    },
    CONF_PV_AES_ACTIVE: {
        "icon": "mdi:export",
        "entity_category": ENTITY_CATEGORY_DIAGNOSTIC,
    },
}

CONFIG_SCHEMA = VOTRONIC_COMPONENT_SCHEMA.extend(
    {
        cv.Optional(key): binary_sensor.binary_sensor_schema(**kwargs)
        for key, kwargs in BINARY_SENSOR_DEFS.items()
    }
)


async def to_code(config):
    hub = await cg.get_variable(config[CONF_VOTRONIC_ID])
    for key in BINARY_SENSOR_DEFS:
        if key in config:
            conf = config[key]
            sens = await binary_sensor.new_binary_sensor(conf)
            cg.add(getattr(hub, f"set_{key}_binary_sensor")(sens))
