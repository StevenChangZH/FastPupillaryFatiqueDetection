#pragma once
#include "includes.h"

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
	// Job contexts within once tick
	virtual void tick();

	// Set & Get the semophare
	// do job once
	virtual void doJob();
	virtual bool hasJob() const;
	// Terminate - can only called once
	virtual void terminate();
	virtual bool isTerminated() const;

	// The copy-by-value image
	// You can opt on it 
	cv::Mat frame_img;

protected:
	ThreadController& operator = (const ThreadController&) = delete;

	std::thread m_thread;

	// Job assignment semophare
	bool m_hasJob;
	// Termination semophare
	bool m_isTerminated;

	// OpenCV variables
	cv::CascadeClassifier classifier;
};

