/**

S.H.M.S. Project

Protocol Codename: AlphaNet
Codename: NexusCore
Author: Talnaci Alexandru

Application: hub_daemon
Current Stage: Stable(Linux)
Final Target: Linux

Communication Protocol: v1.4r1-ENC(planned)
                        v0.4(in use)

Copyright: Talnaci Alexandru (c) 2019

**/

#include <vector>
#include <memory>

#include <iostream>
#include <stdio.h>
#include <cstdlib>
#include <pthread.h>

#include "HTTPClient.h"

#include "debug_logger.h"

#include "build_config.h"

#include "RFProtocol.h"
#include "ProtocolHandler.h"

#ifdef DEBUG_NO_RF
#include "SockRF.h"
#else
#include "nRF24L01.h"
#endif

#include "Device.h"
#include "SmartBraceletDevice.h"
#include "SmokeSensorDevice.h"
#include "HuaweiModem.h"


using namespace std;

#ifndef DEBUG_NO_RF
nRF24L01 *rf;
#endif

ProtocolHandler* protocol;
vector<Device*> device_list(MAXIMUM_DEVICES_ALLOWED);

// Modem

#ifdef CONFIG_USE_MODEM

HuaweiModem modem("/dev/ttyUSB2", "/dev/ttyUSB1");

#endif

// API requirements

#include <cJSON.h>

HTTPClient http(CONFIG_SERVER_IP, CONFIG_SERVER_PORT);

std::string API_SID;

vector<string> API_queue(1024);
pthread_t queue_thr;

// UTILS

std::string didToHexString(Device* device) {
    std::string res;

    char buf[16] = {0};

    for(int i = 0; i < 8; i++) {
        sprintf(buf, "%02X", device->did[i]);
        res.append(buf);
    }

    return res;
}

std::string typeToString(device_t d_type) {
    switch(d_type) {
        case DEVICE_DUMMY:
            return "DEVICE_DUMMY";
        case DEVICE_RELAY:
            return "DEVICE_RELAY";
        case DEVICE_SMOKE:
            return "DEVICE_SMOKE";
        case DEVICE_SBRACELET:
            return "DEVICE_SBRACELET";
    }
    return "DEVICE_DUMMY";
}

void* queue_thread(void* v) {
    while(1) {
        for(int i = 0; i < API_queue.size(); i++) {
            if(API_queue[i].size() > 1) {
		log("queue found request to " << API_queue[i]);
                http.get(API_queue[i]);
                API_queue.erase(API_queue.begin() + i);
                break;
            }
        }
    }
}

void push_to_queue(char* path) {
    API_queue.push_back(path);
}

/**

helper function:
void add_device(Device*)

**/

void add_device(Device* device) {
    for(int i = 0; i < MAXIMUM_DEVICES_ALLOWED; i++) {
        if(device_list[i] == NULL) {
            device_list[i] = device;
            return;
        }
    }
    return;
}

int compare(unsigned char* one, unsigned char* two) {
    int identical_bytes = 0;
    for(unsigned int i = 0; i < sizeof(one); i++)
        identical_bytes += (one[i] == two[i]);
    return identical_bytes;
}

Device* find_device(unsigned char* did) {
    for(int i = 0; i < MAXIMUM_DEVICES_ALLOWED; i++) {
        if(device_list[i] != NULL) {
            if(compare(did, device_list[i]->did) == sizeof(did)) return device_list[i];
        }
    }
    return NULL;
}

void loadDevices() {
    log("will load devices from forcedDB...");
    usleep(1000000);
    unsigned char did_smoke[8] = {0x99, 0xB7, 0x6C, 0x7A, 0xBE, 0x4D, 0x60, 0x64};
    unsigned char did_sbrac[8] = {0x81, 0x74, 0xAF, 0x72, 0xE9, 0x7A, 0x74, 0x8A};
    #ifndef DEBUG_NO_RF
    add_device(new SmokeSensorDevice(rf, did_smoke, "Smoke Detector DEV"));
    add_device(new SmartBraceletDevice(rf, did_sbrac, "Smart Bracelet DEV"));
    #endif
    log("devices loaded successfully!");
}

void APIRegister() {
    log("APIRegister startup");
    std::string response = http.get("/api/device/register/DEVELOPMENT"); // for now we use a hardcoded HID(Hardware ID) - DEVELOPMENT
    cJSON *json = cJSON_Parse(response.c_str());
    if (json == NULL) {
        cJSON_Delete(json);
        log("JSON Parser - ERROR!");
        exit(1);
    }

    const cJSON *status = cJSON_GetObjectItemCaseSensitive(json, "status");
    if(cJSON_IsNumber(status) && status->valuedouble == 0) {
        const cJSON *sid = cJSON_GetObjectItemCaseSensitive(json, "sid");
        if(cJSON_IsString(sid) && (sid->valuestring != NULL)) {
            API_SID = sid->valuestring;
            log("Retrieved a new API_SID from the server. API_SID = " << API_SID);
        }
    } else {
        const cJSON *message = cJSON_GetObjectItemCaseSensitive(json, "message");
        if(cJSON_IsString(message) && (message->valuestring != NULL)) {
            log("ERROR: API RESPONSE: " << message->valuestring);
            exit(0);
        }
    }
    cJSON_Delete(json);
}

