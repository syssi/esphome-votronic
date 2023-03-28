import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import button
from esphome.const import CONF_ICON, CONF_ID

from .. import CONF_VOTRONIC_BLE_ID, VOTRONIC_BLE_COMPONENT_SCHEMA, votronic_ble_ns

DEPENDENCIES = ["votronic_ble"]

CODEOWNERS = ["@syssi"]

CONF_RETRIEVE_SETTINGS = "retrieve_settings"
CONF_RETRIEVE_DEVICE_INFO = "retrieve_device_info"

CONF_RETRIEVE_DAILY_OPERATING_STATISTICS = "retrieve_daily_operating_statistics"
CONF_RETRIEVE_WEEKLY_OPERATING_STATISTICS = "retrieve_weekly_operating_statistics"
CONF_RETRIEVE_MONTHLY_OPERATING_STATISTICS = "retrieve_monthly_operating_statistics"
CONF_RETRIEVE_OPERATING_STATISTICS = "retrieve_operating_statistics"
CONF_RESET_ACCUMULATOR = "reset_accumulator"

ICON_RETRIEVE_DAILY_OPERATING_STATISTICS = "mdi:chart-box-outline"
ICON_RETRIEVE_WEEKLY_OPERATING_STATISTICS = "mdi:chart-box-outline"
ICON_RETRIEVE_MONTHLY_OPERATING_STATISTICS = "mdi:chart-box-outline"
ICON_RETRIEVE_OPERATING_STATISTICS = "mdi:chart-box-outline"
ICON_RESET_ACCUMULATOR = "mdi:history"

BUTTONS = {
    CONF_RETRIEVE_DAILY_OPERATING_STATISTICS: 0x01,
    CONF_RETRIEVE_WEEKLY_OPERATING_STATISTICS: 0x02,
    CONF_RETRIEVE_MONTHLY_OPERATING_STATISTICS: 0x03,
    CONF_RETRIEVE_OPERATING_STATISTICS: 0x04,
    CONF_RESET_ACCUMULATOR: 0x06,
}

VotronicButton = votronic_ble_ns.class_("VotronicButton", button.Button, cg.Component)

CONFIG_SCHEMA = VOTRONIC_BLE_COMPONENT_SCHEMA.extend(
    {
        cv.Optional(
            CONF_RETRIEVE_DAILY_OPERATING_STATISTICS
        ): button.BUTTON_SCHEMA.extend(
            {
                cv.GenerateID(): cv.declare_id(VotronicButton),
                cv.Optional(
                    CONF_ICON, default=ICON_RETRIEVE_DAILY_OPERATING_STATISTICS
                ): cv.icon,
            }
        ).extend(
            cv.COMPONENT_SCHEMA
        ),
        cv.Optional(
            CONF_RETRIEVE_WEEKLY_OPERATING_STATISTICS
        ): button.BUTTON_SCHEMA.extend(
            {
                cv.GenerateID(): cv.declare_id(VotronicButton),
                cv.Optional(
                    CONF_ICON, default=ICON_RETRIEVE_WEEKLY_OPERATING_STATISTICS
                ): cv.icon,
            }
        ).extend(
            cv.COMPONENT_SCHEMA
        ),
        cv.Optional(
            CONF_RETRIEVE_MONTHLY_OPERATING_STATISTICS
        ): button.BUTTON_SCHEMA.extend(
            {
                cv.GenerateID(): cv.declare_id(VotronicButton),
                cv.Optional(
                    CONF_ICON, default=ICON_RETRIEVE_MONTHLY_OPERATING_STATISTICS
                ): cv.icon,
            }
        ).extend(
            cv.COMPONENT_SCHEMA
        ),
        cv.Optional(CONF_RETRIEVE_OPERATING_STATISTICS): button.BUTTON_SCHEMA.extend(
            {
                cv.GenerateID(): cv.declare_id(VotronicButton),
                cv.Optional(
                    CONF_ICON, default=ICON_RETRIEVE_OPERATING_STATISTICS
                ): cv.icon,
            }
        ).extend(cv.COMPONENT_SCHEMA),
        cv.Optional(CONF_RESET_ACCUMULATOR): button.BUTTON_SCHEMA.extend(
            {
                cv.GenerateID(): cv.declare_id(VotronicButton),
                cv.Optional(CONF_ICON, default=ICON_RESET_ACCUMULATOR): cv.icon,
            }
        ).extend(cv.COMPONENT_SCHEMA),
    }
)


async def to_code(config):
    hub = await cg.get_variable(config[CONF_VOTRONIC_BLE_ID])
    for key, address in BUTTONS.items():
        if key in config:
            conf = config[key]
            var = cg.new_Pvariable(conf[CONF_ID])
            await cg.register_component(var, conf)
            await button.register_button(var, conf)
            cg.add(var.set_parent(hub))
            cg.add(var.set_holding_register(address))
