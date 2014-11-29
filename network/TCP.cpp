#include "TCP.hpp"
#include "../Utils/HandleThread.hpp"
#include "../Utils/HandleMutex.hpp"
#include <sys/time.h>
#include <cstring>
#include <stdint.h>
#ifndef WIN32
#include <sys/select.h>
#endif

#define PORT 2425

#define NEW_CLIENT 1
#define QUIT_CLIENT 2
#define SET_ID 0
#define WATCHDOG 4

typedef struct {
    char type;
    int32_t size;
} header;

bool getFullData(int fd, void *data, int size) {
    int got = 0;
    char *pointer = (char*)data;

    while (got < size) {
        int ret = recv(fd, pointer + got, size - got, 0);

        if (ret < 1)
            return false;
        got += ret;
    }
    return true;
}

void *handle_tcp_data(void *obj) {
    static_cast<TCP*>(obj)->loop();
    return 0;
}

TCP::TCP(bool server_mode) : sock(-1), server_mode(server_mode), thread(0), mutex(0) {
    id = 0;
#ifdef WIN32
    start_network();
#endif
}

TCP::~TCP() {
    if (thread)
        delete thread;
    if (mutex)
        mutex->unlock();
    this->close();
    if (mutex)
        delete mutex;
}

bool TCP::start(const char *addr) {
    struct sockaddr_in sin = {0};

    sin.sin_family = AF_INET;
    sin.sin_port = htons(PORT);
    if (!server_mode) {
        if (!addr)
            return false;

        this->addr = addr;
        if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
            perror("socket");
#ifdef WIN32
            closesocket(sock);
#else
            ::close(sock);
#endif
            return false;
        }

        sin.sin_addr.s_addr = inet_addr(addr);
        if (connect(sock, (struct sockaddr *)&sin, sizeof(sockaddr_in)) < 0) {
#ifdef WIN32
            closesocket(sock);
#else
            ::close(sock);
#endif
            sock = -1;
            perror("connect");
            return false;
        }
        std::cout << "connected to server !" << std::endl;
    } else {
        if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
#ifdef WIN32
            closesocket(sock);
#else
            ::close(sock);
#endif
            perror("socket");
            return false;
        }

        sin.sin_addr.s_addr = htonl(INADDR_ANY);
        if (bind(sock, (struct sockaddr *) &sin, sizeof(sin)) != 0) {
#ifdef WIN32
            closesocket(sock);
#else
            ::close(sock);
#endif
            sock = -1;
            perror("bind");
            return false;
        }

        if (listen(sock, 10) != 0) {
#ifdef WIN32
            closesocket(sock);
#else
            ::close(sock);
#endif
            sock = -1;
            perror("listen");
            return false;
        }
        std::cout << "server launched !" << std::endl;
    }
    mutex = new HandleMutex;
    thread = new HandleThread(handle_tcp_data, static_cast<void*>(this));
    thread->start();
    return true;
}

void TCP::accept_new_client() {
    int cli;
    struct sockaddr_in data;
    socklen_t len = sizeof(data);

    cli = accept(sock, (sockaddr*)&data, &len);

    if (cli < 0) {
        perror("accept");
        return;
    }
    client cli_data = {false, cli, data};

    char ak[sizeof(int32_t) + 1 + sizeof(int)] = {SET_ID, 0};
    int tmp = sizeof(int);

    // send id to new client
    memcpy(ak + 1 + sizeof(int32_t), &cli, sizeof(cli));
    memcpy(ak + 1, &tmp, sizeof(tmp));
    this->send(cli, &ak, sizeof(ak));

    // send new client's id to everyone
    ak[0] = NEW_CLIENT;
    memcpy(ak + 1 + sizeof(int32_t), &cli, sizeof(cli));
    sendToEveryone(ak, sizeof(ak), cli);

    // send all clients' id to new one
    for (auto it = clients.begin(); it != clients.end(); ++it) {
        memcpy(ak + 1 + sizeof(int32_t), &(*it), sizeof(*it));
        this->send(cli, &ak, sizeof(ak));
    }
    tmp = 0;
    memcpy(ak + 1 + sizeof(int32_t), &tmp, sizeof(tmp));
    this->send(cli, &ak, sizeof(ak));

    // add new client to pending list
    mutex->lock();
    pending_clients.push_back(cli_data);
    mutex->unlock();
    clients.push_back(cli);
    std::cout << "new client !" << std::endl;
}

int TCP::send(int fd, void *data, size_t len) {
    size_t sent = 0;
    size_t tmp;
    char *c_data = (char*)data;

    while (sent < len) {
        if ((tmp = ::send(fd, c_data + sent, len - sent, 0)) < 1)
            return 0;
        sent += tmp;
    }
    return 1;
}

void TCP::sendToEveryone(void *data, size_t len, int except) {
    if (!server_mode)
        return;
    for (auto it = clients.begin(); it != clients.end(); ++it) {
        if ((*it) != except) {
            this->send((*it), data, len);
        }
    }
}

bool TCP::isConnected() {
    return sock != -1;
}

