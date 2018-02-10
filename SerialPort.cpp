/*
* Author: Manash Kumar Mandal
* Modified Library introduced in Arduino Playground which does not work
* This works perfectly
* LICENSE: MIT
*/

#include "SerialPort.h"
#include "config.h"
#include "winshim.h"


// #define SERIAL_FILE_ATTRIBUTES FILE_ATTRIBUTE_NORMAL
#define SERIAL_FILE_ATTRIBUTES 0


SerialPort::SerialPort(char *portName)
{
    this->connected = false;

    this->handler = CreateFileA(static_cast<LPCSTR>(portName),
                                GENERIC_READ | GENERIC_WRITE,
                                0,
                                NULL,
                                OPEN_EXISTING,
                                SERIAL_FILE_ATTRIBUTES,
                                NULL);
    if (this->handler == INVALID_HANDLE_VALUE){
        if (GetLastError() == ERROR_FILE_NOT_FOUND) {
            printf("ERROR: Handle was not attached. Reason: %s not available\n", portName);
        } else {
            printf("ERROR!!!\n");
        }
    } else {
        DCB dcbSerialParameters = {0};

        if (!GetCommState(this->handler, &dcbSerialParameters)) {
            printf("failed to get current serial parameters\n");
        } else {
            // dcbSerialParameters.BaudRate = CBR_9600;
            dcbSerialParameters.BaudRate = CBR_38400;
            dcbSerialParameters.ByteSize = 8;
            dcbSerialParameters.StopBits = ONESTOPBIT;
            dcbSerialParameters.Parity = NOPARITY;
            dcbSerialParameters.fDtrControl = DTR_CONTROL_ENABLE;

            if (!SetCommState(handler, &dcbSerialParameters)) {
                printf("ALERT: could not set Serial port parameters\n");
            } else {
                this->connected = true;
                PurgeComm(this->handler, PURGE_RXCLEAR | PURGE_TXCLEAR);
#if ARDUINO_WAIT_TIME > 0
                usleep(ARDUINO_WAIT_TIME);
#endif
            }
        }
    }
}

SerialPort::~SerialPort()
{
    if (this->connected){
        this->connected = false;
        CloseHandle(this->handler);
    }
}

int SerialPort::readSerialPort(char *buffer, unsigned int buf_size)
{
    DWORD bytesRead;
    unsigned int toRead = 0;

    ClearCommError(this->handler, &this->errors, &this->status);

    if (this->status.cbInQue > 0){
        if (this->status.cbInQue > buf_size){
            toRead = buf_size;
        }
        else toRead = this->status.cbInQue;
    }

    if (ReadFile(this->handler, buffer, toRead, &bytesRead, NULL)) return bytesRead;

    return 0;
}

int SerialPort::writeSerialPort(char *buffer, unsigned int buf_size)
{
    DWORD bytesSent;

    if (!WriteFile(this->handler, (void*) buffer, buf_size, &bytesSent, 0)){
        ClearCommError(this->handler, &this->errors, &this->status);
        printf("ERROR while writing to SerialPort, potentially %i bytes sent", bytesSent);
        return 0;
    }
    else return bytesSent;
}

bool SerialPort::isConnected()
{
    return this->connected;
}