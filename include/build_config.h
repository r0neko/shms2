/**

HUB DAEMON BUILD CONFIGURATION

- written by Talnaci Alexandru -
- version 1.0.0 -

- Copyright: Talnaci Alexandru (c)
             S.H.M.S. Systems (c)

**/

#define BUILD_VERSION "v1.2RC1"

//#define DEBUG_NO_RF // for testing purposes on PC
#define MAXIMUM_DEVICES_ALLOWED 64 // 64 devices for demo purposes and development

#define DEBUG
// RF24
#define MINIMAL
// Uncomment above definition to use minimal memory in RF24

// API
#define CONFIG_USE_API
#define CONFIG_SERVER_IP "192.168.2.3"
#define CONFIG_SERVER_PORT 80

// Modem
#define CONFIG_USE_MODEM
#define CALL_DESTINATION "REDACTEDOWNPHONENUMBER"

// DEVICE CONSTANTS - OLD PROTOCOL

#define DEVICE_TYPE_SMOKE 0x0b
#define DEVICE_TYPE_SMART_BRACELET 0x1a

#define PROTOCOL_ACTION_IDLE 0x00
#define PROTOCOL_ACTION_TRIGGER 0x02
#define PROTOCOL_ACTION_TRIGGER_HIGH_TEMPERATURE 0xF2
#define PROTOCOL_ACTION_TRIGGER_SB_PULSE 0x11
#define PROTOCOL_ACTION_TRIGGER_SB_TEMPERATURE 0x12
#define PROTOCOL_ACTION_POST_PULSE 0x13
#define PROTOCOL_ACTION_POST_TEMPERATURE 0x14
