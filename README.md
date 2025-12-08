# Snenseo 
# 🚨 WIP 🚨

This project is not finished... Yet! 
But stay tuned. I am working on it 😬

Snenseo is a zepyhr based firmware that implements a BLE device and is able to interact with a [Senseo coffe machine](https://www.senseo.us/about-senseo/the-senseo-coffee-pod-system/).

![Here should be a picture of a senseo machine](https://www.kaffeewiki.de/images/thumb/1/16/Senseo.jpg/300px-Senseo.jpg)

Modifications for the coffee machine are required and severe mistakes might also break the 40 € machine. 

## How is the machine automated?
Snenseo is  controlled by a Zephyr application that handles the coffee machines hardware (like pressing buttons etc.)
The Zepyhr applications also acts as BLE device. 

The name of the BLE device can be set with the Kconfig option `CONFIG_SNENSEO_BLE_SERVICES_DEV_NAME`

The UID for the Snenseo BLE service is `0x00003890, 0x82e2, 0x4279, 0x939f, 0x619a6607319c` 

