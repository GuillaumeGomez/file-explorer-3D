#include "UDP.hpp"
#include "../Utils/HandleThread.hpp"
#include "../Utils/HandleMutex.hpp"
#include <cstring>

#define PORT 2424

void *handle_udp_data(void *obj) {
    static_cast<UDP*>(obj)->listenClients();
    return 0;
}

UDP::UDP(bool server_mode, int port) : sock(-1), server_mode(server_mode), thread(0), mutex(0), client_mutex(0), nbWaitingData(0) {
    waitingData.reserve(1000);
    memset(&server, 0, sizeof(server));
    if (server_mode) {
        this->port = PORT;
    } else {
        this->port = port;
    }
    client_sock = -1;
    memset(&server, 0, sizeof(server));
}

UDP::~UDP() {
    if (sock != -1)
        close(sock);
    if (client_sock != -1)
        close(client_sock);
    if (thread)
        delete thread;
    if (mutex)
        delete mutex;
}

bool UDP::start(const char *server_addr) {
    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        std::cerr << "cannot create socket" << std::endl;
        return false;
    }
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    if (server_mode) {
        server.sin_addr.s_addr = htonl(INADDR_ANY);
        if (bind(sock, (struct sockaddr *)&server, sizeof(server)) < 0) {
            std::cerr << "bind failed" << std::endl;
            return false;
        }
    } else {
        server.sin_addr.s_addr = inet_addr(server_addr);
    }
    if (!server_mode) {
        //client_sock = socket(AF_INET, SOCK_DGRAM, 0);
        //s_client.sin_port = htons(PORT);
        //s_client.sin_family = AF_INET;
        //s_client.sin_port = htons(port);
        Vector3D t;
        this->send(t, 0.f, 0.f);
    }
    client_mutex = new HandleMutex;
    mutex = new HandleMutex;
    thread = new HandleThread(handle_udp_data, static_cast<void*>(this));
    thread->start();
    return true;
}

void UDP::listenClients() {
    struct sockaddr_in remaddr;
    socklen_t addrlen = sizeof(remaddr);
    int recvlen;
    character_data buf;

    for (;;) {
        recvlen = recvfrom(this->sock, &buf, sizeof(buf), 0, (struct sockaddr *)&remaddr, &addrlen);
        std::cout << "new udp data" << std::endl;
        if (!server_mode)
        if (recvlen == sizeof(buf)) {
            mutex->lock();
            if (waitingData.capacity() < nbWaitingData)
                waitingData[nbWaitingData] = buf;
            else
                waitingData.push_back(buf);
            ++nbWaitingData;
            mutex->unlock();
            if (server_mode) {
                client_mutex->lock();
                for (std::vector<client>::iterator it = clients.begin(); it != clients.end(); ++it) {
                    sendto(sock, &(*it).data, sizeof((*it).data), 0, (sockaddr*)&buf, sizeof(buf));
                }
                client_mutex->unlock();
            }
        }
    }
}

const std::vector<character_data> &UDP::getData() {
    return waitingData;
}

unsigned int UDP::getNbWaitingData() {
    return nbWaitingData;
}

void UDP::resetData() {
    nbWaitingData = 0;
}

HandleMutex *UDP::getMutex() {
    return mutex;
}

bool UDP::isServer() {
    return server_mode;
}

void UDP::addClient(int id, struct sockaddr_in data) {
    for (std::vector<client>::iterator it = clients.begin(); it != clients.end(); ++it) {
        if ((*it).id == id)
            return ;
    }
    client_mutex->lock();
    clients.push_back(client{id, data});
    std::cout << "client added : " << inet_ntoa(data.sin_addr) << std::endl;
    client_mutex->unlock();
}

void UDP::send(Vector3D &pos, float theta, float phi, int id) {
    character_data d{id, (int)(theta * 10), (int)(phi * 10), (int)(pos.x() * 10), (int)(pos.y() * 10), (int)(pos.z() * 100)};

    if (server_mode) {
        client_mutex->lock();
        for (std::vector<client>::iterator it = clients.begin(); it != clients.end(); ++it) {
            if ((*it).id != id)
                sendto(sock, &d, sizeof(d), 0, (sockaddr*)&(*it).data, sizeof((*it).data));
        }
        client_mutex->unlock();
    } else {
        sendto(sock, &d, sizeof(d), 0, (sockaddr*)&server, sizeof(server));
    }
}
