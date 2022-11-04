# smart_silvia

This project is designed to be the brains of a Rancilio Silvia Espresso Machine. Leverging a microcontroller as the brains, this provides a display, weight, lower water level, and control over the pump and pressure valve.

# Gettings Started

Follow the steps to setup your esp tool chain
https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started/linux-macos-setup.html

# Building and Loading
Load your env:
'''
 . $HOME/esp/esp-idf/export.sh
 idf.py build


 idf.py -p /dev/cu.usbserial-0001 monitor
'''

# Git References:
OLED ESP-IDF-ssd1306:
https://github.com/nopnop2002/esp-idf-ssd1306



# Parts List
ESP-32:
https://www.amazon.com/dp/B09C5K91H7

OLED:
https://www.amazon.com/dp/B09C5K91H7

Solid State Relay:
https://www.amazon.com/dp/B01N0L5WSU

Load Sensor & ADC:
https://www.amazon.com/dp/B078KS1NBB

Rotery Encoder:
https://www.amazon.com/dp/B00NL7PVGM

Water Level Sensor:
https://www.amazon.com/gp/B08JPNY5KJ

Assorted Wires and Connectors:

https://www.amazon.com/dp/B0746HMTPP

https://www.amazon.com/dp/B07WWWPR2X

https://www.amazon.com/dp/B08RS737TF
