#ifndef IPCMANAGER_H
#define IPCMANAGER_H
#include "stdafx.h"
#include "messege.h"

#include <sstream>

#define PORT 1234

#define COORDINATOR_MSG_STR  "COORDINATOR"
#define ELECTION_MSG_STR     "ELECTION"
#define ALIVE_MSG_STR        "ALIVE"


// This class is responsable for all communications between all processes
class IPCManager
{
public:
    // Singleton create or return instance
    static IPCManager *instance();
    ~IPCManager();
    void broadcastMessage(Messege &messege);
    bool readBroadcastMessage(Messege &msg, int timeout = 0); // receive messeges broadcast from other processes
private:
    IPCManager(); // private default constructo (for singleton)


	WSADATA m_wsaData;
    SOCKET m_broadcastSocket;
    SOCKET m_receiveSocket;
    sockaddr_in m_broadcastAddress;
    static IPCManager *m_instance;

};

#endif // IPCMANAGER_H
