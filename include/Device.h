#ifndef DEVICE_H
#define DEVICE_H

#include "RFProtocol.h"
#include "DeviceTypes.h"

// import all devices here

class Device
{
    public:
        Device() {}
        Device(RFProtocol* _rf, unsigned char* _did);
        Device(RFProtocol* _rf, unsigned char* _did, char* _d_name);

        char* name;
        unsigned char did[8];

        virtual device_t getDeviceType() { return DEVICE_DUMMY; }   
    protected:

    private:
        RFProtocol* rf;
};

#endif // DEVICE_H
