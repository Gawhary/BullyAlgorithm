#ifndef MESSEGE_H
#define MESSEGE_H
#include "stdafx.h"

#include <vector>

#define MSG_MAX_SIZE 1024
#define MSG_STR_COORDINATOR "COORDINATOR"
#define MSG_STR_ELECTION    "ELECTION"
#define MSG_STR_ALIVE       "ALIVE"
#define MSG_STR_UNKNOWN     "UNKNOWN"
#define MSG_STR_TASK_REQUEST "TASK_REQUEST"
#define MSG_STR_TASK_RESULT  "TASK_RESULT"
#define MSG_STR_TASK        "TASK"

#define ELECTION_TIMEOUT    1000
#define COORDINATOR_TIMEOUT 3000
#define COORDINATOR_MSG_INTERVAL 1000 
#define SERVER_TIMEOUT 100

class Messege
{
public:

    enum MessegeType{
        Unknown = -1,
        Coordinator,
        Election,
        Alive,
        TaskRequest,
        TaskResult,
        Task
    };
    int senderId = 0;
    MessegeType messegeType = Unknown;
    void* messegeData = NULL;
    int messegeDataSize = 0;
    sockaddr_in senderAddress; // recieved sender address

    Messege(){}
    ~Messege();
    // Constructs Messege by type, sender id, timeout and data
    Messege(MessegeType type, int pid, void *data = NULL, int dataSize = 0);
    // composes message string
    std::string msgString();
    // converts Messege type into string
    std::string typeString();

    bool fillMessege(char *buffer, int size);
private:
    void parseType(std::string strType);
};


#endif // MESSEGE_H
