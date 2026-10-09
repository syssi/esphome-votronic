import logging

import esphome.codegen as cg
from esphome.components import uart
import esphome.config_validation as cv
from esphome.const import CONF_ID, CONF_THROTTLE

_LOGGER = logging.getLogger(__name__)

CODEOWNERS = ["@syssi"]
DOMAIN = "votronic"

DEPENDENCIES = ["uart"]
AUTO_LOAD = ["binary_sensor", "sensor", "text_sensor"]
MULTI_CONF = True


def deprecated_renames(renames: dict[str, str]):
    def validator(config):
        config = config.copy()
        for old, new in renames.items():
            if old in config:
                _LOGGER.warning(
                    "'%s' is deprecated, use '%s' instead. Will be removed in a future release.",
                    old,
                    new,
                )
                config[new] = config.pop(old)
        return config

    return validator


CONF_VOTRONIC_ID = "votronic_id"
CONF_RX_TIMEOUT = "rx_timeout"

votronic_ns = cg.esphome_ns.namespace("votronic")
Votronic = votronic_ns.class_("Votronic", cg.PollingComponent, uart.UARTDevice)

VOTRONIC_COMPONENT_SCHEMA = cv.Schema(
    {
        cv.GenerateID(CONF_VOTRONIC_ID): cv.use_id(Votronic),
    }
)

CONFIG_SCHEMA = cv.All(
    cv.require_esphome_version(2024, 6, 0),
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(Votronic),
            cv.Optional(
                CONF_THROTTLE, default="2s"
            ): cv.positive_time_period_milliseconds,
            cv.Optional(
                CONF_RX_TIMEOUT, default="150ms"
            ): cv.positive_time_period_milliseconds,
        }
    )
    .extend(cv.polling_component_schema("never"))
    .extend(uart.UART_DEVICE_SCHEMA),
)

FINAL_VALIDATE_SCHEMA = uart.final_validate_device_schema(
    DOMAIN,
    baud_rate=1000,
    data_bits=8,
    parity="NONE",
    stop_bits=1,
    require_rx=True,
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await uart.register_uart_device(var, config)

    cg.add(var.set_throttle(config[CONF_THROTTLE]))
    cg.add(var.set_rx_timeout(config[CONF_RX_TIMEOUT]))
