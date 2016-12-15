#include "BullyProcess.h"
#include "timeHelper.h"

#include <algorithm>

using namespace std;

BullyProcess *BullyProcess::getInstance(int processNumber){
    if(m_instance)
        return m_instance;
    m_instance = new BullyProcess(processNumber);
    return m_instance;
}

void BullyProcess::start()
{
    cout << "Starting process: " << m_processId << endl;
    while(true){
        if(electMe()) // returns true if no higher processes
            beCoordinator(); // will return when higher process joins
        beWorker(); // will return when coordinator dies
    }
}

BullyProcess::BullyProcess(int processNumber)
{
    if(processNumber == 0)
        // use system's process ID;
        m_processId = (int) GetCurrentProcessId();
    else
        m_processId = processNumber;
    m_ipcManager = IPCManager::instance();
}

BullyProcess::~BullyProcess()
{
}
bool BullyProcess::electMe()
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

void BullyProcess::beWorker()
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
    m_ipcManager->client_disconnectServer();
}

void BullyProcess::doSubTask(Messege &coordinatorMsg){
    // connect iff not connected
    if(m_ipcManager->client_connectToServer(coordinatorMsg)){
        Messege task(Messege::TaskRequest);
        bool gotTask = m_ipcManager->client_sendRequest(task, true, SERVER_TIMEOUT);
        if(gotTask){
            cout << Time::timeStamp() << ": Task recieved." << endl;
            // do the task
            int minValue = INT_MAX;
            int* values = (int*)task.userData();
            for(int i = 0; i < task.userDataSize(); i+= sizeof(int) )
                if(values[i] < minValue)
                    minValue = values[i];
            // return the result
            cout << Time::timeStamp() << ": Sending result:" << minValue << endl;
            Messege resultMsg(Messege::TaskResult, m_processId, (void*)minValue, sizeof(minValue));
            m_ipcManager->client_sendRequest(resultMsg);
        }
    }
}

int BullyProcess::processID(){
    return BullyProcess::getInstance()->m_processId;
}

void BullyProcess::beCoordinator()
{
    cout << "Coordinator mode started." << endl;
    // ToDo: clear higher process list and connections
    HANDLE    sendingThread, distributingThread;
    bool *keepAlive = new bool(true);
    m_ipcManager->server_startTcpServer(); // start listening for work requests
    // start broadcasting coordinator message
    distributingThread = CreateThread(NULL, 0, BullyProcess::taskDistributionTread
                          , (void*) keepAlive, 0, 0);
    sendingThread = CreateThread(NULL, 0, BullyProcess::keepSendingCoordinatorMessage
                          , (void*) keepAlive, 0, 0);
    while(true){
        if(gotBully()) // check if some higher process initiated an Election
            break;
    }
    // stop broadcasting coordinator message and
    // stop distributing tasks (discard current task)
    *keepAlive = false; // inform threads to stop
    // wait for threads to exit
    WaitForSingleObject(sendingThread, INFINITE);
    WaitForSingleObject(distributingThread, INFINITE);
    // clean up
    CloseHandle(sendingThread);
    CloseHandle(distributingThread);
    // stop listening to worker requests
    m_ipcManager->server_closeTcpServer();
    delete keepAlive;
}


bool BullyProcess::gotBully()
{
    while(true){
        // wait for election message, don't wait for long
        Messege msg;
        bool somethingReceived = m_ipcManager->readBroadcastMessage(msg, ELECTION_TIMEOUT);
        if(!somethingReceived) //timeout
            return false;
        if(msg.messegeType == Messege::Election ||
			msg.messegeType == Messege::Coordinator){
            if( msg.senderId > m_processId){
                // toDo: save new higher process and create a connection
                return true;
            }
            else{
                // ToDo: send alive message only to the sender
                m_ipcManager->broadcastMessage(Messege(Messege::Alive,m_processId));
            }
        }
    }
}

DWORD WINAPI BullyProcess::taskDistributionTread(void* param)
{
    bool* keepAlive = (bool*)param;
    bool taskAccomplished ;
    int numberOfSubTasks = (TASK_ARRAY_SIZE / SUB_TASK_SIZE);
    int array[TASK_ARRAY_SIZE];
    Messege request;
    while(*keepAlive){
        // Create new task
        cout << Time::timeStamp() << ": Creating new task..." << endl;
        int minimumValue = INT_MAX;
        for(int i = 0; i < TASK_ARRAY_SIZE; i++)
            array[i] = rand();
        taskAccomplished = false;
        int nextStart = 0;
        int accomplishedSubtasks = 0;
        while(*keepAlive && !taskAccomplished){
            // wait for worker request
            if(!IPCManager::instance()->server_receiveRequest(request)){
                Sleep(SERVER_TIMEOUT);
                continue;
            }
            // when get task request
            if(request.messegeType == Messege::TaskRequest){
                // if still have sub-tasks
                if(nextStart < TASK_ARRAY_SIZE){
                // send next subtask
                    cout << Time::timeStamp() << ": Sending sub-task to process " << request.senderId << endl;
                    Messege taskMsg(Messege::Task, processID(),(void*)&array[nextStart], SUB_TASK_SIZE * sizeof(int));
                    IPCManager::instance()->server_respondToRequest(request, taskMsg);
                }
                nextStart += SUB_TASK_SIZE;
            }
            // when get task result
            else if(request.messegeType == Messege::TaskResult){
                // update result
                int result = (int)request.userData();
                if(result < minimumValue)
                    minimumValue = result;
                ++ accomplishedSubtasks;
            }
            if(numberOfSubTasks == accomplishedSubtasks){
                taskAccomplished = true;
                cout << Time::timeStamp() << ": Task Accomplished, Result: " << minimumValue << endl;
            }
        }
    }
    return 0;
}

DWORD WINAPI BullyProcess::keepSendingCoordinatorMessage(void *param)
{
    bool *keepSending = (bool*)param;
    while(*keepSending){
        Messege msg(Messege::Coordinator, BullyProcess::processID());
        IPCManager::instance()->broadcastMessage(msg);
		Sleep(COORDINATOR_MSG_INTERVAL);
    }
    return 0;
}

BullyProcess* BullyProcess::m_instance = NULL;
