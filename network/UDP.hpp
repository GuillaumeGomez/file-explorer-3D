#include <string>
#include <vector>

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

private:
    int sock;
    bool server_mode;
    HandleThread *thread;
    HandleMutex *mutex;
    unsigned int nbWaitingData;
    std::string addr;
    std::vector<character_data> waitingData;
};
