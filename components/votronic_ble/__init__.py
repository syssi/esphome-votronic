import esphome.codegen as cg
from esphome.components import ble_client
import esphome.config_validation as cv
from esphome.const import CONF_ID, CONF_THROTTLE

AUTO_LOAD = ["binary_sensor", "sensor", "text_sensor"]
CODEOWNERS = ["@syssi"]
MULTI_CONF = True

CONF_VOTRONIC_BLE_ID = "votronic_ble_id"

votronic_ble_ns = cg.esphome_ns.namespace("votronic_ble")
VotronicBle = votronic_ble_ns.class_(
    "VotronicBle", ble_client.BLEClientNode, cg.PollingComponent
)

VOTRONIC_BLE_COMPONENT_SCHEMA = cv.Schema(
    {
        cv.GenerateID(CONF_VOTRONIC_BLE_ID): cv.use_id(VotronicBle),
    }
)

CONFIG_SCHEMA = (
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(VotronicBle),
            cv.Optional(
                CONF_THROTTLE, default="2s"
            ): cv.positive_time_period_milliseconds,
        }
    )
    .extend(ble_client.BLE_CLIENT_SCHEMA)
    .extend(cv.polling_component_schema("2s"))
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await ble_client.register_ble_node(var, config)

    cg.add(var.set_throttle(config[CONF_THROTTLE]))
