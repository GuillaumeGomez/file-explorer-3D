#include "UDP.hpp"
#include "../Utils/HandleThread.hpp"
#include "../Utils/HandleMutex.hpp"
#include "../Utils/MyMutexLocker.hpp"
#include "../Camera.hpp"
#include <cstring>

#define PORT 2424

void *handle_udp_data(void *obj) {
    static_cast<UDP*>(obj)->listenClients();
    return 0;
}

void *handle_send_data(void *obj) {
    static_cast<UDP*>(obj)->sendData();
    return 0;
}

UDP::UDP(Camera *player, int id, bool server_mode) : sock(-1), id(id), server_mode(server_mode), thread(0), send_thread(0), mutex(0), client_mutex(0),
    nbWaitingData(0) {
    waitingData.reserve(1000);
    memset(&server, 0, sizeof(server));
    if (server_mode) {
        this->port = PORT;
    } else {
        this->port = 0;
    }
    memset(&server, 0, sizeof(server));
    this->player = player;
}

UDP::~UDP() {
    if (sock != -1)
        close(sock);
    if (thread)
        delete thread;
    if (send_thread)
        delete send_thread;
    if (mutex)
        delete mutex;
    if (client_mutex)
        delete client_mutex;
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
        float tmp = 0.f;
        this->send(t, tmp);
    }
    client_mutex = new HandleMutex;
    mutex = new HandleMutex;
    thread = new HandleThread(handle_udp_data, static_cast<void*>(this));
    thread->start();
    send_thread = new HandleThread(handle_send_data, static_cast<void*>(this));
    send_thread->start();
    return true;
}

void UDP::pushNewData(character_data *buf) {
    MyMutexLocker l(this->mutex);

    (void)l;
    waitingData.push_back(*buf);
    ++nbWaitingData;
}

void UDP::listenClients() {
    struct sockaddr_in remaddr;
    socklen_t addrlen = sizeof(remaddr);
    int recvlen;
    character_data buf;

    for (;;) {
        recvlen = recvfrom(this->sock, &buf, sizeof(buf), 0, (struct sockaddr *)&remaddr, &addrlen);
        if (recvlen == sizeof(buf) && (buf.id || !server_mode)) {
            this->pushNewData(&buf);
            if (server_mode) {
                client_mutex->lock();
                for (std::vector<client>::iterator it = clients.begin(); it != clients.end(); ++it) {
                    if (buf.id != (*it).id) {
                        if ((*it).ok)
                            sendto(sock, &(*it).data, sizeof((*it).data), 0, (sockaddr*)&buf, sizeof(buf));
                    } else if (!(*it).ok) {
                        memcpy(&(*it).data, &remaddr, sizeof(remaddr));
                        (*it).ok = true;
                    }
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
    waitingData.clear();
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
    clients.push_back(client{false, id, data});
    client_mutex->unlock();
}

void UDP::send(Vector3D &pos, float &theta, int id) {
    character_data d{id, (int)(pos.x() * 10), (int)(pos.y() * 10), (int)(pos.z() * 10), (int)(theta * 10)};

    if (server_mode) {
        client_mutex->lock();
        for (std::vector<client>::iterator it = clients.begin(); it != clients.end(); ++it) {
            if ((*it).id != id && (*it).ok)
                sendto(sock, &d, sizeof(d), 0, (sockaddr*)&(*it).data, sizeof((*it).data));
        }
        client_mutex->unlock();
    } else {
        sendto(sock, &d, sizeof(d), 0, (sockaddr*)&server, sizeof(server));
    }
}

void UDP::sendData() {
    for (;;) {
        usleep(1000000 / 60); // one send every 1/60 second
        player->lock();
        this->send(player->getPosition(), player->getTheta(), id);
        player->unlock();
    }
}
