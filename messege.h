#pragma once

#include "stdafx.h"
#include "configurations.h"

#include <vector>

#define MSG_STR_COORDINATOR     "COORDINATOR"
#define MSG_STR_ELECTION        "ELECTION"
#define MSG_STR_ALIVE           "ALIVE"
#define MSG_STR_UNKNOWN         "UNKNOWN"
#define MSG_STR_TASK_REQUEST    "TASK_REQUEST"
#define MSG_STR_TASK_RESULT     "TASK_RESULT"
#define MSG_STR_TASK            "TASK"

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
    sockaddr_in senderAddress; // recieved sender address
	SOCKET socket = NULL;


    Messege(){}
    ~Messege();
    // Constructs Messege by type, sender id, timeout and data
    Messege(MessegeType type, int pid = 0, void *data = NULL, int dataSize = 0);
    // composes message string
    std::string msgString();
    std::vector<char> msgContent();
    // converts Messege type into string
    std::string typeString();
    bool fillMessege(char *buffer, int size);
    void const *userData(){return (const void*) m_userData;}
    int userDataSize() {return m_userDataSize;}

private:
    void parseType(std::string strType);
    void* m_userData = NULL;
    int m_userDataSize = 0;
};
