//
//  AbstractThreadPool.h
//  FPFWD Project
//
//	Abstract thread pool class, only used to define some fundamental features
//
//  Created by Steven Chang on 15/6/5.
//  Copyright (c) 2015 Feathergames. All rights reserved.
//

#pragma once
#include "includes.h"

_NAMESPACE_STL_THREAD_POOL_START_

class AbstractThreadPool {
public:
	AbstractThreadPool(){}
	virtual ~AbstractThreadPool(){}

	// Run loop method controlling the job assignments - keep it working
	virtual void runLoop() = 0;

protected:
	// Begin running once initialized
	virtual void Begin() = 0;
};

_NAMESPACE_STL_THREAD_POOL_END_