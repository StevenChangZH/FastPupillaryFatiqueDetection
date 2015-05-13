#include "stdafx.h"
#include "ThreadController.h"
#include "SingletonGrabber.h"
#pragma once

#ifndef _THREAD_POOL_
#define _THREAD_POOL_
// Thread Pool, keep it as a singleton
class ThreadPool
{
public:
	// Set friend
	template<class T>
	friend class SingletonGrabber;

	// Start method, used to call detach method to get into the loop
	void start();
	// Run loop method, decide each conroller's action
	void runLoop();

	// Cascade name
	const std::string cascadeName;

private:
	// Hide these methods to realize singleton
	explicit ThreadPool(void);
	ThreadPool(const ThreadPool*);
	ThreadPool* operator=(const ThreadPool*);
	~ThreadPool();

protected:
	// Some assistance methods
	// Show which threadController has no job
	ThreadController* checkFreedom();

	// OpenCV variables
	cv::CascadeClassifier	m_cascade;// Cascade classifier
	IplImage*				m_image;// Image of every frame

	// Thread controller vector
	std::vector<ThreadController*> m_controllerVec;

	// Use number of threads as a constant
	static const unsigned int MAX_THREAD_NUM = 4;
};

#endif