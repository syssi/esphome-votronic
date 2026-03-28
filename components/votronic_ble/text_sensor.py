import esphome.codegen as cg
from esphome.components import text_sensor
import esphome.config_validation as cv
from esphome.const import ENTITY_CATEGORY_DIAGNOSTIC

from . import CONF_VOTRONIC_BLE_ID, VOTRONIC_BLE_SCHEMA

DEPENDENCIES = ["votronic_ble"]

CODEOWNERS = ["@syssi"]

CONF_BATTERY_STATUS = "battery_status"
CONF_PV_CONTROLLER_STATUS = "pv_controller_status"

ICON_BATTERY_STATUS = "mdi:alert-circle-outline"
ICON_PV_CONTROLLER_STATUS = "mdi:heart-pulse"

TEXT_SENSORS = [
    CONF_BATTERY_STATUS,
    CONF_PV_CONTROLLER_STATUS,
]

CONFIG_SCHEMA = VOTRONIC_BLE_SCHEMA.extend(
    {
        cv.Optional(CONF_BATTERY_STATUS): text_sensor.text_sensor_schema(
            icon=ICON_BATTERY_STATUS,
            entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
        ),
        cv.Optional(CONF_PV_CONTROLLER_STATUS): text_sensor.text_sensor_schema(
            icon=ICON_PV_CONTROLLER_STATUS,
            entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
        ),
    }
)


async def to_code(config):
    hub = await cg.get_variable(config[CONF_VOTRONIC_BLE_ID])
    for key in TEXT_SENSORS:
        if key in config:
            conf = config[key]
            sens = await text_sensor.new_text_sensor(conf)
            cg.add(getattr(hub, f"set_{key}_text_sensor")(sens))
