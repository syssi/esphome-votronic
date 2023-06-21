import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import sensor
from esphome.const import (
    CONF_BATTERY_VOLTAGE,
    CONF_CURRENT,
    CONF_POWER,
    DEVICE_CLASS_CURRENT,
    DEVICE_CLASS_EMPTY,
    DEVICE_CLASS_POWER,
    DEVICE_CLASS_TEMPERATURE,
    DEVICE_CLASS_VOLTAGE,
    ENTITY_CATEGORY_DIAGNOSTIC,
    ICON_EMPTY,
    STATE_CLASS_MEASUREMENT,
    UNIT_AMPERE,
    UNIT_CELSIUS,
    UNIT_EMPTY,
    UNIT_PERCENT,
    UNIT_VOLT,
    UNIT_WATT,
)

from . import CONF_VOTRONIC_ID, Votronic

DEPENDENCIES = ["votronic"]

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
CONF_PV_BATTERY_STATUS_BITMASK = "pv_battery_status_bitmask"
CONF_CHARGING_CONTROLLER_STATUS_BITMASK = "charging_controller_status_bitmask"
CONF_PV_CONTROLLER_STATUS_BITMASK = "pv_controller_status_bitmask"
CONF_CHARGED_CAPACITY = "charged_capacity"
CONF_CHARGED_ENERGY = "charged_energy"
CONF_PV_POWER = "pv_power"
CONF_CHARGING_MODE_SETTING_ID = "charging_mode_setting_id"
CONF_CONTROLLER_TEMPERATURE = "controller_temperature"
CONF_PV_CONTROLLER_TEMPERATURE = "pv_controller_temperature"

ICON_BATTERY_CAPACITY_REMAINING = "mdi:battery-50"
ICON_STATE_OF_CHARGE = "mdi:battery-50"
ICON_CURRENT_DC = "mdi:current-dc"
ICON_BATTERY_NOMINAL_CAPACITY = "mdi:battery"

ICON_BATTERY_STATUS_BITMASK = "mdi:alert-circle-outline"
ICON_PV_BATTERY_STATUS_BITMASK = "mdi:alert-circle-outline"
ICON_CHARGING_CONTROLLER_STATUS_BITMASK = "mdi:alert-circle-outline"
ICON_PV_CONTROLLER_STATUS_BITMASK = "mdi:alert-circle-outline"
ICON_CHARGING_MODE_SETTING_ID = "mdi:car-battery"

UNIT_AMPERE_HOURS = "Ah"

SENSORS = [
    CONF_BATTERY_VOLTAGE,
    CONF_SECONDARY_BATTERY_VOLTAGE,
    CONF_BATTERY_CAPACITY_REMAINING,
    CONF_STATE_OF_CHARGE,
    CONF_CURRENT,
    CONF_POWER,
    CONF_BATTERY_NOMINAL_CAPACITY,
    CONF_PV_VOLTAGE,
    CONF_PV_CURRENT,
    CONF_PV_POWER,
    CONF_BATTERY_STATUS_BITMASK,
    CONF_PV_BATTERY_STATUS_BITMASK,
    CONF_CHARGING_CONTROLLER_STATUS_BITMASK,
    CONF_PV_CONTROLLER_STATUS_BITMASK,
    CONF_CHARGING_MODE_SETTING_ID,
    CONF_CONTROLLER_TEMPERATURE,
    CONF_PV_CONTROLLER_TEMPERATURE,
]

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(CONF_VOTRONIC_ID): cv.use_id(Votronic),
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
        cv.Optional(CONF_BATTERY_CAPACITY_REMAINING): sensor.sensor_schema(
            unit_of_measurement=UNIT_AMPERE_HOURS,
            icon=ICON_BATTERY_CAPACITY_REMAINING,
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
        cv.Optional(CONF_POWER): sensor.sensor_schema(
            unit_of_measurement=UNIT_WATT,
            icon=ICON_EMPTY,
            accuracy_decimals=2,
            device_class=DEVICE_CLASS_POWER,
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
        cv.Optional(CONF_PV_POWER): sensor.sensor_schema(
            unit_of_measurement=UNIT_WATT,
            icon=ICON_EMPTY,
            accuracy_decimals=1,
            device_class=DEVICE_CLASS_POWER,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_BATTERY_STATUS_BITMASK): sensor.sensor_schema(
            unit_of_measurement=UNIT_EMPTY,
            icon=ICON_BATTERY_STATUS_BITMASK,
            accuracy_decimals=0,
            device_class=DEVICE_CLASS_EMPTY,
            state_class=STATE_CLASS_MEASUREMENT,
            entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
        ),
        cv.Optional(CONF_PV_BATTERY_STATUS_BITMASK): sensor.sensor_schema(
            unit_of_measurement=UNIT_EMPTY,
            icon=ICON_PV_BATTERY_STATUS_BITMASK,
            accuracy_decimals=0,
            device_class=DEVICE_CLASS_EMPTY,
            state_class=STATE_CLASS_MEASUREMENT,
            entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
        ),
        cv.Optional(CONF_CHARGING_CONTROLLER_STATUS_BITMASK): sensor.sensor_schema(
            unit_of_measurement=UNIT_EMPTY,
            icon=ICON_CHARGING_CONTROLLER_STATUS_BITMASK,
            accuracy_decimals=0,
            device_class=DEVICE_CLASS_EMPTY,
            state_class=STATE_CLASS_MEASUREMENT,
            entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
        ),
        cv.Optional(CONF_PV_CONTROLLER_STATUS_BITMASK): sensor.sensor_schema(
            unit_of_measurement=UNIT_EMPTY,
            icon=ICON_PV_CONTROLLER_STATUS_BITMASK,
            accuracy_decimals=0,
            device_class=DEVICE_CLASS_EMPTY,
            state_class=STATE_CLASS_MEASUREMENT,
            entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
        ),
        cv.Optional(CONF_CHARGING_MODE_SETTING_ID): sensor.sensor_schema(
            unit_of_measurement=UNIT_EMPTY,
            icon=ICON_CHARGING_MODE_SETTING_ID,
            accuracy_decimals=0,
            device_class=DEVICE_CLASS_EMPTY,
            state_class=STATE_CLASS_MEASUREMENT,
            entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
        ),
        cv.Optional(CONF_CONTROLLER_TEMPERATURE): sensor.sensor_schema(
            unit_of_measurement=UNIT_CELSIUS,
            icon=ICON_EMPTY,
            accuracy_decimals=0,
            device_class=DEVICE_CLASS_TEMPERATURE,
            state_class=STATE_CLASS_MEASUREMENT,
            entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
        ),
        cv.Optional(CONF_PV_CONTROLLER_TEMPERATURE): sensor.sensor_schema(
            unit_of_measurement=UNIT_CELSIUS,
            icon=ICON_EMPTY,
            accuracy_decimals=0,
            device_class=DEVICE_CLASS_TEMPERATURE,
            state_class=STATE_CLASS_MEASUREMENT,
            entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
        ),
    }
)


async def to_code(config):
    hub = await cg.get_variable(config[CONF_VOTRONIC_ID])
    for key in SENSORS:
        if key in config:
            conf = config[key]
            sens = await sensor.new_sensor(conf)
            cg.add(getattr(hub, f"set_{key}_sensor")(sens))
