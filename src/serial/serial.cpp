#include "serial/serial.h"
#include <iostream>

gb::serial::serial()
{
    std::cout << "Serial Port Initialised..." << "\n";
}

void gb::serial::StartSerialTransfer()
{
    serialActive = true;
    cycles_remaining = 8;
}

void gb::serial::UpdateSerial(uint8_t cycles)
{
    cycles_remaining -= cycles;

    if(cycles_remaining <= 0)
    {
        serialActive = false;
        cycles_remaining = 0;

        requestInterrupt = true;
    }
}