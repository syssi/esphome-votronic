import esphome.codegen as cg
from esphome.components import binary_sensor
import esphome.config_validation as cv
from esphome.const import CONF_ID

from . import CONF_VOTRONIC_ID, Votronic

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

BINARY_SENSORS = [
    CONF_CHARGING,
    CONF_DISCHARGING,
    CONF_CHARGER_CHARGING,
    CONF_CHARGER_DISCHARGING,
    CONF_CHARGER_CONTROLLER_ACTIVE,
    CONF_CHARGER_CURRENT_REDUCTION,
    CONF_CHARGER_AES_ACTIVE,
    CONF_CHARGING_CONVERTER_CHARGING,
    CONF_CHARGING_CONVERTER_DISCHARGING,
    CONF_CHARGING_CONVERTER_CONTROLLER_ACTIVE,
    CONF_CHARGING_CONVERTER_CURRENT_REDUCTION,
    CONF_CHARGING_CONVERTER_AES_ACTIVE,
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
        cv.Optional(CONF_CHARGER_CHARGING): binary_sensor.binary_sensor_schema(
            icon="mdi:battery-charging",
        ),
        cv.Optional(CONF_CHARGER_DISCHARGING): binary_sensor.binary_sensor_schema(
            icon="mdi:power-plug",
        ),
        cv.Optional(CONF_CHARGER_CONTROLLER_ACTIVE): binary_sensor.binary_sensor_schema(
            icon="mdi:power",
        ),
        cv.Optional(CONF_CHARGER_CURRENT_REDUCTION): binary_sensor.binary_sensor_schema(
            icon="mdi:car-speed-limiter",
        ),
        cv.Optional(CONF_CHARGER_AES_ACTIVE): binary_sensor.binary_sensor_schema(
            icon="mdi:export",
        ),
        cv.Optional(
            CONF_CHARGING_CONVERTER_CHARGING
        ): binary_sensor.binary_sensor_schema(
            icon="mdi:battery-charging",
        ),
        cv.Optional(
            CONF_CHARGING_CONVERTER_DISCHARGING
        ): binary_sensor.binary_sensor_schema(
            icon="mdi:power-plug",
        ),
        cv.Optional(
            CONF_CHARGING_CONVERTER_CONTROLLER_ACTIVE
        ): binary_sensor.binary_sensor_schema(
            icon="mdi:power",
        ),
        cv.Optional(
            CONF_CHARGING_CONVERTER_CURRENT_REDUCTION
        ): binary_sensor.binary_sensor_schema(
            icon="mdi:car-speed-limiter",
        ),
        cv.Optional(
            CONF_CHARGING_CONVERTER_AES_ACTIVE
        ): binary_sensor.binary_sensor_schema(
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
