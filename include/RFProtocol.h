#ifndef RFPROTOCOL_H
#define RFPROTOCOL_H

#include <iostream>

using namespace std;

#define DISCOVERY_MODE_ENABLE 1
#define DISCOVERY_MODE_DISABLE 0

class RFProtocol
{
    public:
        RFProtocol();
        int resolve_challenge(unsigned char* challenge, unsigned char* seed);
        void write_data(unsigned char* data, size_t d_len);
        int read_data(unsigned char* dst);
        int has_message() { return 0; }
    protected:
    private:
};

#endif // RFPROTOCOL_H
