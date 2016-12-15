#include "ipcmanager.h"
#include "timeHelper.h"
using namespace std;
IPCManager *IPCManager::instance()
{
    if(m_instance)
        return m_instance;
    m_instance = new IPCManager();
    return m_instance;
}

IPCManager::~IPCManager()
{
    WSACleanup();
}

IPCManager::IPCManager(){
    // initiates use of the Winsock DLL
    if (WSAStartup(MAKEWORD(2, 2), &m_wsaData) != 0) {
        printf("WSAStartup failed: %d\n", WSAGetLastError());
        WSACleanup();
        exit(EXIT_FAILURE);
    }

    // initialize broadcast address
    m_broadcastAddress.sin_family = AF_INET;
    m_broadcastAddress.sin_addr.s_addr = INADDR_BROADCAST;
    m_broadcastAddress.sin_port = htons( (unsigned short) UDP_PORT );

    // create broadcast socket
    if((m_broadcastSocket = socket(AF_INET , SOCK_DGRAM , IPPROTO_UDP )) == INVALID_SOCKET)
    {
        printf("Could not create broadcast socket : %d" , WSAGetLastError());
        WSACleanup();
        exit(EXIT_FAILURE);
    }
    printf("Socket created.\n");
    int broadcastValue = 1;
    if(setsockopt(m_broadcastSocket, SOL_SOCKET, SO_BROADCAST, (char*)&broadcastValue, sizeof(broadcastValue)) == SOCKET_ERROR){
        cout<< "could not set socket option" << endl;;
        WSACleanup();
        exit(EXIT_FAILURE);
    }

    // create receive socket
    if((m_broadcastReadSocket = socket(AF_INET , SOCK_DGRAM , IPPROTO_UDP )) == INVALID_SOCKET)
    {
        printf("Could not create receive socket : %d" , WSAGetLastError());
        WSACleanup();
        exit(EXIT_FAILURE);
    }
    broadcastValue = 1;
    if(setsockopt(m_broadcastReadSocket, SOL_SOCKET, SO_REUSEADDR, (char*)&broadcastValue, sizeof(broadcastValue)) == SOCKET_ERROR){
        cout<< "could not set socket option" <<endl;
        WSACleanup();
        exit(EXIT_FAILURE);
    }

    // set recieve address
    sockaddr_in RecvAddr;
    RecvAddr.sin_family = AF_INET;
    RecvAddr.sin_port = htons(UDP_PORT);
    RecvAddr.sin_addr.s_addr = htonl(INADDR_ANY);

    // bind recieve address with recieve socket
    if (bind(m_broadcastReadSocket, (SOCKADDR *) & RecvAddr, sizeof (RecvAddr)) != 0) {
        wprintf(L"bind failed with error %d\n", WSAGetLastError());
        WSACleanup();
        exit(EXIT_FAILURE);
    }
}

void IPCManager::broadcastMessage(Messege &messege)
{
    cout << Time::timeStamp() <<  ": Broadcasting message: " << messege.msgString() << endl;
    vector<char> msg = messege.msgContent();
    if (sendto(m_broadcastSocket, msg.data(), msg.size(), 0,
               (struct sockaddr*) &m_broadcastAddress, sizeof(m_broadcastAddress)) == SOCKET_ERROR)
    {
        printf("sendto() failed with error code : %d" , WSAGetLastError());
        WSACleanup();
        exit(EXIT_FAILURE);
    }

}

bool IPCManager::readBroadcastMessage(Messege &msg, int timeout)
{
    int res = 0;
    char RecvBuf[MSG_MAX_SIZE];
    memset(RecvBuf, 0, MSG_MAX_SIZE);
    int fromlen = sizeof(msg.senderAddress);

    // set timeout
    if(setsockopt(m_broadcastReadSocket, SOL_SOCKET, SO_RCVTIMEO,(char*)&timeout,sizeof(timeout))){
        cout << "could not set socket option!" <<endl;
        return false;
    }

    res = recvfrom(m_broadcastReadSocket, RecvBuf, MSG_MAX_SIZE, 0,
                   (SOCKADDR *) & msg.senderAddress, &fromlen);
    if(res <= 0){
        return false;
    }
    msg.fillMessege(RecvBuf, res);
    return true;
}

bool IPCManager::client_connectToServer(Messege coordinatorMsg)
{
	if (m_clientSocket != INVALID_SOCKET) { // check if already connected
		return true;
	}
	std::cout << Time::timeStamp() << ": Connecting to server..." << endl;
	int trials = 2;
	bool error;
	do {
		error = false;
		// Create a SOCKET for connecting to server
		m_clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (m_clientSocket == INVALID_SOCKET) {
			printf("socket failed with error: %ld\n", WSAGetLastError());
			WSACleanup();
			exit(EXIT_FAILURE);
		}
		// extract server IP address from coordination messege

		sockaddr_in address;
		address.sin_family = AF_INET;
		address.sin_port = htons(TCP_PORT);
		address.sin_addr.s_addr = coordinatorMsg.senderAddress.sin_addr.s_addr;

		// establishes a connection to the server/coordinator
		int iResult;
		iResult = connect(m_clientSocket, (SOCKADDR*)&address,
			sizeof(address));

		if (iResult == SOCKET_ERROR) {
			error = true;
			break;
		}
		if (error) {
			client_disconnectServer();
			-- trials;
		}
	} while (error && trials > 0);
	if (error)
		return false;
	return true;
}

