# Adafruit TCS34725 Color Sensor Driver in C for Raspberry Pi
_Modified from Adafruit_

This driver is for the [Adafruit TCS34725 Breakout](http://www.adafruit.com/products/1334).


## Compilation

### gcc flags

`-lm` is linking to lib math

```
scp * lighthouse:/root/dev/huesy ; echo "**********************`n* connecting via ssh *`n* running gcc        *`n**********************"; ssh lighthouse "cd ~/dev/huesy; gcc -g -o hues test.cpp WireShim.cpp Adafruit_TCS34725.cpp -fdiagnostics-color=always -lwiringPi -lm"
```



## About this Driver

These modules use I2C to communicate, 2 pins are required to  
interface
**Adafruit invests time and resources providing this open source code, please support Adafruit and open-source hardware by purchasing products from Adafruit!**

Written by Kevin (KTOWN) Townsend for Adafruit Industries.
BSD license, check license.txt for more information

All text above must be included in any redistribution

To download. click the ZIP button in the top bar, and check this tutorial
for instructions on how to install: 
http://learn.adafruit.com/adafruit-all-about-arduino-libraries-install-use
