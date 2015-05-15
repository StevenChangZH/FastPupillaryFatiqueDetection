#include "stdafx.h"
#include "UnInheritable.h"
#pragma once

#ifndef _THREAD_CONTROLLER_
#define _THREAD_CONTROLLER_

// The controller controls a thread: Cannot be inherited.
class ThreadController : public virtual Uninheritable<ThreadController>
{
public:
	ThreadController(unsigned int id, const std::string &cascadeName);
	~ThreadController(void);

	// Start method, used to call detach method
	void start();
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
	void runLoopControl();

	// THE thread
	std::thread m_thread;
	// id
	const unsigned int id;
	// cascade
	cv::CascadeClassifier cv_cascade;

private:
	ThreadController();

	// Detect method, used in ThreadController::runLoopControl
	void detectPupil();
};

#endif