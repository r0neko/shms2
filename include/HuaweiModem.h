#ifndef HUAWEIMODEM_H
#define HUAWEIMODEM_H

#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <sstream>

class HuaweiModem
{
    public:
        HuaweiModem() {};
        HuaweiModem(std::string cmd_port, std::string data_port);

        int hasSIM();
        int sendSMS(std::string destination, std::string text);
        int voiceCall(std::string destination);
        void playFile(std::string location);

        int hasModem = 1;
    protected:
    private:
        int fd;
        int fdAudio;

        int set_interface_attribs(int fd, int speed, int parity);

        int writePCUI(std::string command);
        std::string readPCUIBlocking();
        std::string readPCUI();
        std::string readPCUIUntil(std::string until);
        void flush();

        std::string executeCommand(std::string command);
        std::string executeCommandUntil(std::string command, std::string until);
};

#endif // HUAWEIMODEM_H