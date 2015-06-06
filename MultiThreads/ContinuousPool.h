//
//  ContinuousPool.h
//  FPFWD Project
//
//	A thread pool with continuous works and never stop - except recevied the 
//	semaphore. 
//
//  Created by Steven Chang on 15/6/5.
//  Copyright (c) 2015 Feathergames. All rights reserved.
//

#pragma once
#include "AbstractThreadPool.h"

_NAMESPACE_STL_THREAD_POOL_START_

template <typename ThreadController, size_t NUM_THREADS = 4>
class ContinuousPool : public AbstractThreadPool {
	using TC = ThreadController;
	using CP = ContinuousPool < ThreadController, NUM_THREADS>;

public:
	ContinuousPool();
	CP& operator=(const CP&&) const throw();//_NOEXCEPT
	virtual ~ContinuousPool();

	// Run loop method controlling the job assignments
	virtual void runLoop();

protected:
	// Begin running once initialized
	virtual void Begin();
	// How do you assign next thread controller 
	// It will return an exception if no valid reference
	virtual std::unique_ptr<TC>& nextController();

	// Thread controller vector
	std::vector<std::unique_ptr<TC>> m_controllerVec;

private:
	ContinuousPool(const ContinuousPool&) = delete;
	ContinuousPool(ContinuousPool&&) = delete;
	ContinuousPool& operator=(const ContinuousPool&) = delete;
};

_NAMESPACE_STL_THREAD_POOL_END_
#include "ContinuousPool.inl"