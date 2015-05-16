#include "stdafx.h"
#pragma once

#ifndef _THREAD_CONTROLLER_
#define _THREAD_CONTROLLER_

// The controller controls a thread and assign jobs 
class ThreadController 
{
	// Set friend
	friend class ThreadPool;

public:
	ThreadController(unsigned int id, const std::string &cascadeName);
	~ThreadController(void);

	// Get thread id
	unsigned int getId();

	// If the thread has any job, modify this to control actions
	bool hasJob;
	// Terminated semaphore
	bool isTerminated;

	// The copy-and-write image
	cv::Mat frame_img;

protected:
	// Run loop method, use this method to register actions after thread detached
	virtual void runLoopControl();

	// THE thread
	std::thread m_thread;
	// id
	const unsigned int id;
	// cascade
	cv::CascadeClassifier cv_cascade;

private:
	ThreadController();
	// Those methods should only be used by ThreadPool
	// Start method, used to call detach method
	void start();

	// Detect method, used in ThreadController::runLoopControl
	void detectPupil();
};

#endif