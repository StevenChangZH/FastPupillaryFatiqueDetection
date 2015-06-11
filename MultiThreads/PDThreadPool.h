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
#include "Threads\ContinuousPool.h"
#include "PDThreadController.h"

const size_t NUM_THREADS = 4;

class PDThreadPool : public stl_tp::ContinuousPool<PDThreadController, NUM_THREADS> {
public:
	PDThreadPool();
	virtual ~PDThreadPool();

	// Rewrite the Initialize() function to modify ptrs for PDThreadController
	virtual void Initialize() override;

	// Run loop method controlling the job assignments
	virtual void runLoop() override;

	// Called only by ThreadController child. Used to store data.
	void GetSynchronizedDataFromThread(std::chrono::system_clock::time_point& duration,
		float& lDiameter, float& rDiameter);

protected:
	// mutex, used to receive data.
	std::mutex m_mutex;


public:
	// temporary pupilla data storage
	std::vector<std::chrono::system_clock::time_point> T_durationVec;//unit is millisecond
	std::vector<float> T_lDiameterVec;
	std::vector<float> T_rDiameterVec;
};