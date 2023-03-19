# UTS IF4051

Author: 13519116 - Jeane Mikha Erwansyah

## Prerequisites

- NodeJS
- NPM
- Ports 3000 and 3001
- mqtt that runs on port 1883

## How to run

1. Run backend ([How to run backend](./be/README.md))
2. Run frontend ([How to run frontend](./fe/README.md))

## Tech Stack and Libraries

- Frontend: Vite + Vue, TailwindCSS, WebSocket
- Backend: MQTT, WebSocket
- ESP32: [PubSubClient](https://github.com/knolleary/pubsubclient), WiFi, [LEDc](https://github.com/espressif/arduino-esp32/blob/master/cores/esp32/esp32-hal-ledc.c)
