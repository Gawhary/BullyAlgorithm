// BullyProcess.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "process.h"

#pragma comment(lib, "Ws2_32.lib")

int main(int argc, char* argv[])
{
    int id = 0;
    if(argc > 1) // allow the user to pass the process number instead of using the process Id
         id = atoi(argv[1]);
    Process *p = Process::getInstance(id);
    p->start();
    return 0;
}