void TCP::close() {
    if (mutex)
        mutex->lock();
    if (server_mode) {
        for (auto it : clients) {
#ifdef WIN32
            closesocket(it);
#else
            ::close(it);
#endif
        }
        for (auto it : pending_clients) {
#ifdef WIN32
            closesocket(it.id);
#else
            ::close(it.id);
#endif
        }
        for (auto it : quit_clients) {
#ifdef WIN32
            closesocket(it);
#else
            ::close(it);
#endif
        }
    }
    if (sock > -1)
#ifdef WIN32
            closesocket(sock);
#else
            ::close(sock);
#endif
    sock = -1;
    if (mutex)
        mutex->unlock();
}

void TCP::loop() {
    fd_set readfs;

    if (server_mode) {
        int max;
        int ret;

        for (;;) {
            max = sock;
            FD_ZERO(&readfs);
            FD_SET(sock, &readfs);
            for (auto it = clients.begin(); it != clients.end(); ++it) {
                FD_SET(*it, &readfs);
                if (*it > max)
                    max = *it;
            }
            ret = select(max + 1, &readfs, 0, 0, 0);
            if (ret < 0) {
                std::cerr << "select error !" << std::endl;
            } else {
                if (FD_ISSET(sock, &readfs)) {
                    this->accept_new_client();
                } else {
                    char head_data[5];

                    for (unsigned int it = 0; it < clients.size(); ++it) {
                        int client = clients[it];

                        if (FD_ISSET(client, &readfs)) {
                            if (!getFullData(client, head_data, sizeof(head_data))) {
                                std::cout << "Client disconnected !" << std::endl;
#ifdef WIN32
                                closesocket(client);
#else
                                ::close(client);
#endif
                                clients.erase(clients.begin() + it);
                                it -= 1;
                                // send to all clients that this one is now disconnected
                                char deco[5 + sizeof(int)] = {QUIT_CLIENT, 0};

                                int tmp = sizeof(int);
                                memcpy(deco + 1, &tmp, sizeof(tmp));
                                memcpy(deco + 5, &client, sizeof(client));
                                for (auto tmp2 = clients.begin(); tmp2 != clients.end(); ++tmp2) {
                                    this->send(*tmp2, deco, sizeof(deco));
                                }
                            } else {
                                head_data[0] = WATCHDOG;
                                int tmp = 0;
                                memcpy(head_data + 1, &tmp, sizeof(tmp));
                                if (this->send(client, head_data, sizeof(head_data)) < 1) {
                                    std::cout << "Client disconnected !" << std::endl;
#ifdef WIN32
                                    closesocket(client);
#else
                                    ::close(client);
#endif
                                    clients.erase(clients.begin() + it);
                                    it -= 1;
                                    // send to all clients that this one is now disconnected
                                    char deco[5 + sizeof(int)] = {QUIT_CLIENT, 0};

                                    tmp = sizeof(int);
                                    memcpy(deco + 1, &tmp, sizeof(tmp));
                                    memcpy(deco + 5, &client, sizeof(client));
                                    for (auto tmp2 = clients.begin(); tmp2 != clients.end(); ++tmp2) {
                                        this->send(*tmp2, deco, sizeof(deco));
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    } else {
        for (;;) {
            int ret;
            struct timeval tv;

            tv.tv_sec = 15;
            tv.tv_usec = 0;

            FD_ZERO(&readfs);
            FD_SET(sock, &readfs);
            ret = select(sock + 1, &readfs, 0, 0, &tv);
            if (ret < 0) {
                perror("select");
            } else if (!ret) {
                //timeout
                char head[5] = {WATCHDOG, 0};

                ::send(sock, head, sizeof(head), 0);
            } else {
                char head_data[5];

                if (!getFullData(sock, head_data, sizeof(head_data))) {
                    std::cerr << "Disconnected from server !" << std::endl;
                    this->close();
                    return;
                }
                header head;
                head.type = head_data[0];
                memcpy(&head.size, head_data + 1, 4);
                int client_id = 0;

                if (head.size == 4) {
                    if (!getFullData(sock, &client_id, sizeof(client_id))) {
                        std::cerr << "Disconnected from server !" << std::endl;
                        this->close();
                        return;
                    }
                }
                std::cout << "[" << (int)head.type << ", " << head.size << ", " << client_id << "]" << std::endl;
                switch (head.type) {
                    case NEW_CLIENT:
                        std::cout << "New client !" << std::endl;
                        mutex->lock();
                        pending_clients.push_back(client{false, client_id, 0});
                        mutex->unlock();
                        break;
                    case QUIT_CLIENT:
                        std::cout << "A client left..." << std::endl;
                        mutex->lock();
                        quit_clients.push_back(client_id);
                        mutex->unlock();
                        break;
                    case SET_ID:
                        id = client_id;
                        std::cout << "my id is : " << id << std::endl;
                        break;
                    case WATCHDOG:
                    default:
                        break;
                }
            }
        }
    }
}

bool TCP::isServer() {
    return server_mode;
}

std::vector<client> &TCP::getPendingClients() {
    return pending_clients;
}

std::vector<SOCKET> &TCP::getQuitClients() {
    return quit_clients;
}

HandleMutex *TCP::getMutex() {
    return mutex;
}

const char *TCP::getAddr() {
    return addr.c_str();
}

int TCP::getId() {
    return id;
}
