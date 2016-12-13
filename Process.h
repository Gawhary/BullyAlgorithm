
#pragma once
#include "stdafx.h"
#include "ipcmanager.h"

#include <vector>

#define TASK_ARRAY_SIZE 1024
#define SUB_TASK_SIZE 128

// The major class implements the Bully algorithm
class Process
{
public:
    // Singleton create or return instance
    static Process *getInstance(int processID = 0);
    void start();

private:
    // creates process with given process number or use Process ID as a process number
    Process(){} // make default constructor private (singleton)
    Process(int processID = 0); // initializes Process with giving number or using ProcessId in the system
	~Process();
    bool electMe(); // initiate election
    void beSlave(); // enter slave mode
    void beCoordinator(); // enter coordinator mode

    bool checkforBully(); // check if higher process has initiated election

    // start task distributing in a new thread
    static DWORD WINAPI keepdistributingTasks(void* param);
    // start new thread to broadcast coordinator message continuesly
    static  DWORD WINAPI keepSendingCoordinatorMessage(void *param);
    // get sub-task from coordinator, do it and return the result
    void doSubTask(Messege &coordinatorMsg);
    // send sub-task and wait for the result in a new thread
    static  DWORD WINAPI sendSubTask(std::vector<int>::iterator start,
                              std::vector<int>::iterator end);
    static int processID(){
        return Process::getInstance()->m_processId;
    }

    int m_processId;
    IPCManager *m_ipcManager;
    static Process *m_instance;

};

