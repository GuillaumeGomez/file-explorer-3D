#include "UDP.hpp"
#include "../Utils/HandleThread.hpp"
#include "../Utils/HandleMutex.hpp"

void *handle_udp_data(void *obj) {
    static_cast<UDP*>(obj)->listenClients();
    return 0;
}

UDP::UDP(bool server_mode) : sock(-1), server_mode(server_mode), thread(0), mutex(0), nbWaitingData(0) {
    waitingData.reserve(1000);
}

UDP::~UDP() {
    if (sock != -1)
        close(sock);
    if (thread)
        delete thread;
    if (mutex)
        delete mutex;
}

void UDP::start(const char *server_addr) {
    if (server_mode) {
        struct sockaddr_in myaddr = {0};

        if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
                std::cerr << "cannot create socket" << std::endl;
                return;
        }

        myaddr.sin_family = AF_INET;
        myaddr.sin_addr.s_addr = htonl(INADDR_ANY);
        myaddr.sin_port = htons(2424);

        if (bind(sock, (struct sockaddr *)&myaddr, sizeof(myaddr)) < 0) {
                std::cerr << "bind failed" << std::endl;
                return;
        }

        mutex = new HandleMutex;
        thread = new HandleThread(handle_udp_data, static_cast<void*>(this));
        thread->start();
    }
}

void UDP::listenClients() {
    struct sockaddr_in remaddr;
    socklen_t addrlen = sizeof(remaddr);
    int recvlen;
    character_data buf;

    for (;;) {
        recvlen = recvfrom(this->sock, &buf, sizeof(buf), 0, (struct sockaddr *)&remaddr, &addrlen);
        std::cout << "received " << recvlen << " bytes" << std::endl;
        if (recvlen == sizeof(buf)) {
            mutex->lock();
            if (waitingData.capacity() < nbWaitingData)
                waitingData[nbWaitingData] = buf;
            else
                waitingData.push_back(buf);
            ++nbWaitingData;
            mutex->unlock();
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
            return;
    }
    clients.push_back(client{id, data});
}

void UDP::send(Vector3D &pos, float theta, float phi, int id) {
    if (server_mode) {
        character_data d{id, theta * 10, phi * 10, pos.x() * 10, pos.y() * 10, pos.z() * 10};

        for (std::vector<client>::iterator it = clients.begin(); it != clients.end(); ++it) {
            sendto(sock, &d, sizeof(d), 0, (sockaddr*)&(*it).data, sizeof((*it).data));
        }
    } else {
        character_data d{id, theta * 10, phi * 10, pos.x() * 10, pos.y() * 10, pos.z() * 10};

        sendto(sock, &d, sizeof(d), 0, (sockaddr*)&server, sizeof(server));
    }
}
