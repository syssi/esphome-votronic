"""Schema structure tests for votronic and votronic_ble ESPHome component modules."""

import os
import sys

sys.path.insert(0, os.path.join(os.path.dirname(__file__), ".."))

import components.votronic as hub  # noqa: E402
from components.votronic import binary_sensor, sensor, text_sensor  # noqa: E402
import components.votronic_ble as hub_ble  # noqa: E402
from components.votronic_ble import (  # noqa: E402
    binary_sensor as ble_binary_sensor,
    sensor as ble_sensor,
    text_sensor as ble_text_sensor,
)


class TestHubConstants:
    def test_votronic_conf_id_defined(self):
        assert hub.CONF_VOTRONIC_ID == "votronic_id"

    def test_votronic_ble_conf_id_defined(self):
        assert hub_ble.CONF_VOTRONIC_BLE_ID == "votronic_ble_id"


class TestVotronicSensorLists:
    def test_sensor_defs_completeness(self):
        assert "battery_voltage" in sensor.SENSOR_DEFS
        assert sensor.CONF_SECONDARY_BATTERY_VOLTAGE in sensor.SENSOR_DEFS
        assert sensor.CONF_STATE_OF_CHARGE in sensor.SENSOR_DEFS
        assert sensor.CONF_PV_VOLTAGE in sensor.SENSOR_DEFS
        assert sensor.CONF_PV_CURRENT in sensor.SENSOR_DEFS
        assert sensor.CONF_PV_POWER in sensor.SENSOR_DEFS
        assert sensor.CONF_CHARGER_CURRENT in sensor.SENSOR_DEFS
        assert len(sensor.SENSOR_DEFS) == 31

    def test_sensor_keys_are_strings(self):
        for key in sensor.SENSOR_DEFS:
            assert isinstance(key, str)


class TestVotronicBinarySensorConstants:
    def test_binary_sensor_defs_dict(self):
        assert binary_sensor.CONF_CHARGING in binary_sensor.BINARY_SENSOR_DEFS
        assert binary_sensor.CONF_DISCHARGING in binary_sensor.BINARY_SENSOR_DEFS
        assert binary_sensor.CONF_CHARGER_CHARGING in binary_sensor.BINARY_SENSOR_DEFS
        assert (
            binary_sensor.CONF_PV_CONTROLLER_ACTIVE in binary_sensor.BINARY_SENSOR_DEFS
        )
        assert len(binary_sensor.BINARY_SENSOR_DEFS) == 15


class TestVotronicTextSensorConstants:
    def test_text_sensors_list(self):
        assert text_sensor.CONF_BATTERY_STATUS in text_sensor.TEXT_SENSORS
        assert text_sensor.CONF_CHARGER_MODE_SETTING in text_sensor.TEXT_SENSORS
        assert text_sensor.CONF_PV_BATTERY_STATUS in text_sensor.TEXT_SENSORS
        assert len(text_sensor.TEXT_SENSORS) == 10


class TestVotronicBleSensorLists:
    def test_sensor_defs_completeness(self):
        assert "battery_voltage" in ble_sensor.SENSOR_DEFS
        assert ble_sensor.CONF_SECONDARY_BATTERY_VOLTAGE in ble_sensor.SENSOR_DEFS
        assert ble_sensor.CONF_STATE_OF_CHARGE in ble_sensor.SENSOR_DEFS
        assert ble_sensor.CONF_PV_VOLTAGE in ble_sensor.SENSOR_DEFS
        assert ble_sensor.CONF_CHARGED_CAPACITY in ble_sensor.SENSOR_DEFS
        assert ble_sensor.CONF_CHARGED_ENERGY in ble_sensor.SENSOR_DEFS
        assert len(ble_sensor.SENSOR_DEFS) == 14


class TestVotronicBleBinarySensorConstants:
    def test_binary_sensor_defs_dict(self):
        assert ble_binary_sensor.CONF_CHARGING in ble_binary_sensor.BINARY_SENSOR_DEFS
        assert (
            ble_binary_sensor.CONF_DISCHARGING in ble_binary_sensor.BINARY_SENSOR_DEFS
        )
        assert (
            ble_binary_sensor.CONF_CONTROLLER_ACTIVE
            in ble_binary_sensor.BINARY_SENSOR_DEFS
        )
        assert ble_binary_sensor.CONF_AES_ACTIVE in ble_binary_sensor.BINARY_SENSOR_DEFS
        assert len(ble_binary_sensor.BINARY_SENSOR_DEFS) == 5


class TestVotronicBleTextSensorConstants:
    def test_text_sensors_list(self):
        assert ble_text_sensor.CONF_BATTERY_STATUS in ble_text_sensor.TEXT_SENSORS
        assert ble_text_sensor.CONF_PV_CONTROLLER_STATUS in ble_text_sensor.TEXT_SENSORS
        assert len(ble_text_sensor.TEXT_SENSORS) == 2
