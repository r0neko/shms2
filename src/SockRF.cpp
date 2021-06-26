#include "SockRF.h"

SockRF::SockRF()
{
    //ctor
}

void RFProtocol::write_data(unsigned char* data, size_t d_len) {
    cout << "SockRF TX >> ";
    if(data != NULL)
    {
        for(unsigned int i = 0; i < d_len; i++) {
            printf("%02x ", data[i]);
        }
    }
    cout << '\n';
}

int RFProtocol::read_data(unsigned char* dst) {
    cout << "SockRF RX >> ";
    cin >> dst;
    for(unsigned int i = 0; i < sizeof(dst); i++) printf("%02x ", dst[i]);
    cout << '\n';
    return sizeof(dst);
}
