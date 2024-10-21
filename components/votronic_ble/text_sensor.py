import esphome.codegen as cg
from esphome.components import text_sensor
import esphome.config_validation as cv
from esphome.const import CONF_ICON, CONF_ID

from . import CONF_VOTRONIC_BLE_ID, VotronicBle

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

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(CONF_VOTRONIC_BLE_ID): cv.use_id(VotronicBle),
        cv.Optional(CONF_BATTERY_STATUS): text_sensor.TEXT_SENSOR_SCHEMA.extend(
            {
                cv.GenerateID(): cv.declare_id(text_sensor.TextSensor),
                cv.Optional(CONF_ICON, default=ICON_BATTERY_STATUS): cv.icon,
            }
        ),
        cv.Optional(CONF_PV_CONTROLLER_STATUS): text_sensor.TEXT_SENSOR_SCHEMA.extend(
            {
                cv.GenerateID(): cv.declare_id(text_sensor.TextSensor),
                cv.Optional(CONF_ICON, default=ICON_PV_CONTROLLER_STATUS): cv.icon,
            }
        ),
    }
)


async def to_code(config):
    hub = await cg.get_variable(config[CONF_VOTRONIC_BLE_ID])
    for key in TEXT_SENSORS:
        if key in config:
            conf = config[key]
            sens = cg.new_Pvariable(conf[CONF_ID])
            await text_sensor.register_text_sensor(sens, conf)
            cg.add(getattr(hub, f"set_{key}_text_sensor")(sens))