void IPCManager::client_disconnectServer()
{
    if(m_clientSocket == INVALID_SOCKET)
        return;
    std::cout << Time::timeStamp() <<  ": Disconnecting from server..." << endl;
    // shutdown the connection
    int iResult = shutdown(m_clientSocket, SD_BOTH);
    if (iResult == SOCKET_ERROR) {
        printf("shutdown connection failed with error: %d\n", WSAGetLastError());
    }
    closesocket(m_clientSocket);
    m_clientSocket = INVALID_SOCKET;
}

bool IPCManager::server_receiveRequest(Messege &request, int timeout)
{
    if(m_serverSocket == INVALID_SOCKET){
		return false;
    }
    // create socket
    SOCKET clientSocket;
    clientSocket = accept(m_serverSocket, NULL, NULL);

    if(clientSocket == INVALID_SOCKET){
        return false;
    }

    // set timeout
    if(setsockopt(clientSocket, SOL_SOCKET, SO_RCVTIMEO,(char*)&timeout,sizeof(timeout))){
        cout << "could not set socket option!" <<endl;
        return false;
    }

    // recieve request
    char recvbuf[MSG_MAX_SIZE];
    int res = recv(clientSocket, recvbuf, MSG_MAX_SIZE, 0);
    if (res <= 0) {
        //closesocket(clientSocket);
        return false;
    }
    request.fillMessege(recvbuf, res);
    request.socket = clientSocket;
    return true;
}

bool IPCManager::server_startTcpServer(){
	std::cout << Time::timeStamp() << ": Starting TCP server..." << endl;
    bool error;
    int trials = 2;
    do {
        error = false;
        if (m_serverSocket != INVALID_SOCKET){
            error =  true;
            break;
        }
        // Create a SOCKET for connecting to server
        m_serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (m_serverSocket == INVALID_SOCKET) {
            printf("socket failed with error: %ld\n", WSAGetLastError());
            WSACleanup();
            exit(EXIT_FAILURE);

        }

        sockaddr_in serverAddress;
        serverAddress.sin_family = AF_INET;
        serverAddress.sin_port = htons(TCP_PORT);
        serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);

        // Setup the TCP listening socket
        int iResult = bind(m_serverSocket, (SOCKADDR *)&serverAddress, sizeof(serverAddress));
        if (iResult == SOCKET_ERROR) {
            printf("bind failed with error: %d\n", WSAGetLastError());
            error = true;
            break;
        }
        //places a socket in a state in which it is listening for an incoming connection.
        iResult = listen(m_serverSocket, SOMAXCONN);
        if (iResult == SOCKET_ERROR) {
            printf("listen failed with error: %d\n", WSAGetLastError());
            error = true;
            break;
        }
        // mark the listening socket as non-blocking
        u_long iMode = 1;
        iResult = ioctlsocket(m_serverSocket, FIONBIO, &iMode);
        if (iResult != NO_ERROR) {
            printf("ioctlsocket failed with error: %ld\n", iResult);
            error = true;
            break;
        }

        if(error){
            -- trials;
            server_closeTcpServer();
            Sleep(RETRY_TIMEOUT);
        }
    } while (error && trials > 0);
    if(error)
        return false;
    // else
    return true;
}

void IPCManager::server_closeTcpServer()
{
    cout << Time::timeStamp() << " : Shutting down server..." << endl;
    // shutdown the connection
    int iResult = shutdown(m_serverSocket, SD_BOTH);
    if (iResult == SOCKET_ERROR) {
        printf("shutdown connection failed with error: %d\n", WSAGetLastError());
    }
    closesocket(m_serverSocket);
    m_serverSocket = INVALID_SOCKET;
}

bool IPCManager::server_respondToRequest(Messege &request, Messege &response)
{
    if(request.socket == INVALID_SOCKET)
        return false;
    int iResult = send(request.socket, response.msgContent().data(), response.msgContent().size(), 0);
    //closesocket(request.socket);
    request.socket = INVALID_SOCKET;
    if (iResult <= 0) {
        printf("send failed with error: %d\n", WSAGetLastError());
        return false;
    }
    return true;
}

bool IPCManager::client_sendRequest(Messege &requestResponse, bool waitForResponse, int timeout)
{
    if(m_clientSocket == INVALID_SOCKET)
        return false;
    int res = send( m_clientSocket, requestResponse.msgContent().data(), requestResponse.msgContent().size(), 0 );
    if (res == SOCKET_ERROR) {
        printf("send failed with error: %d\n", WSAGetLastError());
        return false;
    }
    if(waitForResponse){

        // set timeout
        if(setsockopt(m_clientSocket, SOL_SOCKET, SO_RCVTIMEO,(char*)&timeout,sizeof(timeout))){
            cout << "could not set socket option!" <<endl;
            return false;
        }

        char recvbuf[MSG_MAX_SIZE];
        res = recv(m_clientSocket, recvbuf, MSG_MAX_SIZE, 0);
        if ( res <= 0){
            return false;
        }
        requestResponse.fillMessege(recvbuf, res);
    }
    return true;
}

IPCManager* IPCManager::m_instance = NULL;