void publishDeviceList() {
    cJSON *devices = cJSON_CreateArray();
    if(devices == NULL) return;

    cJSON *device = NULL;

    for(int i = 0; i < MAXIMUM_DEVICES_ALLOWED; i++) {
        if(device_list[i] != NULL) {
            device = cJSON_CreateObject();
            if(device == NULL) return;
            cJSON_AddItemToArray(devices, device);

            cJSON_AddItemToObject(device, "name", cJSON_CreateString(device_list[i]->name));
            cJSON_AddItemToObject(device, "did", cJSON_CreateString(didToHexString(device_list[i]).c_str()));
            cJSON_AddItemToObject(device, "type", cJSON_CreateString(typeToString(device_list[i]->getDeviceType()).c_str()));
        }
    }

    cJSON *core = cJSON_CreateObject();
    if(core == NULL) return;

    cJSON_AddItemToObject(core, "devices", devices);

    std::string json_str = cJSON_PrintUnformatted(core);
    cJSON_Delete(core);

    char sid_path[64] = {0};
    sprintf(sid_path, "/api/device/%s/publish", API_SID.c_str());

    std::string response = http.post(sid_path, json_str);

    cJSON *json = cJSON_Parse(response.c_str());
    if (json == NULL) {
        cJSON_Delete(json);
        log("JSON Parser - ERROR!");
        exit(1);
    }

    const cJSON *status = cJSON_GetObjectItemCaseSensitive(json, "status");
    if(cJSON_IsNumber(status) && status->valuedouble == 1) {
        const cJSON *message = cJSON_GetObjectItemCaseSensitive(json, "message");
        if(cJSON_IsString(message) && (message->valuestring != NULL)) {
            log("ERROR: API RESPONSE: " << message->valuestring);
            return;
        }
    } else {
        const cJSON *message = cJSON_GetObjectItemCaseSensitive(json, "message");
        if(cJSON_IsString(message) && (message->valuestring != NULL)) {
            log("API Response: " << message->valuestring);
        }
    }
    cJSON_Delete(json);
}

void trigger(Device* device) {
    log("trigger called!");
    char trigger_sid_str[128] = {0};
    sprintf(trigger_sid_str, "/api/device/%s/trigger/%s", API_SID.c_str(), didToHexString(device).c_str());
    push_to_queue(trigger_sid_str);
}

void updateDeviceValue(Device* device, std::string value, int val) {
    log("updateDeviceValue int called!");
    char upd_sid_str[128] = {0};
    sprintf(upd_sid_str, "/api/device/%s/update/%s/%s/%i", API_SID.c_str(), didToHexString(device).c_str(), value.c_str(), val);
    log("API call: " << upd_sid_str);
    push_to_queue(upd_sid_str);
}

void updateDeviceValue(Device* device, std::string value, float val) {
    log("updateDeviceValue float called!");
    char upd_sid_str[128] = {0};
    sprintf(upd_sid_str, "/api/device/%s/update/%s/%s/%i", API_SID.c_str(), didToHexString(device).c_str(), value.c_str(), (int)val);
    log("API call: " << upd_sid_str);
    push_to_queue(upd_sid_str);
}

