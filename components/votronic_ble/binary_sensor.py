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

BINARY_SENSORS = [
    CONF_CHARGING,
    CONF_DISCHARGING,
    CONF_CONTROLLER_ACTIVE,
    CONF_CURRENT_REDUCTION,
    CONF_AES_ACTIVE,
]

CONFIG_SCHEMA = VOTRONIC_BLE_SCHEMA.extend(
    {
        cv.Optional(CONF_CHARGING): binary_sensor.binary_sensor_schema(
            binary_sensor.BinarySensor,
            icon=ICON_CHARGING,
            entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
        ),
        cv.Optional(CONF_DISCHARGING): binary_sensor.binary_sensor_schema(
            binary_sensor.BinarySensor,
            icon=ICON_DISCHARGING,
            entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
        ),
        cv.Optional(CONF_CONTROLLER_ACTIVE): binary_sensor.binary_sensor_schema(
            binary_sensor.BinarySensor,
            icon=ICON_CONTROLLER_ACTIVE,
            entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
        ),
        cv.Optional(CONF_CURRENT_REDUCTION): binary_sensor.binary_sensor_schema(
            binary_sensor.BinarySensor,
            icon=ICON_CURRENT_REDUCTION,
            entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
        ),
        cv.Optional(CONF_AES_ACTIVE): binary_sensor.binary_sensor_schema(
            binary_sensor.BinarySensor,
            icon=ICON_AES_ACTIVE,
            entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
        ),
    }
)


async def to_code(config):
    hub = await cg.get_variable(config[CONF_VOTRONIC_BLE_ID])
    for key in BINARY_SENSORS:
        if key in config:
            conf = config[key]
            sens = await binary_sensor.new_binary_sensor(conf)
            cg.add(getattr(hub, f"set_{key}_binary_sensor")(sens))
