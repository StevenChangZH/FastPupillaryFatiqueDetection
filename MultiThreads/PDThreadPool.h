//
//  PDThreadPool.h
//  FPFWD Project
//
//	thread pool of this project
//
//  Created by Steven Chang on 15/6/5.
//  Copyright (c) 2015 Feathergames. All rights reserved.
//

#pragma once
#include "ContinuousPool.h"
#include "PDThreadController.h"

template <size_t NUM_THREADS = 4>
class PDThreadPool : public stl_tp::ContinuousPool<PDThreadController, NUM_THREADS> {
public:
	PDThreadPool();
	virtual ~PDThreadPool();

	// Run loop method controlling the job assignments
	virtual void runLoop();

};

#include "PDThreadPool.inl"