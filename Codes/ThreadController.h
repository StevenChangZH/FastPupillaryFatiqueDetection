#include "stdafx.h"
#pragma once

#ifndef _THREAD_CONTROLLER_
#define _THREAD_CONTROLLER_

// The controller controls a thread
class ThreadController
{
public:
	ThreadController(unsigned int id, cv::CascadeClassifier cas);
	~ThreadController(void);

	// Start method, used to call detach method
	void start();
	// Get thread id
	unsigned int getId();

	// If the thread has any job, modify this to control actions
	bool hasJob;
	// Terminated semaphore
	bool isTerminated;

	// Global cascade
	cv::CascadeClassifier cascade;
	// The copy-and-write image
	IplImage* img;

protected:
	// Run loop method, use this method to register actions after thread detached
	void runLoopControl();

	// THE thread
	std::thread m_thread;
	// id
	const unsigned int id;

private:
	ThreadController();

	// Detect method, used in ThreadController::runLoopControl
	void detectPupil();

	//// Some params to monitor thread to protect thread safety
	//// This bool is used to show if the thread::detach has returned
	//bool isReallyTerminated;
};

#endif