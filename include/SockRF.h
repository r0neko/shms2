#ifndef SOCKRF_H
#define SOCKRF_H

#include "../debug_logger.h"
#include <cstring>
#include <cstdio>
#include <iostream>

#include "RFProtocol.h"

class SockRF: public RFProtocol
{
    public:
        SockRF();
        void write_data(unsigned char* data, size_t d_len);
        int read_data(unsigned char* dst);
    protected:
    private:
};

#endif // SOCKRF_H
