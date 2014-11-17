#ifndef __TCP_HPP__
#define __TCP_HPP__

#include <string>
#include <vector>
#include "UDP.hpp"

class HandleThread;
class HandleMutex;

class TCP {
public:
    TCP(bool server_mode = true);
    ~TCP();
    bool start(const char *addr = NULL);
    bool isServer();
    void loop();
    void accept_new_client();
    std::vector<client> &getPendingClients();
    HandleMutex *getMutex();
    void sendToEveryone(void *data, size_t len, int except = -1);
    std::vector<int> &getQuitClients();
    void send(int fd, void *data, size_t len);
    int getPortNumber();
    const char *getAddr();
    int getId();

private:
    int sock;
    bool server_mode;
    HandleThread *thread;
    HandleMutex *mutex;
    std::string addr;
    int id;
    std::vector<int> clients;
    std::vector<client> pending_clients;
    std::vector<int> quit_clients;
    int port_number;
};

#endif
