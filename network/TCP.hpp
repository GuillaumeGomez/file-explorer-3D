#include <string>
#include <vector>

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
    std::vector<int> &getPendingClients();

private:
    int sock;
    bool server_mode;
    HandleThread *thread;
    HandleMutex *mutex;
    std::string addr;
    int id;
    std::vector<int> clients;
    std::vector<int> pending_clients;
};
