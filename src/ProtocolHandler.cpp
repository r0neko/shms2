#include "ProtocolHandler.h"

ProtocolHandler::ProtocolHandler(RFProtocol* rf)
{
    this->rf = rf;
}

int ProtocolHandler::register_device_discovery_callback(void* cb)
{
    if(!onDeviceDiscovered_callback_set) {
        this->onDeviceDiscovered = cb;
        this->onDeviceDiscovered_callback_set = 1;
    }
    return this->onDeviceDiscovered_callback_set;
}

int ProtocolHandler::unregister_device_discovery_callback()
{
    if(onDeviceDiscovered_callback_set) {
        this->onDeviceDiscovered = NULL;
        this->onDeviceDiscovered_callback_set = 0;
    }
    return this->onDeviceDiscovered_callback_set;
}

void ProtocolHandler::append_checksum(unsigned char* msg, size_t msg_sz)
{
    for(unsigned int i = 0; i < msg_sz; i++) msg[msg_sz - 1] += msg[i];
    msg[msg_sz - 1] %= 256;
}

unsigned char* ProtocolHandler::create_unencrypted_message(unsigned char* bytez, size_t size_)
{
    // to do
    unsigned char tmp_msg[7 + size_] = {PROTOCOL_HEADER, PROTOCOL_HEADER_UNENCRYPTED, 0x00, 0x00, 0x00, 0x00};
    int ix = 2;

    for(int i = 24; i > -8; i -= 8)
    {
            tmp_msg[ix] = ((size_ >> i) & 0xFF);
            ix++;
    }
    this->append_checksum(tmp_msg, sizeof(tmp_msg));
    return tmp_msg;
}

int ProtocolHandler::create_unencrypted_message_safe(unsigned char* ptr, unsigned char* bytez, size_t size_)
{
    ptr[0] = PROTOCOL_HEADER;
    ptr[1] = PROTOCOL_HEADER_UNENCRYPTED;

    ptr[2] = 0x00;
    ptr[3] = 0x00;
    ptr[4] = 0x00;
    ptr[5] = 0x00;

    for(int i = 6; i <= 5 + size_; i++) {
        ptr[i] = bytez[i-6];
    }

    int ix = 2;

    for(int i = 24; i > -8; i -= 8)
    {
            ptr[ix] = ((size_ >> i) & 0xFF);
            ix++;
    }

    this->append_checksum(ptr, 7 + size_);
}

void ProtocolHandler::device_discovery_thread()
{
    log("device discovery thread started");
    while(this->device_discovery) {
        if(rf->has_message()) break;
        unsigned char t_msg[11] = {PROTOCOL_DISCOVERY_QUEUE};
        this->create_unencrypted_message_safe(t_msg, t_msg, 1);
        rf->write_data(t_msg, sizeof(t_msg));
	    usleep(500 * 1000);
    }
    log("device discovery thread stopped");
}

int ProtocolHandler::start_device_discovery()
{
    this->device_discovery = 1;
    if(pthread_create(&this->discovery_thread,
                      NULL,
                      [](void* ptr) {
                        ((ProtocolHandler*)ptr)->device_discovery_thread();
                        return (void*)NULL;
                      },
                      (void*) this))
        return DEVICE_DISCOVERY_FAILURE;
    return DEVICE_DISCOVERY_SUCCESS;
}

int ProtocolHandler::stop_device_discovery()
{
    this->device_discovery = 0;
    pthread_join(this->discovery_thread, NULL);
    return DEVICE_DISCOVERY_SUCCESS;
}
