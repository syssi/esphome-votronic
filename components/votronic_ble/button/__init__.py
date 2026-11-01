import esphome.codegen as cg
from esphome.components import button
import esphome.config_validation as cv
from esphome.const import CONF_ID, ENTITY_CATEGORY_CONFIG, ENTITY_CATEGORY_DIAGNOSTIC

from .. import CONF_VOTRONIC_BLE_ID, VOTRONIC_BLE_SCHEMA, votronic_ble_ns

DEPENDENCIES = ["votronic_ble"]

CODEOWNERS = ["@syssi"]

CONF_RETRIEVE_24H_LOG = "retrieve_24h_log"
CONF_RETRIEVE_DAILY_LOG = "retrieve_daily_log"
CONF_RESET_SOLAR_COUNTER = "reset_solar_counter"

ICON_RETRIEVE_LOG = "mdi:chart-box-outline"
ICON_RESET_SOLAR_COUNTER = "mdi:counter"

# Commands written to the management characteristic (AC12F485-CAB7-4E0A-AAC5-3585918852F6)
BUTTONS = {
    CONF_RETRIEVE_24H_LOG: 0x01,
    CONF_RETRIEVE_DAILY_LOG: 0x04,
    CONF_RESET_SOLAR_COUNTER: 0x06,
}

VotronicButton = votronic_ble_ns.class_("VotronicButton", button.Button, cg.Component)

CONFIG_SCHEMA = VOTRONIC_BLE_SCHEMA.extend(
    {
        cv.Optional(CONF_RETRIEVE_24H_LOG): button.button_schema(
            VotronicButton,
            icon=ICON_RETRIEVE_LOG,
            entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
        ).extend(cv.COMPONENT_SCHEMA),
        cv.Optional(CONF_RETRIEVE_DAILY_LOG): button.button_schema(
            VotronicButton,
            icon=ICON_RETRIEVE_LOG,
            entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
        ).extend(cv.COMPONENT_SCHEMA),
        cv.Optional(CONF_RESET_SOLAR_COUNTER): button.button_schema(
            VotronicButton,
            icon=ICON_RESET_SOLAR_COUNTER,
            entity_category=ENTITY_CATEGORY_CONFIG,
        ).extend(cv.COMPONENT_SCHEMA),
    }
)


async def to_code(config):
    hub = await cg.get_variable(config[CONF_VOTRONIC_BLE_ID])
    for key, command in BUTTONS.items():
        if key in config:
            conf = config[key]
            var = cg.new_Pvariable(conf[CONF_ID])
            await cg.register_component(var, conf)
            await button.register_button(var, conf)
            cg.add(var.set_parent(hub))
            cg.add(var.set_command(command))
