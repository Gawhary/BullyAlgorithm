#include "messege.h"
#include "timeHelper.h"

#include <sstream>
using namespace std;


bool Messege::fillMessege(std::string msgString){
    stringstream ss(msgString);
    string tmp;
    ss >> tmp;
    parseType(tmp);
    if(messegeType != Unknown){
        ss >> senderId;
    }
    return (messegeType != Unknown);
}

Messege::Messege(Messege::MessegeType type, int pid, string data)
{
    messegeType = type;
    senderId = pid;
    messegeData = data;
}


std::string Messege::msgString(){
    string strType = typeString();
    std::stringstream ss;
    ss << strType << "\t" <<senderId;
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
    default:
        return std::string(MSG_STR_UNKNOWN);
    }
}

void Messege::parseType(std::string strType){

    if      (0 == strcmpi(strType.c_str(),MSG_STR_COORDINATOR)) messegeType = Coordinator;
    else if (0 == strcmpi(strType.c_str(),MSG_STR_ELECTION))    messegeType = Election;
    else if (0 == strcmpi(strType.c_str(),MSG_STR_ALIVE))       messegeType = Alive;
    else                                                        messegeType = Unknown;
}

