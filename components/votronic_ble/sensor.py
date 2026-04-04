import esphome.codegen as cg
from esphome.components import sensor
import esphome.config_validation as cv
from esphome.const import (
    CONF_BATTERY_VOLTAGE,
    CONF_CURRENT,
    CONF_POWER,
    DEVICE_CLASS_CURRENT,
    DEVICE_CLASS_EMPTY,
    DEVICE_CLASS_ENERGY,
    DEVICE_CLASS_POWER,
    DEVICE_CLASS_VOLTAGE,
    ENTITY_CATEGORY_DIAGNOSTIC,
    ICON_EMPTY,
    STATE_CLASS_MEASUREMENT,
    STATE_CLASS_TOTAL_INCREASING,
    UNIT_AMPERE,
    UNIT_EMPTY,
    UNIT_PERCENT,
    UNIT_VOLT,
    UNIT_WATT,
    UNIT_WATT_HOURS,
)

from . import CONF_VOTRONIC_BLE_ID, VOTRONIC_BLE_SCHEMA

DEPENDENCIES = ["votronic_ble"]

CODEOWNERS = ["@syssi"]

# CONF_BATTERY_VOLTAGE = "battery_voltage"
CONF_SECONDARY_BATTERY_VOLTAGE = "secondary_battery_voltage"
CONF_BATTERY_CAPACITY_REMAINING = "battery_capacity_remaining"
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

ICON_BATTERY_CAPACITY_REMAINING = "mdi:battery-50"
ICON_STATE_OF_CHARGE = "mdi:battery-50"
ICON_CURRENT_DC = "mdi:current-dc"
ICON_BATTERY_NOMINAL_CAPACITY = "mdi:battery"

ICON_BATTERY_STATUS_BITMASK = "mdi:alert-circle-outline"
ICON_PV_CONTROLLER_STATUS_BITMASK = "mdi:alert-circle-outline"
ICON_CHARGED_CAPACITY = "mdi:battery-charging"

ICON_CHARGING_CYCLES = "mdi:battery-sync"
ICON_ERRORS_BITMASK = "mdi:alert-circle-outline"

UNIT_AMPERE_HOURS = "Ah"

SENSOR_DEFS = {
    CONF_BATTERY_VOLTAGE: {
        "unit_of_measurement": UNIT_VOLT,
        "icon": ICON_EMPTY,
        "accuracy_decimals": 2,
        "device_class": DEVICE_CLASS_VOLTAGE,
        "state_class": STATE_CLASS_MEASUREMENT,
    },
    CONF_SECONDARY_BATTERY_VOLTAGE: {
        "unit_of_measurement": UNIT_VOLT,
        "icon": ICON_EMPTY,
        "accuracy_decimals": 2,
        "device_class": DEVICE_CLASS_VOLTAGE,
        "state_class": STATE_CLASS_MEASUREMENT,
    },
    CONF_BATTERY_CAPACITY_REMAINING: {
        "unit_of_measurement": UNIT_AMPERE_HOURS,
        "icon": ICON_BATTERY_CAPACITY_REMAINING,
        "accuracy_decimals": 0,
        "device_class": DEVICE_CLASS_EMPTY,
        "state_class": STATE_CLASS_MEASUREMENT,
    },
    CONF_STATE_OF_CHARGE: {
        "unit_of_measurement": UNIT_PERCENT,
        "icon": ICON_STATE_OF_CHARGE,
        "accuracy_decimals": 0,
        "device_class": DEVICE_CLASS_EMPTY,
        "state_class": STATE_CLASS_MEASUREMENT,
    },
    CONF_CURRENT: {
        "unit_of_measurement": UNIT_AMPERE,
        "icon": ICON_CURRENT_DC,
        "accuracy_decimals": 3,
        "device_class": DEVICE_CLASS_CURRENT,
        "state_class": STATE_CLASS_MEASUREMENT,
    },
    CONF_POWER: {
        "unit_of_measurement": UNIT_WATT,
        "icon": ICON_EMPTY,
        "accuracy_decimals": 2,
        "device_class": DEVICE_CLASS_POWER,
        "state_class": STATE_CLASS_MEASUREMENT,
    },
    CONF_BATTERY_NOMINAL_CAPACITY: {
        "unit_of_measurement": UNIT_AMPERE_HOURS,
        "icon": ICON_BATTERY_NOMINAL_CAPACITY,
        "accuracy_decimals": 1,
        "device_class": DEVICE_CLASS_EMPTY,
        "state_class": STATE_CLASS_MEASUREMENT,
    },
    CONF_PV_VOLTAGE: {
        "unit_of_measurement": UNIT_VOLT,
        "icon": ICON_EMPTY,
        "accuracy_decimals": 2,
        "device_class": DEVICE_CLASS_VOLTAGE,
        "state_class": STATE_CLASS_MEASUREMENT,
    },
    CONF_PV_CURRENT: {
        "unit_of_measurement": UNIT_AMPERE,
        "icon": ICON_CURRENT_DC,
        "accuracy_decimals": 1,
        "device_class": DEVICE_CLASS_CURRENT,
        "state_class": STATE_CLASS_MEASUREMENT,
    },
    CONF_PV_POWER: {
        "unit_of_measurement": UNIT_WATT,
        "icon": ICON_EMPTY,
        "accuracy_decimals": 1,
        "device_class": DEVICE_CLASS_POWER,
        "state_class": STATE_CLASS_MEASUREMENT,
    },
    CONF_BATTERY_STATUS_BITMASK: {
        "unit_of_measurement": UNIT_EMPTY,
        "icon": ICON_BATTERY_STATUS_BITMASK,
        "accuracy_decimals": 0,
        "device_class": DEVICE_CLASS_EMPTY,
        "entity_category": ENTITY_CATEGORY_DIAGNOSTIC,
    },
    CONF_PV_CONTROLLER_STATUS_BITMASK: {
        "unit_of_measurement": UNIT_EMPTY,
        "icon": ICON_PV_CONTROLLER_STATUS_BITMASK,
        "accuracy_decimals": 0,
        "device_class": DEVICE_CLASS_EMPTY,
        "entity_category": ENTITY_CATEGORY_DIAGNOSTIC,
    },
    CONF_CHARGED_CAPACITY: {
        "unit_of_measurement": UNIT_AMPERE_HOURS,
        "icon": ICON_CHARGED_CAPACITY,
        "accuracy_decimals": 0,
        "device_class": DEVICE_CLASS_EMPTY,
        "state_class": STATE_CLASS_TOTAL_INCREASING,
    },
    CONF_CHARGED_ENERGY: {
        "unit_of_measurement": UNIT_WATT_HOURS,
        "icon": ICON_CHARGED_CAPACITY,
        "accuracy_decimals": 0,
        "device_class": DEVICE_CLASS_ENERGY,
        "state_class": STATE_CLASS_TOTAL_INCREASING,
    },
}

CONFIG_SCHEMA = VOTRONIC_BLE_SCHEMA.extend(
    {
        cv.Optional(key): sensor.sensor_schema(**kwargs)
        for key, kwargs in SENSOR_DEFS.items()
    }
)


async def to_code(config):
    hub = await cg.get_variable(config[CONF_VOTRONIC_BLE_ID])
    for key in SENSOR_DEFS:
        if key in config:
            conf = config[key]
            sens = await sensor.new_sensor(conf)
            cg.add(getattr(hub, f"set_{key}_sensor")(sens))
