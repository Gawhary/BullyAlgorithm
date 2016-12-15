#pragma once

#include "stdafx.h"
#include "ipcmanager.h"
#include "configurations.h"

#include <vector>

// The major class implements the Bully algorithm
class BullyProcess
{
public:
    // Singleton create or return instance
    static BullyProcess *getInstance(int processID = 0);
    void start();

private:
    // creates process with given process number or use Process ID as a process number
    BullyProcess(){} // make default constructor private (singleton)
    BullyProcess(int processID = 0); // initializes Process with giving number or using ProcessId in the system
    ~BullyProcess();
    bool electMe(); // initiate election
    void beWorker(); // enter slave mode
    void beCoordinator(); // enter coordinator mode

    bool gotBully(); // check if higher process has initiated election

    // start task distributing in a new thread
    static DWORD WINAPI taskDistributionTread(void* param);
    // start new thread to broadcast coordinator message continuesly
    static  DWORD WINAPI keepSendingCoordinatorMessage(void *param);
    // get sub-task from coordinator, do it and return the result
    void doSubTask(Messege &coordinatorMsg);
    static int processID();

    int m_processId;
    IPCManager *m_ipcManager;
    static BullyProcess *m_instance;

};

