#ifndef PROTOCOLHANDLER_H
#define PROTOCOLHANDLER_H

#include "RFProtocol.h"
#include <pthread.h>
#include <cstring>
#include <unistd.h>
#include <cstdio>
#include "../debug_logger.h"

#define DEVICE_DISCOVERY_FAILURE 0xFF
#define DEVICE_DISCOVERY_SUCCESS 0xEF

// protocol
#define PROTOCOL_HEADER 0x09
#define PROTOCOL_HEADER_UNENCRYPTED 0xFF

// protocol commands
#define PROTOCOL_DISCOVERY_QUEUE 0x01

class ProtocolHandler
{
    public:
        ProtocolHandler();
        ProtocolHandler(RFProtocol* rf);
        int start_device_discovery();
        int stop_device_discovery();
        int register_device_discovery_callback(void* cb);
        int unregister_device_discovery_callback();
        void append_checksum(unsigned char* msg, size_t msg_sz);
        unsigned char* create_unencrypted_message(unsigned char *bytez, size_t size_);
        int create_unencrypted_message_safe(unsigned char* ptr, unsigned char* bytez, size_t size_);
    protected:

    private:
        RFProtocol* rf;

        void* onDeviceDiscovered;
        int onDeviceDiscovered_callback_set;
        int device_discovery;

        void device_discovery_thread();

        pthread_t discovery_thread;
};

#endif // PROTOCOLHANDLER_H
