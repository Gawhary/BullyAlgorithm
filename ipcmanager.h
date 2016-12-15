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

    bool server_receiveRequest(Messege &request, int timeout = 0); // reads a request sent by slave process
    bool server_respondToRequest(Messege &request, Messege &response); // takes the socket from response and send back it's content
    void server_initTcpServer(); // initialize TCP server
    void server_closeTcpServer();// stop listening for task requests

    bool client_serverConnected(); // used by client/worker
    bool client_sendRequest(_Inout_ Messege &requestResponse, bool waitForResponse = false, int timeout = 0); // send task result and optionally wait for responce
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
