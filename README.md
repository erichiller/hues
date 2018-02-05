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


## Sound to Hues

uses degrees

**example with `FREQUENCY_BINS=4`**
```
hues[0]=0.00
hues[1]=120.00
hues[2]=240.00
hues[3]=360.00
```


**example with `FREQUENCY_BINS=6`**
```
hues[0]=0.00
hues[1]=72.00
hues[2]=144.00
hues[3]=216.00
hues[4]=288.00
hues[5]=360.00
```

**example with `FREQUENCY_BINS=2`**
```
hues[0]=0.00
hues[1]=360.00
```


use command to trigger sampling setup
```
SET SAMPLE_RATE_HZ 9000;
```

FREQUENCY -> BIN
INTENSITY -> COLORATION

### Process for singular blended light

- read on HSL

1. find peak frequency
2. convert to color / over 360 degree spectrum
3. take "area under curve"


fabs(x) = *floating point* absolute value of `x`
fmod(x, y) = *floating point* remainder of x (numerator) / y (denominator)


chroma = intensity
h1 = hue / 60
x = chroma * (1.0 - absolute(h1 % 2.0) - 1.0 )

Using *Saturation*, we can add red or the other non-primary color(s) into the bin.


