
#pragma once
#include "stdafx.h"
#include "ipcmanager.h"

#include <vector>

// The major class implements the Bully algorithm
class Process
{
public:
    // Singleton create or return instance
    static Process *getInstance(int processNumber = 0);
    void start();

private:
    // creates process with given process number or use Process ID as a process number
    Process(){} // make default constructor private (singleton)
    Process(int processNumber = 0); // initializes Process with giving number or using ProcessId in the system
	~Process();
    bool electMe(); // initiate election
    void beSlave(); // enter slave mode
    void doSubTask();// get sub-task from coordinator, do it and return the result
    void beCoordinator(); // enter coordinator mode

    bool checkforBully(); // check if higher process has initiated election

    void startNewTask(); // start task distributing
    // start new thread to broadcast coordinator message continuesly
    static  DWORD WINAPI keepSendingCoordinatorMessage(void *param);
    // send sub-task and wait for the result in a new thread
    static void handleSubTask(std::vector<int>::iterator start,
                              std::vector<int>::iterator end);
    static int processNumber(){
        return Process::getInstance()->m_processNumber;
    }

    int m_processNumber;
    IPCManager *m_ipcManager;
    static Process *m_instance;

};

