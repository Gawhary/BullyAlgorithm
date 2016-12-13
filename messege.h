#ifndef MESSEGE_H
#define MESSEGE_H
#include "stdafx.h"

#define MSG_MAX_SIZE 1024
#define MSG_STR_COORDINATOR "COORDINATOR"
#define MSG_STR_ELECTION    "ELECTION"
#define MSG_STR_ALIVE       "ALIVE"
#define MSG_STR_UNKNOWN     "UNKNOWN"

#define ELECTION_TIMEOUT    1000
#define COORDINATOR_TIMEOUT 3000
#define COORDINATOR_MSG_INTERVAL 1000 

class Messege
{
public:

    enum MessegeType{
        Unknown = -1,
        Coordinator,
        Election,
        Alive
    };
    int senderId;
    MessegeType messegeType;
    std::string messegeData;
    sockaddr_in senderAddress; // recieved sender address

    Messege(){}
    // Constructs Messege by type, sender id, timeout and data
    Messege(MessegeType type, int pid, std::string data = std::string());
    // composes message string
    std::string msgString();
    // converts Messege type into string
    std::string typeString();

    bool fillMessege(std::string msgString);
private:
    void parseType(std::string strType);
};


#endif // MESSEGE_H