int main()
{
    log("hub_daemon starting up.");
    log("version " << BUILD_VERSION);
    #ifdef CONFIG_USE_MODEM
    log("compiled with modem support");
    #endif
    #ifdef CONFIG_USE_API
    log("compiled to use API");
    #endif
    #ifndef DEBUG_NO_RF
        log("nRF will be initialised now.");
        rf = new nRF24L01();
        rf->begin();
        if(!rf->isPVariant())
        {
            log("nRF could not start!");
            return 1;
        }
        rf->enableDynamicPayloads();
        rf->setAutoAck(1);
        rf->enableAckPayload();
        rf->setRetries(15,15);
        rf->setDataRate(RF24_250KBPS);
        rf->setPALevel(RF24_PA_MAX);
        rf->setChannel(89);
        rf->setCRCLength(RF24_CRC_8);
        rf->openWritingPipe(0x9AF6ACD8E6LL);
        rf->openReadingPipe(1, 0x9E3DED32E9LL);
        log("nRF started successfully");
        rf->printDetails();
    #endif // DEBUG_NO_RF

    #ifndef DEBUG_NO_RF
    protocol = new ProtocolHandler(rf);
    #endif

    #ifdef CONFIG_USE_MODEM
    if(!modem.hasModem) {
        log("no modem");
    }
    #endif

    #ifndef DEBUG_NO_RF
    loadDevices();
    #endif
    #ifdef CONFIG_USE_API
    APIRegister();
    publishDeviceList();
    pthread_create(&queue_thr, NULL, queue_thread, NULL);
    #endif

    int warn0_stop = 0;
    int stop = 0;

    #ifndef DEBUG_NO_RF
    while(1) {
        char receivePayload[16];
        uint8_t pipe = 1;

        unsigned char device_id[8];
        unsigned char type;
        unsigned char action;

        int pulse = 0;
        float sb_temperature = 0;

        rf->startListening();
        while (rf->available(&pipe)) {
            int len = rf->getDynamicPayloadSize();
            rf->read(receivePayload, len);

            for(int i = 0; i < len; i++) {
                device_id[i] = receivePayload[i];
                printf("%02x ", device_id[i]);
            }
            printf("\n");

            if(len == 0) continue;
            
            log("len = " << len);

            type = receivePayload[8];
            action = receivePayload[9];

            switch(type) {
                case DEVICE_TYPE_SMART_BRACELET:
                    if(action == PROTOCOL_ACTION_POST_TEMPERATURE) {
                        unsigned char temperature[(strlen(receivePayload) - 10)] = {0}; 
                        strncpy ((char*)temperature, receivePayload + 10, (strlen(receivePayload) - 10));
                        sb_temperature = atof((char*)temperature);
                        printf("SB->temperature = %2.1fC\n", sb_temperature);
                        warn0_stop = 0;
                    } else if(action == PROTOCOL_ACTION_POST_PULSE) {
                        unsigned char pulse_[(strlen(receivePayload) - 10)] = {0};
                        strncpy ((char*)pulse_, receivePayload + 10, (strlen(receivePayload) - 10));
                        pulse = atoi((char*)pulse_);
                        printf("SB->pulse = %i BPM\n", pulse);
                    }
                    break;
            }

            Device* device = NULL;

            if(find_device(device_id) == NULL) {
                switch(type) {
                    case DEVICE_TYPE_SMART_BRACELET:
                        device = new SmartBraceletDevice(rf, device_id, "SMART_BRACELET");
                        add_device(device);
                        break;
                    case DEVICE_TYPE_SMOKE:
                        device = new SmokeSensorDevice(rf, device_id, "SMOKE_SENSOR");
                        add_device(device);
                        break;
                }
		#ifdef CONFIG_USE_API
                publishDeviceList();
		#endif
            } else {
                device = find_device(device_id);
            }

            if(type == DEVICE_TYPE_SMART_BRACELET && ((action == 0x02) || (action == 0x12))) {
                log("TRIGGERED SB");
                if(warn0_stop == 0) {
                    #ifdef CONFIG_USE_API
                    trigger(device);
                    #endif
                    warn0_stop = 1;
                    #ifdef CONFIG_USE_MODEM
                    if(modem.hasModem) {
                        modem.sendSMS(CALL_DESTINATION, "ALERTA COD ROSU - SALONUL 1, PATUL 2");
                    }
                    log("SENT SMS");
                    #endif
                }
            } else if(type == DEVICE_TYPE_SMART_BRACELET && action == PROTOCOL_ACTION_POST_PULSE) {
                #ifdef CONFIG_USE_API
                updateDeviceValue(device, "pulse", pulse);
                #endif
            } else if(type == DEVICE_TYPE_SMART_BRACELET && action == PROTOCOL_ACTION_POST_TEMPERATURE) {
                #ifdef CONFIG_USE_API
                updateDeviceValue(device, "temperature", sb_temperature);
                #endif
                warn0_stop = 0;
            } else if (type == DEVICE_TYPE_SMOKE && action == PROTOCOL_ACTION_TRIGGER_HIGH_TEMPERATURE) {
                if (!stop) {
                    log("HIGH TEMPERATURE DETECTED! SENDING REQUEST AND INITIATING PHNCALL!");
                    #ifdef CONFIG_USE_API
                    trigger(device);
                    #endif
                    #ifdef CONFIG_USE_MODEM
                    if(modem.hasModem) {
                        int dR = modem.voiceCall(CALL_DESTINATION);
                        if (dR == 2) {
                            stop = 1;
                            modem.playFile("/home/pi/hub_daemon/messages/alert.wav");
                        }
                    }
                    #endif
                }
            } else if (type == DEVICE_TYPE_SMOKE && action == PROTOCOL_ACTION_TRIGGER) {
                log("trigger smoke");
                #ifdef CONFIG_USE_API
                trigger(device);
                #endif
                if (!stop) {
                    log("SMOKE DETECTED! SENDING REQUEST AND INITIATING PHNCALL!");
                    trigger(device);
                    #ifdef CONFIG_USE_MODEM
                    if(modem.hasModem) {
                        int dR = modem.voiceCall(CALL_DESTINATION);
                        if (dR == 2) {
                            stop = 1;
                            modem.playFile("/home/pi/hub_daemon/messages/alert.wav");
                        }
                    }
                    #endif
                }
            } else if(action == PROTOCOL_ACTION_IDLE) {
                stop = 0;
            }
        }
    }
    #endif

    return 0;
}
