"""Tests for the votronic deprecated-key compatibility layer (see components/votronic/__init__.py)."""

import logging
import os
import sys

sys.path.insert(0, os.path.join(os.path.dirname(__file__), ".."))

import pytest  # noqa: E402
import voluptuous as vol  # noqa: E402

from components.votronic import binary_sensor, sensor, text_sensor  # noqa: E402


def _validate(schema, config):
    return schema({"votronic_id": "votronic0", **config})


class TestSensorRenames:
    @pytest.mark.parametrize(
        ("old_key", "new_key"),
        [
            ("state_of_charge", sensor.CONF_BATTERY_COMPUTER_BATTERY_CHARGE),
            ("current", sensor.CONF_BATTERY_COMPUTER_CURRENT),
            ("power", sensor.CONF_BATTERY_COMPUTER_POWER),
            (
                "battery_capacity_remaining",
                sensor.CONF_BATTERY_COMPUTER_BATTERY_CAPACITY_REMAINING,
            ),
            (
                "battery_nominal_capacity",
                sensor.CONF_BATTERY_COMPUTER_BATTERY_NOMINAL_CAPACITY,
            ),
            (
                "battery_status_bitmask",
                sensor.CONF_BATTERY_COMPUTER_BATTERY_STATUS_BITMASK,
            ),
            (
                "charging_converter_controller_temperature",
                sensor.CONF_CHARGING_CONVERTER_BATTERY_TEMPERATURE,
            ),
        ],
    )
    def test_old_key_migrates_with_warning(self, caplog, old_key, new_key):
        with caplog.at_level(logging.WARNING):
            config = _validate(sensor.CONFIG_SCHEMA, {old_key: {"name": old_key}})
        assert new_key in config
        assert old_key not in config
        assert any(
            old_key in record.message and new_key in record.message
            for record in caplog.records
        )

    @pytest.mark.parametrize("key", ["battery_voltage", "secondary_battery_voltage"])
    def test_ambiguous_key_is_rejected(self, key):
        with pytest.raises(vol.Invalid, match=key):
            _validate(sensor.CONFIG_SCHEMA, {key: {"name": key}})

    def test_new_key_still_works(self):
        config = _validate(
            sensor.CONFIG_SCHEMA,
            {
                sensor.CONF_BATTERY_COMPUTER_BATTERY_CHARGE: {
                    "name": "new key still works"
                }
            },
        )
        assert sensor.CONF_BATTERY_COMPUTER_BATTERY_CHARGE in config


class TestBinarySensorRenames:
    @pytest.mark.parametrize(
        ("old_key", "new_key"),
        [
            ("charging", binary_sensor.CONF_BATTERY_COMPUTER_CHARGING),
            ("discharging", binary_sensor.CONF_BATTERY_COMPUTER_DISCHARGING),
        ],
    )
    def test_old_key_migrates_with_warning(self, caplog, old_key, new_key):
        with caplog.at_level(logging.WARNING):
            config = _validate(
                binary_sensor.CONFIG_SCHEMA, {old_key: {"name": old_key}}
            )
        assert new_key in config
        assert old_key not in config
        assert any(
            old_key in record.message and new_key in record.message
            for record in caplog.records
        )


class TestTextSensorRenames:
    def test_old_key_migrates_with_warning(self, caplog):
        with caplog.at_level(logging.WARNING):
            config = _validate(
                text_sensor.CONFIG_SCHEMA,
                {"battery_status": {"name": "battery_status"}},
            )
        assert text_sensor.CONF_BATTERY_COMPUTER_BATTERY_STATUS in config
        assert "battery_status" not in config
