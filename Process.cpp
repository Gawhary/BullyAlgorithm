#include "Process.h"
#include "timeHelper.h"

#include <algorithm>

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
    cout << "Initiating Election..." << endl;
        // Broadcast election message
        Messege msg(Messege::Election, m_processId);
        m_ipcManager->broadcastMessage(msg);
    while(true){
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
    Messege msg;
    while(timer.elepsedMiliSec() < COORDINATOR_TIMEOUT){
        // wait for coordinator message
        bool gotSomething = m_ipcManager->readBroadcastMessage(msg, COORDINATOR_TIMEOUT);
        if(!gotSomething)
            break;
        if(msg.messegeType == Messege::Coordinator){
            // check message sender
			if (msg.senderId < m_processId)
                return;// initiate a new Election
            doSubTask(msg);
            timer.reset();
        }
    }
    cout << "Coordinator lost." << endl;
}

void Process::doSubTask(Messege &coordinatorMsg){
    // connects iff not connected
    m_ipcManager->connectToServer(coordinatorMsg.senderAddress);
    if(m_ipcManager->serverConnected()){
        Messege task;
        bool gotTask = m_ipcManager->requestSubTask(task);
        if(gotTask){
            // do the task
            int minValue = INT_MAX;
            int* values = (int*)task.messegeData;
            for(int i = 0; i < task.messegeDataSize; i+= sizeof(int) )
                if(values[i] < minValue)
                    minValue = values[i];
            // return the result
            Messege resultMsg(Messege::TaskResult, m_processId, (void*)minValue, sizeof(minValue));
            m_ipcManager->sendToServer(resultMsg);
        }
    }
}

void Process::beCoordinator()
{
	cout << "Coordinator mode started." << endl;
    // ToDo: clear higher process list and connections
    HANDLE    sendingThread, distributingThread;
    bool *keepAlive = new bool(true);
    // start broadcasting coordinator message
    sendingThread = CreateThread(NULL, 0, Process::keepSendingCoordinatorMessage
                          , (void*) keepAlive, 0, 0);
    distributingThread = CreateThread(NULL, 0, Process::taskDistributionTread
                          , (void*) keepAlive, 0, 0);
    while(true){
        if(checkforBully())
            break;
    }
    // stop broadcasting coordinator message and stop distributing tasks discard current task
    *keepAlive = false;
    WaitForSingleObject(sendingThread, INFINITE);
    CloseHandle(sendingThread);
    WaitForSingleObject(distributingThread, INFINITE);
    CloseHandle(distributingThread);
    delete keepAlive;
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

DWORD WINAPI Process::taskDistributionTread(void* param)
{
    bool* keepAlive = (bool*)param;
    bool taskAccomplished ;
    int numberOfSubTasks = (TASK_ARRAY_SIZE / SUB_TASK_SIZE);
    int array[TASK_ARRAY_SIZE];
    Messege request;
    while(*keepAlive){
        // Create new task
        for(int i = 0; i < TASK_ARRAY_SIZE; i++)
            array[i] = rand();
        taskAccomplished = false;
        int nextStart = 0;
        int accomplishedSubtasks = 0;
        while(*keepAlive && !taskAccomplished){
            // wait for client request
            IPCManager::instance()->recieveRequest(request);
            // when get task request
            if(request.messegeType == Messege::TaskRequest){
                // if still have sub-tasks
                if(nextStart < TASK_ARRAY_SIZE){
                // send next subtask
                    Messege taskMsg(Messege::Task, processID(),(void*)array[nextStart], SUB_TASK_SIZE );
                    // ToDo: send task
                }
                nextStart += SUB_TASK_SIZE;
            }
            // when get task result
            else if(request.messegeType == Messege::TaskResult){
                // update result
                ++ accomplishedSubtasks;
            }
            taskAccomplished = numberOfSubTasks == accomplishedSubtasks;
        }
    }
    return 0;
}

DWORD WINAPI Process::keepSendingCoordinatorMessage(void *param)
{
    bool *keepSending = (bool*)param;
    while(*keepSending){
        Messege msg(Messege::Coordinator, Process::processID());
        IPCManager::instance()->broadcastMessage(msg);
		Sleep(COORDINATOR_MSG_INTERVAL);
    }
    return 0;
}

DWORD WINAPI Process::sendSubTask(vector<int>::iterator start,
                            vector<int>::iterator end)
{
    return 0;
}

Process* Process::m_instance = NULL;
