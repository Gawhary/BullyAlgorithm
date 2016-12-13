
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
	- Election interrupts tasks, When a higher process joins while current coordinator distributing a task, task will be discarded and Election will be started.
	
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
	
	

	
	