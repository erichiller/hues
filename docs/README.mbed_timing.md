


**only `mbedtls/esp_config.h` is ever read** never `mbedtls/config.h`


```
C:/Users/ehiller/AppData/local/omega/system/msys/opt/esp-idf/components/mbedtls/library/timing.c:43:2: error: #error "This module only works on Unix and Windows, see MBEDTLS_TIMING_C in config.h"
 #error "This module only works on Unix and Windows, see MBEDTLS_TIMING_C in config.h"
  ^
make[1]: *** [/c/Users/ehiller/AppData/local/omega/system/msys/opt/esp-idf/make/component_wrapper.mk:274: library/timing.o] Error 1
make: *** [/c/Users/ehiller/AppData/local/omega/system/msys/opt/esp-idf/make/project.mk:450: component-mbedtls-build] Error 2
```