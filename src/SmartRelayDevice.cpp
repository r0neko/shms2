#include "RelayDevice.h"
#include "../debug_logger.h"

int RelayDevice::getRelayCount()
{
    return 4;
}

int RelayDevice::getRelayStatus(int relay_num) {
    return 0;
}

void RelayDevice::setRelayState(int relay_num, int state) {
    if(relay_num <= this->getRelayCount()) {
        log("SetRelay: output " << relay_num << " state " << state);
    }
    else {
        log("SetRelay: output " << relay_num << " state " << state << " INEXISTENT RELAY");
    }
}
