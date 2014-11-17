#ifndef __UDP_HPP__
#define __UDP_HPP__

#include <string>
#include <vector>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <iostream>
#include <stdint.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "../objects/Vector3D.hpp"

class HandleThread;
class HandleMutex;

typedef struct {
    int32_t id;
    int32_t theta;
    int32_t phi;
    int32_t x;
    int32_t y;
    int32_t z;
} character_data;

typedef struct {
    bool ok;
    int id;
    struct sockaddr_in data;
} client;

class UDP {
public:
    UDP(bool server_mode = true, int port = 0);
    ~UDP();
    bool start(const char *server_addr = NULL);
    const std::string &getAddr() const;
    void listenClients();
    const std::vector<character_data> &getData();
    HandleMutex *getMutex();
    HandleMutex *getClientMutex();
    unsigned int getNbWaitingData();
    void resetData();
    bool isServer();
    void send(Vector3D &pos, float theta, float phi, int id = 0);
    void addClient(int id, struct sockaddr_in data);
    std::vector<client> &getClients();
    void pushNewData(character_data *buf);

private:
    int sock;
    bool server_mode;
    HandleThread *thread;
    HandleMutex *mutex;
    HandleMutex *client_mutex;
    unsigned int nbWaitingData;
    std::string addr;
    std::vector<character_data> waitingData;
    std::vector<client> clients;
    struct sockaddr_in server;
    int port;
};

#endif
