#include "Device.h"
#include <stdio.h>

Device::Device(RFProtocol* _rf, unsigned char* _did)
{
    this->rf = _rf;
    this->did[0] = _did[0];
    this->did[1] = _did[1];
    this->did[2] = _did[2];
    this->did[3] = _did[3];
    this->did[4] = _did[4];
    this->did[5] = _did[5];
    this->did[6] = _did[6];
    this->did[7] = _did[7];
    this->name = "NO_NAME";
}

Device::Device(RFProtocol* _rf, unsigned char* _did, char* _d_name)
{
    this->rf = _rf;
    this->did[0] = _did[0];
    this->did[1] = _did[1];
    this->did[2] = _did[2];
    this->did[3] = _did[3];
    this->did[4] = _did[4];
    this->did[5] = _did[5];
    this->did[6] = _did[6];
    this->did[7] = _did[7];
    this->name = _d_name;
}
