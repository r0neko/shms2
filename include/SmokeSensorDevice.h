#ifndef SMOKESENSORDEVICE_H
#define SMOKESENSORDEVICE_H

#include "RFProtocol.h"
#include "Device.h"
#include "DeviceTypes.h"

class SmokeSensorDevice : public Device
{
    public:
        SmokeSensorDevice() = default; // must set a parameter -> device_type = DEVICE_SMOKE
        SmokeSensorDevice(RFProtocol* _rf, unsigned char* _did) : Device(_rf, _did) {}
        SmokeSensorDevice(RFProtocol* _rf, unsigned char* _did, char* _d_name) : Device(_rf, _did, _d_name) {}

        device_t getDeviceType() { return DEVICE_SMOKE; }	       
    protected:
    private:
};

#endif // SMOKESENSORDEVICE_H
