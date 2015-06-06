//
//  ContinuousController.h
//  FPFWD Project
//
//	Thread controller paired with ContinuousPool.
//
//  Created by Steven Chang on 15/6/5.
//  Copyright (c) 2015 Feathergames. All rights reserved.
//

#pragma once
#include "..\includes.h"
#include "AbstractThreadController.h"

_NAMESPACE_STL_THREAD_POOL_START_


template <typename ThreadJob>
// A thread controller with continuous jobs and never stop - except recevied the 
// semaphore. 
class ContinuousController : public AbstractThreadController {
public:
	ContinuousController();
	virtual ~ContinuousController();

	// Begin running - call detach() function
	// may throw std::runtime_error exception
	virtual void Begin();
	// Run loop method controlling the job assignments - keep it working
	virtual void Loop();

	//// pre-tick callback - use capture list
	//virtual void PreCallback(std::function<void(void)>&) = 0;
	//// post-tick callback - use capture list
	//virtual void PostCallback(std::function<void(void)>&) = 0;

protected:
	// the job obj
	std::unique_ptr<ThreadJob> m_job;

private:
	ContinuousController(const ContinuousController&) = delete;
	ContinuousController<ThreadJob>& operator = (const ContinuousController<ThreadJob>&)
		= delete;
};

_NAMESPACE_STL_THREAD_POOL_END_
#include "ContinuousController.inl"