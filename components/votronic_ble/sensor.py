import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import sensor
from esphome.const import (
    CONF_BATTERY_VOLTAGE,
    CONF_CURRENT,
    DEVICE_CLASS_CURRENT,
    DEVICE_CLASS_EMPTY,
    DEVICE_CLASS_ENERGY,
    DEVICE_CLASS_POWER,
    DEVICE_CLASS_VOLTAGE,
    ICON_EMPTY,
    STATE_CLASS_MEASUREMENT,
    UNIT_AMPERE,
    UNIT_EMPTY,
    UNIT_PERCENT,
    UNIT_VOLT,
    UNIT_WATT,
    UNIT_WATT_HOURS,
)

from . import CONF_VOTRONIC_BLE_ID, VotronicBle

DEPENDENCIES = ["votronic_ble"]

CODEOWNERS = ["@syssi"]

CONF_TOTAL_VOLTAGE = "total_voltage"

# CONF_BATTERY_VOLTAGE = "battery_voltage"
CONF_SECONDARY_BATTERY_VOLTAGE = "secondary_battery_voltage"
CONF_BATTERY_CAPACITY = "battery_capacity"
CONF_STATE_OF_CHARGE = "state_of_charge"
# CONF_CURRENT = "current"
CONF_BATTERY_NOMINAL_CAPACITY = "battery_nominal_capacity"
CONF_PV_VOLTAGE = "pv_voltage"
CONF_PV_CURRENT = "pv_current"
CONF_BATTERY_STATUS_BITMASK = "battery_status_bitmask"
CONF_PV_CONTROLLER_STATUS_BITMASK = "pv_controller_status_bitmask"
CONF_CHARGED_CAPACITY = "charged_capacity"
CONF_CHARGED_ENERGY = "charged_energy"
CONF_PV_POWER = "pv_power"

ICON_CURRENT_DC = "mdi:current-dc"

UNIT_AMPERE_HOURS = "Ah"

ICON_BATTERY_CAPACITY = "mdi:battery-medium"
ICON_STATE_OF_CHARGE = "mdi:battery-50"
ICON_BATTERY_NOMINAL_CAPACITY = "mdi:battery-100"

ICON_BATTERY_STATUS_BITMASK = "mdi:alert-circle-outline"
ICON_PV_CONTROLLER_STATUS_BITMASK = "mdi:alert-circle-outline"
ICON_CHARGED_CAPACITY = "mdi:battery-charging"

ICON_CHARGING_CYCLES = "mdi:battery-sync"
ICON_ERRORS_BITMASK = "mdi:alert-circle-outline"

SENSORS = [
    CONF_BATTERY_VOLTAGE,
    CONF_SECONDARY_BATTERY_VOLTAGE,
    CONF_BATTERY_CAPACITY,
    CONF_STATE_OF_CHARGE,
    CONF_CURRENT,
    CONF_BATTERY_NOMINAL_CAPACITY,
    CONF_PV_VOLTAGE,
    CONF_PV_CURRENT,
    CONF_BATTERY_STATUS_BITMASK,
    CONF_PV_CONTROLLER_STATUS_BITMASK,
    CONF_CHARGED_CAPACITY,
    CONF_CHARGED_ENERGY,
    CONF_PV_POWER,
]

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(CONF_VOTRONIC_BLE_ID): cv.use_id(VotronicBle),
        cv.Optional(CONF_BATTERY_VOLTAGE): sensor.sensor_schema(
            unit_of_measurement=UNIT_VOLT,
            icon=ICON_EMPTY,
            accuracy_decimals=2,
            device_class=DEVICE_CLASS_VOLTAGE,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_SECONDARY_BATTERY_VOLTAGE): sensor.sensor_schema(
            unit_of_measurement=UNIT_VOLT,
            icon=ICON_EMPTY,
            accuracy_decimals=2,
            device_class=DEVICE_CLASS_VOLTAGE,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_BATTERY_CAPACITY): sensor.sensor_schema(
            unit_of_measurement=UNIT_AMPERE_HOURS,
            icon=ICON_BATTERY_CAPACITY,
            accuracy_decimals=0,
            device_class=DEVICE_CLASS_EMPTY,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_STATE_OF_CHARGE): sensor.sensor_schema(
            unit_of_measurement=UNIT_PERCENT,
            icon=ICON_STATE_OF_CHARGE,
            accuracy_decimals=0,
            device_class=DEVICE_CLASS_EMPTY,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_CURRENT): sensor.sensor_schema(
            unit_of_measurement=UNIT_AMPERE,
            icon=ICON_CURRENT_DC,
            accuracy_decimals=3,
            device_class=DEVICE_CLASS_CURRENT,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_BATTERY_NOMINAL_CAPACITY): sensor.sensor_schema(
            unit_of_measurement=UNIT_AMPERE_HOURS,
            icon=ICON_BATTERY_NOMINAL_CAPACITY,
            accuracy_decimals=1,
            device_class=DEVICE_CLASS_EMPTY,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_PV_VOLTAGE): sensor.sensor_schema(
            unit_of_measurement=UNIT_VOLT,
            icon=ICON_EMPTY,
            accuracy_decimals=2,
            device_class=DEVICE_CLASS_VOLTAGE,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_PV_CURRENT): sensor.sensor_schema(
            unit_of_measurement=UNIT_AMPERE,
            icon=ICON_CURRENT_DC,
            accuracy_decimals=1,
            device_class=DEVICE_CLASS_CURRENT,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_BATTERY_STATUS_BITMASK): sensor.sensor_schema(
            unit_of_measurement=UNIT_EMPTY,
            icon=ICON_BATTERY_STATUS_BITMASK,
            accuracy_decimals=0,
            device_class=DEVICE_CLASS_EMPTY,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_PV_CONTROLLER_STATUS_BITMASK): sensor.sensor_schema(
            unit_of_measurement=UNIT_EMPTY,
            icon=ICON_PV_CONTROLLER_STATUS_BITMASK,
            accuracy_decimals=0,
            device_class=DEVICE_CLASS_EMPTY,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_CHARGED_CAPACITY): sensor.sensor_schema(
            unit_of_measurement=UNIT_AMPERE_HOURS,
            icon=ICON_CHARGED_CAPACITY,
            accuracy_decimals=0,
            device_class=DEVICE_CLASS_EMPTY,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_CHARGED_ENERGY): sensor.sensor_schema(
            unit_of_measurement=UNIT_WATT_HOURS,
            icon=ICON_CHARGED_CAPACITY,
            accuracy_decimals=0,
            device_class=DEVICE_CLASS_ENERGY,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_PV_POWER): sensor.sensor_schema(
            unit_of_measurement=UNIT_WATT,
            icon=ICON_EMPTY,
            accuracy_decimals=1,
            device_class=DEVICE_CLASS_POWER,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
    }
)


async def to_code(config):
    hub = await cg.get_variable(config[CONF_VOTRONIC_BLE_ID])
    for key in SENSORS:
        if key in config:
            conf = config[key]
            sens = await sensor.new_sensor(conf)
            cg.add(getattr(hub, f"set_{key}_sensor")(sens))
