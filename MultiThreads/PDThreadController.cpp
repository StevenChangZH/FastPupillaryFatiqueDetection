//
//  PDThreadController.cpp
//  FPFWD Project
//
//  Created by Steven Chang on 15/6/5.
//  Copyright (c) 2015 Feathergames. All rights reserved.
//

#include "PDThreadController.h"


PDThreadController::PDThreadController()
{}


void PDThreadController::Initialize()
{
	// Initialize the job
	m_job = std::make_unique<PDThreadJob>("haarcascade_eye_tree_eyeglasses.xml");

	// Regitster the Loop() function
	bool isJoinable = m_thread.joinable();
	m_thread = std::thread(&PDThreadController::Loop, this);
}

PDThreadController::~PDThreadController()
{}

void PDThreadController::SynchronizeData(cv::Mat& img_,
	std::chrono::system_clock::time_point& tp_)
{
	m_job->SynchronizeData(img_, tp_);
}

void PDThreadController::Loop()
{
	// Use std::condition_variable to control whether we should set a thread
	// sleep or not. There are two semaphore: m_hasJob and m_isTreminated. If 
	// thread pool assigned a job to the controller or force them to terminate,
	// the condition_variable will notify all threads waiting for it (only one, 
	// explicitly.). Otherwise the thread will sleep and process no CPU time.

	std::unique_lock<std::mutex> lk(m_mutex);
	while (!m_isTerminated) {
		m_conVariable.wait(lk);
		if (!m_isTerminated) {
			// this check is used for termination
			// Maybe you can see it as a 'tick'
			// Do something here
			m_job->Task();

			// Set the semaphore back
			m_hasJob = false;
		}
		// Sleep for a few ms
		//std::this_thread::sleep_for(std::chrono::milliseconds(33));
	}
}