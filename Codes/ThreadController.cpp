#include "ThreadController.h"


ThreadController::ThreadController(unsigned int id):
	isTerminated(false), hasJob(false), isReallyTerminated(false), id(id)
{
	// Register the thread
	this->m_thread = std::thread( [this] { 
		this->runLoopControl(); } );
}

ThreadController::~ThreadController(void)
{
	// Check if the thread really terminated
	// UNFINISHED
}

void ThreadController::start()
{
	// Detach!
	this->m_thread.detach();
}

void ThreadController::runLoopControl()
{
	// Use isTerminated to check if needed to terminate
	while ( !isTerminated ) {

		// Use hasJob to check if needed to do a job
		if ( hasJob ) {
			// Do something
			std::cout << "DO SOMETHING!! Thread: " << this->id << std::endl;
			cvShowImage( "result", img );

			hasJob = false;
		}

		// Use this to wait for 200 ms
		Sleep( 100 );
	}
}