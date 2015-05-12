#include "stdafx.h"
#include "ThreadController.h"
#pragma once

#ifndef _THREAD_POOL_
#define _THREAD_POOL_
// Thread Pool, keep it as a singleton
class ThreadPool
{
public:
	// Use this to get instance instead of default constructors
	static ThreadPool* getInstance();

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
	~ThreadPool(void);

	// The singleton
	static ThreadPool* instance;

protected:
	// Some assistance methods
	// Show which threadController has no job
	ThreadController* checkFreedom();

	// OpenCV variables
	cv::CascadeClassifier	m_cascade;// Cascade classifier
	CvMemStorage*			m_memStorage;// Memory storage
	IplImage*				m_image;// Image of every frame

	// Thread controller vector
	std::vector<ThreadController*> m_controllerVec;

	// Use number of threads as a constant
	static const unsigned int MAX_THREAD_NUM = 4;
};

#endif