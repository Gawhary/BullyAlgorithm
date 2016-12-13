#### The Bully Algorithm

*************** Selected IPC Technology ***************

	@IPC Technologies supported by Windows:
		- Clipboard: not sutiable 
		- COM: Managed (doesn't fit the requirement)
		- DDE: not recommended (not very efficien)
		- RPC: not sutiable
		- File Mapping: Only on the same machine(local communications)
		- Data Copy
		- Mailslot: one way communications
		- Named pipes: Only two process communications
	-->	- Windows Sockets 2

Windows Sockets 2 (WinSocks) has chosen because it fulfill all requirements.


******************** Algorithm ********************

	@election:
	  - loop
			- Broadcast Election Message and wait for responce
			- if got Alive Message from a higher process then
				go to slave mode
			- else (timeout) then
				go to coordinator 
				
	@coordinator:
		- loop
			- broadcast Coordinator message
			- check for election message
			- if got election message from higher process
				- go to slave mode
			- start task distribution			
			
	@slave: 
		- Loop 
			- wait for coordinator message
			- if timeout
				- start election
			- request sub-task, do it and return the result
		 
	 
* Assumptions:
	- Election interrupts tasks distributing, When a higher process joins while current coordinator distributing a task, task will be discarded and Election will be started, this behaviour has chosen to avoid having more than one coordinator at any time.
	- For simplicity, Election, Coordinator and Alive message will be broadcasted to all processes instead of being sent to individual targets (processes)
	- connections between Coordinator and workers will be created once a new coordinator has elected for task related communications
	- Coordinator will divide the task into fixed size sub-tasks, and assign them one by one whenever get a requests from worker process, this will allow faster workers to do more sub-tasks than slower ones.

	
******************** BUILD AND RUN ********************
# Prerequisites 
	* Microsoft Visual studio (2015 Community version used)
# Build
	- Open the solution file "BullyProcess.sln" in Visual studio 
	- build

# RUN	
	- launch the executable file once for each required process
	- for more convenient, process number can be passed as a command line argument to be used instead of process Id, in this case the user will responceble for keeping process numbers unique.
	- Process can be terminated just by closing its console window
	
	

	
	