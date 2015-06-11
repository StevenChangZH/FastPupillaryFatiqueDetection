//
//  PDThreadController.h
//  FPFWD Project
//
//	thread controller of this project
//
//  Created by Steven Chang on 15/6/5.
//  Copyright (c) 2015 Feathergames. All rights reserved.
//

#pragma once
#include "Threads\ContinuousController.h"
#include "PDThreadJob.h"

class PDThreadPool;


// thread controller of this project
class PDThreadController : public stl_tp::ContinuousController<PDThreadJob> {
public:
	PDThreadController();
	PDThreadController(PDThreadPool* poolPtr_);
	virtual ~PDThreadController();
	
	// Initialization. Called after ctor.
	virtual void Initialize() override;

	// Run loop method controlling the job assignments - keep it working
	virtual void Loop() override;

	// Called by ThreadPool only. Called before Task() method to exchange data
	void SynchronizeData(cv::Mat& img_, std::chrono::system_clock::time_point& tp_);

	// Override these two function to use condition_variable
	virtual void doJob() override { m_hasJob = true; m_conVariable.notify_all(); }
	virtual void terminate() override { m_isTerminated = true; m_conVariable.notify_all(); }

	//// pre-tick callback - use capture list
	//virtual void PreCallback(std::function<void(void)>&) = 0;
	//// post-tick callback - use capture list
	//virtual void PostCallback(std::function<void(void)>&) = 0;
	
protected:
	// Set the Pool ptr explicitly. DO not try to release it.
	PDThreadPool* poolptr;

	// Condition variable for thread to wait and call
	std::condition_variable m_conVariable;
	// Mutex, only used paired with m_conVariable
	std::mutex m_mutex;
};

