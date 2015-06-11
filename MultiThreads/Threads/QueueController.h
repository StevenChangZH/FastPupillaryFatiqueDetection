//
//  QueueController.h
//  FPFWD Project
//
//	Thread controller paired with QueuePool.
//
//  Created by Steven Chang on 15/6/5.
//  Copyright (c) 2015 Feathergames. All rights reserved.
//

#pragma once
#include "AbstractThreadController.h"

_NAMESPACE_STL_THREAD_POOL_START_

template <typename ThreadJob>
class QueueController : public AbstractThreadController {
public:
	QueueController();
	virtual ~QueueController();

	// Initialization. Called after ctors.
	virtual void Initialize() override;

	// Begin running - call detach() function
	// may throw std::runtime_error exception
	virtual void Begin();
	// Run loop method controlling the job assignments - keep it working
	virtual void Loop();
	// Get the job object from pool
	virtual void GetJobObject(std::unique_ptr<ThreadJob>&& job);

	// Rewrite this method
	virtual void terminate() { m_isTerminated = true; m_isRunning = false; }
	// Get if the thread has benn detached or joined
	virtual bool isRunning() const;

protected:
	// the job obj
	std::unique_ptr<ThreadJob> m_job;
};


_NAMESPACE_STL_THREAD_POOL_END_
#include "QueueController.inl"