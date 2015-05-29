#pragma once
#include "includes.h"
#include "ThreadController.h"

// In this design, the thread pool will do one job consistantly.
// It will start running once initialized.
// The thread pool is an singleton.
class ThreadPool
{
public:
	explicit ThreadPool();
	explicit ThreadPool(unsigned int tNum, const std::string& cName);
	virtual ~ThreadPool();

	// Run loop method controlling the job assignments
	virtual void runLoop();
	// How do you assign next thread controller 
	// It will return an exception if no valid reference
	virtual std::unique_ptr<ThreadController>& nextController();

	// OpenCV variables
	// Cascade name
	const std::string cascadeName;
	// Image of every frame
	cv::Mat	m_image;
	// Thread controller vector
	std::vector<std::unique_ptr<ThreadController>> m_controllerVec;

private:
	ThreadPool(const ThreadPool&) = delete;
	ThreadPool& operator=(const ThreadPool&) = delete;

	// Begin running once initialized
	virtual void Begin();

	// number of threads
	const unsigned int NUM_THREADS;
};

