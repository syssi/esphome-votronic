# esphome-votronic

![GitHub actions](https://github.com/syssi/esphome-votronic/actions/workflows/ci.yaml/badge.svg)
![GitHub stars](https://img.shields.io/github/stars/syssi/esphome-votronic)
![GitHub forks](https://img.shields.io/github/forks/syssi/esphome-votronic)
![GitHub watchers](https://img.shields.io/github/watchers/syssi/esphome-votronic)
[!["Buy Me A Coffee"](https://img.shields.io/badge/buy%20me%20a%20coffee-donate-yellow.svg)](https://www.buymeacoffee.com/syssi)

ESPHome component to monitor votronic devices via BLE or Display Link

## Supported devices

* Bluetooth Connector S-BC
  * Solar Charger SR/MPP since 2014 (S/N 14Vxx.xxxxx)
  * Battery Computer S + Smart Shunt

## Untested devices

* Battery Charger VBCS-Triple
* VPC Jupiter + Smart Shunt

## Requirements

* [ESPHome 2022.12.0 or higher](https://github.com/esphome/esphome/releases).
* A cable with rj11 western connector
* Generic ESP32 or ESP8266 board

## Setup

* [BLE communication via the Bluetooth Controller](votronic_bluetooth.md)
* [Wired communication using the Display Link interface](votronic_display_link.md)

## References

* https://github.com/heeplr/votronic
* https://github.com/TheRealMoeder/vosodi
* https://github.com/crathje/VotronicSRDuoDig
* https://github.com/SirReal-surreal/SirReal-surreal/blob/main/2021_08_16_Votronic.json
* https://codeberg.org/scy/votonic
* https://cumulumbus.de/smart-camper-auslesen-der-batterie-und-solarinformationen-aus-dem-votronic-bluetooth-connector/
