//
//  AbstractThreadJob.h
//  FPFWD Project
//
//	Abstract thread job class, only used to define some fundamental features
//
//  Created by Steven Chang on 15/6/5.
//  Copyright (c) 2015 Feathergames. All rights reserved.
//

#pragma once
#include "includes.h"

_NAMESPACE_STL_THREAD_POOL_START_

class AbstractThreadJob {
public:
	AbstractThreadJob(){}
	virtual ~AbstractThreadJob(){}

	// Task method, do it as job
	virtual void Task() = 0;
};

_NAMESPACE_STL_THREAD_POOL_END_