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
    std::vector<SOCKET> &getQuitClients();
    int send(int fd, void *data, size_t len);
    const char *getAddr();
    int getId();
    bool isConnected();
    void close();

private:
    SOCKET sock;
    bool server_mode;
    HandleThread *thread;
    HandleMutex *mutex;
    std::string addr;
    int id;
    std::vector<SOCKET> clients;
    std::vector<client> pending_clients;
    std::vector<SOCKET> quit_clients;
};

#endif
