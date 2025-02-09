#ifndef GB_SERIAL
#define GB_SERIAL

#include <cstdint>

namespace gb
{
    class serial
    {
    public:
        serial();

        void StartSerialTransfer();
        void UpdateSerial(uint8_t cycles);
        
        uint8_t cycles_remaining = 0;
        bool serialActive = false;
        bool requestInterrupt = false;
    };
}

#endif