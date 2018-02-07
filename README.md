# project notes


## Compilation

### gcc flags

`-lm` is linking to lib math


### Compiling on Windows

```
scp * lighthouse:/root/dev/huesy ; echo "**********************`n* connecting via ssh *`n* running gcc        *`n**********************"; ssh lighthouse "cd ~/dev/huesy; gcc -g -o hues test.cpp WireShim.cpp Adafruit_TCS34725.cpp -fdiagnostics-color=always -lwiringPi -lm"
```



```
cl /EHsc F:\Users\ehiller\dev\src\github.com\erichiller\hues\_test_serial.cpp F:\Users\ehiller\dev\src\github.com\erichiller\hues\SerialPort.cpp


cl /FC /EHsc F:\Users\ehiller\dev\src\github.com\erichiller\hues\_hues.cpp F:\Users\ehiller\dev\src\github.com\erichiller\hues\SerialPort.cpp /link /LIBPATH:F:\Users\ehiller\dev\src\github.com\erichiller\hues\ /DYNAMICBASE "kernel32.lib" "user32.lib" "gdi32.lib" "winspool.lib" "comdlg32.lib" "advapi32.lib" "shell32.lib" "ole32.lib" "oleaut32.lib" "uuid.lib" "odbc32.lib" "odbccp32.lib" "library\mbedTLS.lib" "msvcrt.lib" /NXCOMPAT /OUT:gohue.exe

cl /FC /EHsc F:\Users\ehiller\dev\src\github.com\erichiller\hues\_hues.cpp F:\Users\ehiller\dev\src\github.com\erichiller\hues\SerialPort.cpp /link /LIBPATH:F:\Users\ehiller\dev\src\github.com\erichiller\hues\ /DYNAMICBASE "kernel32.lib" "user32.lib" "gdi32.lib" "winspool.lib" "comdlg32.lib" "advapi32.lib" "shell32.lib" "library\mbedTLS.lib" "msvcrt.lib" /NXCOMPAT /OUT:gohue.exe /NODEFAULTLIB:library

```

this will fix many microsoft windows build errors:
<https://stackoverflow.com/questions/40230731/unresolved-externals-when-compiling-with-freetype>

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


# More TCS tests
**At night**
eb 84 14 7a 1e b8

**In a black bag**
ff ff 0 0 0 0


