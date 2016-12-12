#include "ipcmanager.h"
using namespace std;
IPCManager *IPCManager::instance(){
    if(m_instance)
        return m_instance;
    m_instance = new IPCManager();
    return m_instance;
}

IPCManager::~IPCManager()
{
    closesocket(m_broadcastSocket);
    closesocket(m_receiveSocket);
    WSACleanup();
}

IPCManager::IPCManager()
{
    // initialize broadcast address
    m_broadcastAddress.sin_family = AF_INET;
    m_broadcastAddress.sin_addr.s_addr = INADDR_BROADCAST;
    m_broadcastAddress.sin_port = htons( (unsigned short) PORT );

    // create broadcast socket
    if((m_broadcastSocket = socket(AF_INET , SOCK_DGRAM , IPPROTO_UDP )) == INVALID_SOCKET)
    {
        printf("Could not create broadcast socket : %d" , WSAGetLastError());
        exit(EXIT_FAILURE);
    }
    printf("Socket created.\n");
    int broadcastValue = 1;
    if(setsockopt(m_broadcastSocket, SOL_SOCKET, SO_BROADCAST, (char*)&broadcastValue, sizeof(broadcastValue)) == SOCKET_ERROR){
        cout<< "could not set socket option" << endl;;
        exit(EXIT_FAILURE);
    }

    // create receive socket
    if((m_receiveSocket = socket(AF_INET , SOCK_DGRAM , IPPROTO_UDP )) == INVALID_SOCKET)
    {
        printf("Could not create receive socket : %d" , WSAGetLastError());
        exit(EXIT_FAILURE);
    }
    broadcastValue = 1;
    if(setsockopt(m_receiveSocket, SOL_SOCKET, SO_REUSEADDR, (char*)&broadcastValue, sizeof(broadcastValue)) == SOCKET_ERROR){
        cout<< "could not set socket option";
        exit(EXIT_FAILURE);
    }

    // set recieve address
    sockaddr_in RecvAddr;
    RecvAddr.sin_family = AF_INET;
    RecvAddr.sin_port = htons(PORT);
    RecvAddr.sin_addr.s_addr = htonl(INADDR_ANY);

    // bind recieve address with recieve socket
    if (bind(m_receiveSocket, (SOCKADDR *) & RecvAddr, sizeof (RecvAddr)) != 0) {
        wprintf(L"bind failed with error %d\n", WSAGetLastError());
        exit(EXIT_FAILURE);
    }
}

void IPCManager::broadcastMessage(Messege &messege)
{
    cout << "Broadcasting message: ";
    messege.print();
	string msg = messege.msgString();
    if (sendto(m_broadcastSocket, msg.c_str(), msg.length(), 0, 
		(struct sockaddr*) &m_broadcastAddress, sizeof(m_broadcastAddress)) == SOCKET_ERROR)
    {
        printf("sendto() failed with error code : %d" , WSAGetLastError());
        exit(EXIT_FAILURE);
    }

}

bool IPCManager::readBroadcastMessage(Messege &msg, int timeout)
{
    int res = 0;
    char RecvBuf[MSG_MAX_SIZE];
    memset(RecvBuf, 0, MSG_MAX_SIZE);
    int SenderAddrSize = sizeof (msg.senderAddress);

    // set timeout
    if(setsockopt(m_receiveSocket, SOL_SOCKET, SO_RCVTIMEO,(char*)&timeout,sizeof(timeout)))
        cout << "could not set socket option!" <<endl;

    res = recvfrom(m_receiveSocket, RecvBuf, MSG_MAX_SIZE, 0,
                   (SOCKADDR *) & msg.senderAddress, &SenderAddrSize);
    if(WSAGetLastError() == WSAETIMEDOUT){
        return false;
    }
    if (res == SOCKET_ERROR) {
        wprintf(L"recvfrom failed with error %d\n", WSAGetLastError());
        exit(EXIT_FAILURE);
    }
	cout << "Broadcast messege received: " << RecvBuf << endl;
    msg.fillMessege(RecvBuf);
    return true;
}
IPCManager* IPCManager::m_instance = NULL;
