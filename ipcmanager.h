#pragma once

#include "stdafx.h"
#include "messege.h"
#include "configurations.h"

#include <sstream>

// This class is responsable for all communications between all processes
class IPCManager
{
public:
    // Singleton create or return instance
    static IPCManager *instance();
    ~IPCManager();

    void broadcastMessage(Messege &messege); // used for sending Election, Coordinator, and Alive messeges
    bool readBroadcastMessage(Messege &msg, int timeout = 0); // receive messeges broadcast from other processes

    bool server_sendTask(Messege &request, int timeout = 0); // reads a request sent by slave process, send task and recieve result
    bool server_startTcpServer(); // initialize TCP server
    void server_closeTcpServer();// stop listening for task requests

    bool client_getTask( Messege &task, int timeout = 0); // get task from server
    bool client_sendResult(Messege &task); // send task result
    bool client_connectToServer(Messege coordinatorMsg); // called by worker once when enter slave mode
    void client_disconnectServer(); // called by worker before exit slave mode

private:
    IPCManager(); // private default constructo (for singleton)


	WSADATA m_wsaData;
    SOCKET m_broadcastSocket;
    SOCKET m_broadcastReadSocket;
    SOCKET m_clientSocket = INVALID_SOCKET; // used by worker to connect to coordinator
    SOCKET m_serverSocket = INVALID_SOCKET; // used by coordinator to receive task requests
    sockaddr_in m_broadcastAddress;
    static IPCManager *m_instance;

};
