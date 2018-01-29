
#ifndef WireShim_h
#define WireShim_h

#include "wiringPi/wiringPi/wiringPi.h"
#include "wiringPi/wiringPi/wiringPiI2C.h"
#include <stdint.h>
#include <stdio.h>
#include "config.h"



/* see:
https://github.com/esp8266/Arduino/blob/master/libraries/Wire/Wire.h
*/
class TwoWire {
public:

	void beginTransmission(uint8_t devId);
    void beginTransmission(int devId);

    void begin(int sda, int scl);
    void begin();
    void begin(uint8_t);
    void begin(int);


	int read();
	int receive();
	uint16_t readReg16(int reg);

	int write(int data);
	int send(int data);

	int writeReg8(int reg, int data);


	/* non equivalent functions*/

    uint8_t endTransmission(void);
    uint8_t endTransmission(uint8_t devId);

	uint8_t status();

	size_t  requestFrom(uint8_t address, size_t size, bool sendStop);
	uint8_t requestFrom(int address, int some_number);
    uint8_t requestFrom(uint8_t, uint8_t);
    uint8_t requestFrom(uint8_t, uint8_t, uint8_t);
    uint8_t requestFrom(int, int, int);

private:
	void setfd(int fd);
	int fd;   // file descriptor // file handle //
	int devId; // i2c address //
};



#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_TWOWIRE)
extern TwoWire Wire;
#endif

#endif
