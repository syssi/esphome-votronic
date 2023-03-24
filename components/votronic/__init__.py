import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import uart
from esphome.const import CONF_ID, CONF_THROTTLE

DEPENDENCIES = ["uart", "binary_sensor", "sensor", "text_sensor"]
CODEOWNERS = ["@syssi"]
MULTI_CONF = True

CONF_VOTRONIC_ID = "votronic_id"
CONF_RX_TIMEOUT = "rx_timeout"

votronic_ns = cg.esphome_ns.namespace("votronic")
Votronic = votronic_ns.class_("Votronic", cg.PollingComponent, uart.UARTDevice)

CONFIG_SCHEMA = (
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
    .extend(uart.UART_DEVICE_SCHEMA)
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await uart.register_uart_device(var, config)

    cg.add(var.set_throttle(config[CONF_THROTTLE]))
    cg.add(var.set_rx_timeout(config[CONF_RX_TIMEOUT]))
