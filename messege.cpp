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
    if(messegeType == Unknown)
            return false;
    ss >> senderId;
	int pos = (int)ss.tellg();
	if (pos <= 0)
		return true;
    char *data = buffer + pos;
    int dataSize = size - pos;
	if (m_userDataSize > sizeof(m_userData)) {
		free(m_userData);
		m_userData = NULL;
	}
    m_userDataSize = dataSize; // set new data size
    if(dataSize > sizeof(m_userData)){ // iff data cann't fit in the pointer itself, allocate memory for it
        m_userData = malloc(dataSize);
        memcpy(m_userData, data, dataSize);
    }
    else if (dataSize > 0){ // data can fit in pointer itself
        m_userData = 0;
        memcpy(&m_userData, data, dataSize);
    }
    return true;
}

Messege::~Messege()
{
    if(m_userDataSize > sizeof(m_userData))
        free(m_userData);
}

Messege::Messege(Messege::MessegeType type, int pid, void *data, int dataSize)
{
    messegeType = type;
    senderId = pid;
    m_userDataSize = dataSize;
    if(dataSize > sizeof(m_userData)){
        m_userData = malloc(dataSize);
        memcpy(m_userData, data, dataSize);
    }
    else if (dataSize > 0){ // data can fit in messegeData itself
        m_userData = 0;
        memcpy(&m_userData, data, dataSize);
    }
}


std::string Messege::msgString(){
    string strType = typeString();
    std::stringstream ss;
    ss << strType << '\t' <<senderId;
    return ss.str();
}

vector<char> Messege::msgContent()
{
    string str = msgString();
    int size = str.size() + 1 + m_userDataSize;// adding 1 for null terminated string
    vector<char> content(size);
    memcpy(content.data(), str.c_str(), str.size()+1);
    memcpy(content.data()+str.size() + 1, m_userData, m_userDataSize);
    return content;
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

    if      (0 == _strcmpi(strType.c_str(),MSG_STR_COORDINATOR)) messegeType = Coordinator;
    else if (0 == _strcmpi(strType.c_str(),MSG_STR_ELECTION))    messegeType = Election;
    else if (0 == _strcmpi(strType.c_str(),MSG_STR_ALIVE))       messegeType = Alive;
    else if (0 == _strcmpi(strType.c_str(),MSG_STR_TASK_REQUEST))messegeType = TaskRequest;
    else if (0 == _strcmpi(strType.c_str(),MSG_STR_TASK))        messegeType = Task;
    else if (0 == _strcmpi(strType.c_str(),MSG_STR_TASK_RESULT)) messegeType = TaskResult;
    else                                                        messegeType = Unknown;
}

