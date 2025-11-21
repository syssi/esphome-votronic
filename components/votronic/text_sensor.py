import esphome.codegen as cg
from esphome.components import text_sensor
import esphome.config_validation as cv
from esphome.const import CONF_ID

from . import CONF_VOTRONIC_ID, VOTRONIC_COMPONENT_SCHEMA

DEPENDENCIES = ["votronic"]

CODEOWNERS = ["@syssi"]

CONF_BATTERY_STATUS = "battery_status"

CONF_CHARGER_MODE_SETTING = "charger_mode_setting"
CONF_CHARGER_CONTROLLER_STATUS = "charger_controller_status"
CONF_CHARGER_BATTERY_STATUS = "charger_battery_status"

CONF_CHARGING_CONVERTER_MODE_SETTING = "charging_converter_mode_setting"
CONF_CHARGING_CONVERTER_BATTERY_STATUS = "charging_converter_battery_status"
CONF_CHARGING_CONVERTER_CONTROLLER_STATUS = "charging_converter_controller_status"

CONF_PV_MODE_SETTING = "pv_mode_setting"
CONF_PV_BATTERY_STATUS = "pv_battery_status"
CONF_PV_CONTROLLER_STATUS = "pv_controller_status"

TEXT_SENSORS = [
    CONF_BATTERY_STATUS,
    CONF_CHARGER_MODE_SETTING,
    CONF_CHARGER_CONTROLLER_STATUS,
    CONF_CHARGER_BATTERY_STATUS,
    CONF_CHARGING_CONVERTER_MODE_SETTING,
    CONF_CHARGING_CONVERTER_BATTERY_STATUS,
    CONF_CHARGING_CONVERTER_CONTROLLER_STATUS,
    CONF_PV_MODE_SETTING,
    CONF_PV_BATTERY_STATUS,
    CONF_PV_CONTROLLER_STATUS,
]

CONFIG_SCHEMA = VOTRONIC_COMPONENT_SCHEMA.extend(
    {
        cv.Optional(CONF_BATTERY_STATUS): text_sensor.text_sensor_schema(
            text_sensor.TextSensor, icon="mdi:alert-circle-outline"
        ),
        cv.Optional(CONF_CHARGER_MODE_SETTING): text_sensor.text_sensor_schema(
            text_sensor.TextSensor, icon="mdi:car-battery"
        ),
        cv.Optional(CONF_CHARGER_CONTROLLER_STATUS): text_sensor.text_sensor_schema(
            text_sensor.TextSensor, icon="mdi:heart-pulse"
        ),
        cv.Optional(CONF_CHARGER_BATTERY_STATUS): text_sensor.text_sensor_schema(
            text_sensor.TextSensor, icon="mdi:alert-circle-outline"
        ),
        cv.Optional(
            CONF_CHARGING_CONVERTER_BATTERY_STATUS
        ): text_sensor.text_sensor_schema(
            text_sensor.TextSensor, icon="mdi:car-battery"
        ),
        cv.Optional(
            CONF_CHARGING_CONVERTER_CONTROLLER_STATUS
        ): text_sensor.text_sensor_schema(
            text_sensor.TextSensor, icon="mdi:heart-pulse"
        ),
        cv.Optional(
            CONF_CHARGING_CONVERTER_MODE_SETTING
        ): text_sensor.text_sensor_schema(
            text_sensor.TextSensor, icon="mdi:car-battery"
        ),
        cv.Optional(CONF_PV_MODE_SETTING): text_sensor.text_sensor_schema(
            text_sensor.TextSensor, icon="mdi:car-battery"
        ),
        cv.Optional(CONF_PV_BATTERY_STATUS): text_sensor.text_sensor_schema(
            text_sensor.TextSensor, icon="mdi:alert-circle-outline"
        ),
        cv.Optional(CONF_PV_CONTROLLER_STATUS): text_sensor.text_sensor_schema(
            text_sensor.TextSensor, icon="mdi:heart-pulse"
        ),
        cv.Optional(
            CONF_CHARGING_CONVERTER_MODE_SETTING
        ): text_sensor.text_sensor_schema(
            text_sensor.TextSensor, icon="mdi:car-battery"
        ),
        cv.Optional(
            CONF_CHARGING_CONVERTER_BATTERY_STATUS
        ): text_sensor.text_sensor_schema(
            text_sensor.TextSensor, icon="mdi:car-battery"
        ),
        cv.Optional(
            CONF_CHARGING_CONVERTER_CONTROLLER_STATUS
        ): text_sensor.text_sensor_schema(
            text_sensor.TextSensor, icon="mdi:heart-pulse"
        ),
    }
)


async def to_code(config):
    hub = await cg.get_variable(config[CONF_VOTRONIC_ID])
    for key in TEXT_SENSORS:
        if key in config:
            conf = config[key]
            sens = cg.new_Pvariable(conf[CONF_ID])
            await text_sensor.register_text_sensor(sens, conf)
            cg.add(getattr(hub, f"set_{key}_text_sensor")(sens))
