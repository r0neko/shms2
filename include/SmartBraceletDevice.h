#ifndef SMARTBRACELETDEVICE_H
#define SMARTBRACELETDEVICE_H

#include "RFProtocol.h"
#include "Device.h"
#include "DeviceTypes.h"

class SmartBraceletDevice : public Device
{
    public:
        SmartBraceletDevice() = default; // must set a parameter -> device_type = DEVICE_RELAY
        SmartBraceletDevice(RFProtocol* _rf, unsigned char* _did) : Device(_rf, _did) {}
        SmartBraceletDevice(RFProtocol* _rf, unsigned char* _did, char* _d_name) : Device(_rf, _did, _d_name) {}

        device_t getDeviceType() { return DEVICE_SBRACELET; }	

        // getters

		int getPulse() { return this->pulse; }
		float getTemperature() { return this->temperature; }

		// setters

		void setPulse(int pulse) { this->pulse = pulse; }
		void setTemperature(float temperature) { this->temperature = temperature; }
    protected:
    private:
    	float temperature = 0.0f;
    	int pulse = 0;
};

#endif // SMARTBRACELETDEVICE_H
