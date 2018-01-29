#include "WireShim.h"
#include <stdio.h>
#include "config.h"

#if LOG_LEVEL
//#pragma message("LOG LEVEL NOT SET, setting it to 0")
#pragma message("LOG_LEVEL")
#endif
#if LOG_LEVEL == 2
//#pragma message("LOG LEVEL NOT SET, setting it to 0")
#pragma message("LOG_LEVEL is 2")
#endif


void TwoWire::begin(int devId) {
	setfd(devId);
}
void TwoWire::begin(uint8_t devId) {
	setfd(devId);
}
void TwoWire::begin(int sda, int scl) {
	printf("\n\tLOG:\tbegin() called sda=%i and scl=%i ; unsupported in WireShim\n",sda,scl);
}
void TwoWire::begin(){
	printf("\n\tLOG:\tbegin() called with no arguments\n");
}

void TwoWire::beginTransmission(int devId) {
	setfd(devId);
}
void TwoWire::beginTransmission(uint8_t devId) {
	setfd(devId);
}


void TwoWire::setfd(int devId){
	if (!fd){
		fd = wiringPiI2CSetup(devId);
		devId = devId;
#if LOG_LEVEL > 0
		printf("\tLOG:\tdevId 0x%X -> fd set %i\n", devId, fd);
#endif
	} else {
#if LOG_LEVEL > 2
		printf("\tLOG:\tdevId 0x%X & fd already set to %i\n", devId, fd);
#endif
	}
}



int TwoWire::read() {
	return wiringPiI2CRead(fd);
}

int TwoWire::receive() {
	return wiringPiI2CRead(fd);
}
uint16_t TwoWire::readReg16(int reg){
	return wiringPiI2CReadReg16(fd, reg);
}


int TwoWire::write(int data) {
	return wiringPiI2CWrite(fd, data);
}
int TwoWire::send(int data) {
	return wiringPiI2CWrite(fd,data);
}


int TwoWire::writeReg8(int reg, int data){
	return wiringPiI2CWriteReg8(fd, reg, data);
}


/* These do nothing, wiringpi has no equivalent */
uint8_t TwoWire::endTransmission(uint8_t devId){
	return 0;
}
/* These do nothing, wiringpi has no equivalent */
uint8_t TwoWire::endTransmission(void){
	return 0;
}

size_t TwoWire::requestFrom(uint8_t address, size_t size, bool sendStop){
	return 0;
}
uint8_t TwoWire::requestFrom(int address, int some_number) {
	return 0;
}
uint8_t TwoWire::requestFrom(uint8_t address, uint8_t some_number) {
	return 0;
}
uint8_t TwoWire::requestFrom(uint8_t address, uint8_t some_number, uint8_t another_number) {
	return 0;
}
uint8_t TwoWire::requestFrom(int address, int some_number, int another_number) {
	return 0;
}

uint8_t TwoWire::status(){
	return 0;
}


#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_TWOWIRE)
TwoWire Wire;
#endif
