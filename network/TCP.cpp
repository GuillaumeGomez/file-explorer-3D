#include "TCP.hpp"
#include "../Utils/HandleThread.hpp"
#include "../Utils/HandleMutex.hpp"
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/select.h>
#include <sys/time.h>

#define PORT 2425

void *handle_tcp_data(void *obj) {
    static_cast<TCP*>(obj)->loop();
    return 0;
}

TCP::TCP(bool server_mode) : sock(-1), server_mode(server_mode), thread(0), mutex(0) {
}

TCP::~TCP() {
    if (sock != -1)
        close(sock);
    if (thread)
        delete thread;
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

        if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
            close(sock);
            return false;
        }

        sin.sin_addr.s_addr = inet_addr(addr);
        if (connect(sock, (struct sockaddr *)&sin, sizeof(sockaddr_in)) < 0) {
            std::cerr << "Unable to connect to server..." << std::endl;
            close(sock);
            sock = -1;
            return false;
        }
    } else {
        if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
            close(sock);
            return false;
        }

        sin.sin_addr.s_addr = htonl(INADDR_ANY);
        if (bind(sock, (struct sockaddr *) &sin, sizeof(sin)) < 0) {
            close(sock);
            sock = -1;
            perror("bind");
            return false;
        }

        listen(sock, 10);
    }
    mutex = new HandleMutex;
    thread = new HandleThread(handle_tcp_data, static_cast<void*>(this));
    thread->start();
    return true;
}

void TCP::accept_new_client() {
    int client;

    if ((client = accept(sock, NULL, NULL)) < 0) {
        return;
    }
    mutex->lock();
    pending_clients.push_back(client);
    mutex->unlock();
}

void TCP::loop() {
    if (server_mode) {
        fd_set readfs;
        int max;

        for (;;) {
            max = sock;
            FD_ZERO(&readfs);
            FD_SET(sock, &readfs);
            for (std::vector<int>::iterator it = clients.begin(); it != clients.end(); ++it) {
                FD_SET(*it, &readfs);
                if (*it > max)
                    max = *it;
            }
            if (select(max + 1, &readfs, 0, 0, 0) < 0) {
                std::cerr << "select error !" << std::endl;
            } else {
                if (FD_ISSET(sock, &readfs)) {
                    this->accept_new_client();
                } else {
                    char useless;

                    for (int it = 0; it < clients.size(); ++it) {
                        int client = clients[it];

                        if (FD_ISSET(client, &readfs)) {
                            if (recv(client, &useless, sizeof(useless), 0) < 1 || send(client, &useless, 1, 0) < 1) {
                                close(client);
                                clients.erase(clients.begin() + it);
                                it -= 1;
                            }
                        }
                    }
                }
            }
        }
    } else {
        mutex->lock();
        if (recv(sock, &id, sizeof(id), 0) < 1) {
            std::cerr << "Disconnected from server !" << std::endl;
        } else {
            mutex->unlock();
            // keep connection alive !
            for (;;) {
                char useless;

                sleep(15);
                if (send(sock, "1", 1, 0) < 1 || recv(sock, &useless, sizeof(useless), 0) < 1) {
                    std::cerr << "Disconnected from server !" << std::endl;
                    return;
                }
            }
        }
    }
}

bool TCP::isServer() {
    return server_mode;
}

std::vector<int> &TCP::getPendingClients() {
    return pending_clients;
}
