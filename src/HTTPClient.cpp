#include "HTTPClient.h"

HTTPClient::HTTPClient(std::string address, int port)
{
  this->client.sin_addr.s_addr = inet_addr(address.c_str()); // Server #1 IP , 4 servers free(5 in total)
  this->client.sin_family = AF_INET;
  this->client.sin_port = htons(port);
  this->address = address;
}

HTTPClient::~HTTPClient()
{
    //dtor
}

int HTTPClient::openConnection() {
    this->sock = socket(AF_INET , SOCK_STREAM , 0);
    if (this->sock == -1) log("Could not create socket!");
    if (connect(this->sock, (struct sockaddr *)&this->client , sizeof(this->client)) < 0) {
        log("Connection failure!");
        return 0;
    } else {
        log("Connected successfully!");
        return 1;
    }
}

int HTTPClient::closeConnection() {
    if (shutdown(this->sock, SHUT_RDWR) == 0) {
        log("Closed connection successfully.");
        return 1;
    } else {
        log("Could not close connection successfully.");
        return 0;
    }
}

std::string HTTPClient::get(std::string path)
{
    if (!this->openConnection()) {
        this->closeConnection();
        return "";
    }

    char msgBuffer[512] = {0}; // get 512 bytes...
    std::string response;
    char buf[512] = {0};

    int x = sprintf(msgBuffer, "GET %s HTTP/1.1\r\n", path.c_str());
    x += sprintf(msgBuffer + x, "Host: %s\r\n", this->address.c_str());
    x += sprintf(msgBuffer + x, "User-Agent: SHMSCORE/1.0\r\n");
    x += sprintf(msgBuffer + x, "Accept: application/json;q=0.9\r\n");
    x += sprintf(msgBuffer + x, "Connection: close\r\n");
    x += sprintf(msgBuffer + x, "\r\n\r\n");

    if ( send(this->sock, msgBuffer, x, 0) < 0) {
        log("SEND FAILURE!");
    } else {
        int i = 0;
        while (strcmp(buf, "\r\n")) {
          for (i = 0; strcmp(buf + i - 2, "\r\n"); i++) {
            recv(this->sock, buf + i, 1, 0);
            buf[i + 1] = '\0';
          }
          if (strstr(buf, "HTTP/") == buf) {
            fputs(strchr(buf, ' ') + 1, stdout);
          }
        }
        int b_read = 0;
        while((b_read = recv(this->sock, buf, sizeof(buf), 0))) {
            response.append(buf, b_read);
        }
    }
    shutdown(this->sock, SHUT_RDWR);
    close(this->sock);
    return response;
}

std::string HTTPClient::post(std::string path, std::string data)
{
    if (!this->openConnection()) {
        this->closeConnection();
        return "";
    }

    char msgBuffer[512] = {0}; // get 512 bytes...
    std::string response;
    char buf[512] = {0};

    int x = sprintf(msgBuffer, "POST %s HTTP/1.1\r\n", path.c_str());
    x += sprintf(msgBuffer + x, "Host: %s\r\n", this->address.c_str());
    x += sprintf(msgBuffer + x, "User-Agent: SHMSCORE/1.0\r\n");
    x += sprintf(msgBuffer + x, "Accept: application/json;q=0.9\r\n");
    x += sprintf(msgBuffer + x, "Content-Type: application/json; charset=UTF-8\r\n");
    x += sprintf(msgBuffer + x, "Content-Length: %i\r\n", strlen(data.c_str()));
    x += sprintf(msgBuffer + x, "Connection: close\r\n\r\n");
    x += sprintf(msgBuffer + x, "%s", data.c_str());

    if ( send(this->sock, msgBuffer, x, 0) < 0) {
        log("SEND FAILURE!");
    } else {
        int i = 0;
        while (strcmp(buf, "\r\n")) {
          for (i = 0; strcmp(buf + i - 2, "\r\n"); i++) {
            recv(this->sock, buf + i, 1, 0);
            buf[i + 1] = '\0';
          }
          if (strstr(buf, "HTTP/") == buf) {
            fputs(strchr(buf, ' ') + 1, stdout);
          }
        }
        int b_read = 0;
        while((b_read = recv(this->sock, buf, sizeof(buf), 0))) {
            response.append(buf, b_read);
        }
    }
    this->closeConnection();
    return response;
}