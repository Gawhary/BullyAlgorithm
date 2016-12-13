#ifndef IPCMANAGER_H
#define IPCMANAGER_H
#include "stdafx.h"
#include "messege.h"

#include <sstream>

#define UDP_PORT 8888
#define TCP_PORT 2222

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
    void broadcastMessage(Messege &messege); // used for sending Election, Coordinator, and Alive messeges
    bool readBroadcastMessage(Messege &msg, int timeout = 0); // receive messeges broadcast from other processes
    bool connectToServer(sockaddr_in serverAddress); // called by worker once when enter slave mode
    void disconnectServer(); // called by worker before exit slave mode
    bool requestSubTask(Messege &task); // send work request to the coordinator and wait for response
    bool recieveRequest(Messege &request, int timeout = 0); // receive messeges from slave process
    bool serverConnected(){
        return m_connectedToServer;
    }
    bool sendToServer(Messege msg); // send task result (no responce)


private:
    IPCManager(); // private default constructo (for singleton)


	WSADATA m_wsaData;
    SOCKET m_broadcastSocket;
    SOCKET m_receiveSocket;
    SOCKET m_taskSocket;
    sockaddr_in m_broadcastAddress;
    sockaddr_in m_serverAddress; // to receive sub-task request
    bool m_connectedToServer;
    static IPCManager *m_instance;

};

#endif // IPCMANAGER_H
