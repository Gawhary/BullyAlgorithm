#include "messege.h"
#include "timeHelper.h"

#include <stdlib.h>
#include <sstream>
using namespace std;


bool Messege::fillMessege(char* buffer, int size){
    stringstream ss(buffer);
    string str;
    ss >> str;
    parseType(str);
    if(messegeType != Unknown){
        ss >> senderId;
    }
    char *data = buffer + ss.tellg();
    int dataSize = size - ss.tellg();
    if(messegeDataSize > sizeof(messegeData))
        free(messegeData);
    messegeDataSize = dataSize; // set new data size
    if(dataSize > sizeof(messegeData)){
        messegeData = malloc(dataSize);
        memcpy(messegeData, data, dataSize);
    }
    else if (dataSize > 0){ // data can fit in messegeData itself
        messegeData = 0;
        memcpy(&messegeData, data, dataSize);
    }
    return (messegeType != Unknown);
}

Messege::~Messege()
{
    if(messegeDataSize > sizeof(messegeData))
        free(messegeData);
}

Messege::Messege(Messege::MessegeType type, int pid, void *data, int dataSize)
{
    messegeType = type;
    senderId = pid;
    messegeDataSize = dataSize;
    if(dataSize > sizeof(messegeData)){
        messegeData = malloc(dataSize);
        memcpy(messegeData, data, dataSize);
    }
    else if (dataSize > 0){ // data can fit in messegeData itself
        messegeData = 0;
        memcpy(&messegeData, data, dataSize);
    }
}


std::string Messege::msgString(){
    string strType = typeString();
    std::stringstream ss;
    ss << strType << '\t' <<senderId;
    return ss.str();
}

std::string Messege::typeString(){
    switch(messegeType){
    case Coordinator:
        return std::string(MSG_STR_COORDINATOR);
    case  Election:
        return std::string(MSG_STR_ELECTION);
    case  Alive:
        return std::string(MSG_STR_ALIVE);
    case  TaskRequest:
        return std::string(MSG_STR_TASK_REQUEST);
    case  TaskResult:
        return std::string(MSG_STR_TASK_RESULT);
    case  Task:
        return std::string(MSG_STR_TASK);
    default:
        return std::string(MSG_STR_UNKNOWN);
    }
}

void Messege::parseType(std::string strType){

    if      (0 == strcmpi(strType.c_str(),MSG_STR_COORDINATOR)) messegeType = Coordinator;
    else if (0 == strcmpi(strType.c_str(),MSG_STR_ELECTION))    messegeType = Election;
    else if (0 == strcmpi(strType.c_str(),MSG_STR_ALIVE))       messegeType = Alive;
    else if (0 == strcmpi(strType.c_str(),MSG_STR_TASK_REQUEST))messegeType = TaskRequest;
    else if (0 == strcmpi(strType.c_str(),MSG_STR_TASK))        messegeType = Task;
    else if (0 == strcmpi(strType.c_str(),MSG_STR_TASK_RESULT)) messegeType = TaskResult;
    else                                                        messegeType = Unknown;
}

