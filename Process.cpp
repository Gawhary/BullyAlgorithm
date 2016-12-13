#include "Process.h"

#include "timeHelper.h"

using namespace std;

Process *Process::getInstance(int processNumber){
    if(m_instance)
        return m_instance;
    m_instance = new Process(processNumber);
    return m_instance;
}

void Process::start()
{
    cout << "Starting process: " << m_processId << endl;
    while(true){
        if(electMe()) // returns true if no higher processes
            beCoordinator(); // will return when higher process joins
        beSlave(); // will return when coordinator dies
    }
}

Process::Process(int processNumber)
{
    if(processNumber == 0)
        // use process ID;
        m_processId = (int) GetCurrentProcessId();
    else
        m_processId = processNumber;
    m_ipcManager = IPCManager::instance();
}

Process::~Process()
{
}
bool Process::electMe()
{
    cout << "Initiating Election..." << endl;;
    while(true){
        // Broadcast election message
        Messege msg(Messege::Election, m_processId);
        m_ipcManager->broadcastMessage(msg);
        // wait for responce or timeout
		bool somethingReceived;
		do {
			somethingReceived = m_ipcManager->readBroadcastMessage(msg, ELECTION_TIMEOUT);
			if (!somethingReceived)
				return true;
		} while (msg.senderId == m_processId); // skip message broadcasted by me
        if((msg.messegeType == Messege::Coordinator ||
            msg.messegeType == Messege::Election)
                && msg.senderId > m_processId){
            // toDo: save new higher process and create connection
            return false;
        }
    }
}

void Process::beSlave()
{
    cout << "Slave mode started." << endl;
    Time timer;
    while(timer.elepsedMiliSec() < COORDINATOR_TIMEOUT){
        // wait for coordinator message
        Messege msg;
        bool gotSomething = m_ipcManager->readBroadcastMessage(msg, COORDINATOR_TIMEOUT);
        if(!gotSomething)
            break;
        if(msg.messegeType == Messege::Coordinator){
            // ToDo: check message sender
            timer.reset();
        }
        // ToDo:
        //if(msg.messegeType == Messege::Election)
            //if(msg.senderAddress > m_processNumber)
                // add new process to higher list and connect
    }
	cout << "Coordinator lost." << endl;;
}
void Process::doSubTask(){
    // ToDo: get sub-task from coordinator, do it and return the result
}

void Process::beCoordinator()
{
	cout << "Coordinator mode started." << endl;
    // ToDo: clear higher process list and connections
    HANDLE    thread;
    bool *keepSending = new bool(true);
    // start broadcasting coordinator message
    thread = CreateThread(NULL, 0, Process::keepSendingCoordinatorMessage
                          , (void*) keepSending, 0, 0);
    while(true){
        if(checkforBully())
            break;
        startNewTask();
    }
    // stop broadcasting coordinator message
    *keepSending = false;
    WaitForSingleObject(thread, INFINITE);
    CloseHandle(thread);
}


bool Process::checkforBully()
{
    while(true){
        // wait for election message, don't wait for long
        Messege msg;
        bool somethingReceived = m_ipcManager->readBroadcastMessage(msg, ELECTION_TIMEOUT);
        if(!somethingReceived) //timeout
            return false;
        if(msg.messegeType == Messege::Election){
            if( msg.senderId > m_processId){
                // toDo: save new higher process and create connection
                return true;
            }
            else{
                // ToDo: send alive message only to the sender
                m_ipcManager->broadcastMessage(Messege(Messege::Alive,m_processId));
            }
        }
    }
}

void Process::startNewTask()
{
    // ToDo: implement
}

DWORD WINAPI Process::keepSendingCoordinatorMessage(void *param)
{
    bool *keepSending = (bool*)param;
    while(*keepSending){
        Messege msg(Messege::Coordinator, Process::processNumber());
        IPCManager::instance()->broadcastMessage(msg);
		Sleep(COORDINATOR_MSG_INTERVAL);
    }
    return 0;
}

void Process::handleSubTask(vector<int>::iterator start,
                            vector<int>::iterator end)
{

}

Process* Process::m_instance = NULL;
