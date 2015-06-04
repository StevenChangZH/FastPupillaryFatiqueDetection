#pragma once
#include "includes.h"
#include "ThreadJob.h"

// Controls a thread and do one job consistantly.
// Notice the controller can only run once.
class ThreadController
{
public:
	ThreadController();
	ThreadController(const std::string& s_cascade);
	virtual ~ThreadController();

	// Start running  - can only called once
	// may throw std::runtime_error exception
	virtual void start();
	// Run loop control
	virtual void runLoop();
	// Synchronize data
	virtual void SynchronizeData(cv::Mat&);

	// Set & Get the semophare
	// do job once
	virtual void doJob();
	virtual bool hasJob() const;
	// Terminate - can only called once
	virtual void terminate();
	virtual bool isTerminated() const;

protected:
	ThreadController& operator = (const ThreadController&) = delete;

	// the thread
	std::thread m_thread;
	// the job
	std::unique_ptr<ThreadJob> m_job;

	// Job assignment semophare
	bool m_hasJob;
	// Termination semophare
	bool m_isTerminated;
};

