#include <string>
#include <vector>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <iostream>

#include "../objects/Vector3D.hpp"

class HandleThread;
class HandleMutex;

typedef struct {
    int id;
    int theta;
    int phi;
    int x;
    int y;
    int z;
} character_data;

typedef struct {
    int id;
    struct sockaddr_in data;
} client;

class UDP {
public:
    UDP(bool server_mode = true);
    ~UDP();
    void start(const char *server_addr = NULL);
    const std::string &getAddr() const;
    void listenClients();
    const std::vector<character_data> &getData();
    HandleMutex *getMutex();
    unsigned int getNbWaitingData();
    void resetData();
    bool isServer();
    void send(Vector3D &pos, float theta, float phi, int id = 0);
    void addClient(int id, struct sockaddr_in data);

private:
    int sock;
    bool server_mode;
    HandleThread *thread;
    HandleMutex *mutex;
    unsigned int nbWaitingData;
    std::string addr;
    std::vector<character_data> waitingData;
    std::vector<client> clients;
    struct sockaddr_in server;
};
