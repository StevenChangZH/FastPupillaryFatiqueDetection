//
//  AbstractThreadController.h
//  FPFWD Project
//
//	Abstract thread controller class, only used to define some fundamental features
//
//  Created by Steven Chang on 15/6/5.
//  Copyright (c) 2015 Feathergames. All rights reserved.
//

#pragma once
#include "includes.h"

_NAMESPACE_STL_THREAD_POOL_START_

class AbstractThreadController {
public:
	AbstractThreadController(){}
	virtual ~AbstractThreadController(){}

	// Begin running - call detach() function
	virtual void Begin() = 0;
	// Run loop method controlling the job assignments - keep it working
	virtual void Loop() = 0;

	//// pre-tick callback - use capture list
	//virtual void PreCallback(std::function<void(void)>&) = 0;
	//// post-tick callback - use capture list
	//virtual void PostCallback(std::function<void(void)>&) = 0;


	// Set & Get the semophare
	// do job once
	virtual void doJob() { m_hasJob = true; }
	virtual bool hasJob() const { return m_hasJob; }
	// Terminate - can only called once
	virtual void terminate() { m_isTerminated = true; }
	virtual bool isTerminated() const{ return m_isTerminated; }

protected:
	// the thread
	std::thread m_thread;

	// Job assignment semophare
	bool m_hasJob = false;
	// Termination semophare
	bool m_isTerminated = false;

private:
	AbstractThreadController(const AbstractThreadController&) = delete;
	AbstractThreadController& operator = (const AbstractThreadController&) = delete;
};

_NAMESPACE_STL_THREAD_POOL_END_