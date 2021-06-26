#ifndef RELAYDEVICE_H
#define RELAYDEVICE_H

#include "RFProtocol.h"
#include "Device.h"
#include "DeviceTypes.h"

class RelayDevice : public Device
{
    public:
        RelayDevice() = default; // must set a parameter -> device_type = DEVICE_RELAY
        RelayDevice(RFProtocol* _rf, unsigned char* _did) : Device(_rf, _did) {}
        RelayDevice(RFProtocol* _rf, unsigned char* _did, char* _d_name) : Device(_rf, _did, _d_name) {}

        int getRelayCount();
        int getRelayStatus(int relay_num);
        void setRelayState(int relay_num, int state);
    protected:
    private:
};

#endif // RELAYDEVICE_H
