#include "RFProtocol.h"
#include "SockRF.h"
#include "../debug_logger.h"

RFProtocol::RFProtocol() {}

int RFProtocol::resolve_challenge(unsigned char* challenge, unsigned char* seed)
{
    for(unsigned int i = 0; i < sizeof(seed); i++) {
        for(unsigned int j = 0; j < sizeof(challenge); j++) {
            challenge[j] ^= seed[i] + j + i;
        }
    }
    return 0;
}
