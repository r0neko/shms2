#ifndef HTTPCLIENT_H
#define HTTPCLIENT_H

#include "../debug_logger.h"

#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>

class HTTPClient
{
    public:
        HTTPClient() {}
        HTTPClient(std::string address, int port);
        virtual ~HTTPClient();

        std::string get(std::string path);
        std::string post(std::string path, std::string data);
    protected:

    private:
        int sock;
        struct sockaddr_in client;
        std::string address;
        int openConnection();
        int closeConnection();
};

#endif // HTTPCLIENT_H
