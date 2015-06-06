//
//  ContinuousController.inl
//  FPFWD Project
//
//  Created by Steven Chang on 15/6/5.
//  Copyright (c) 2015 Feathergames. All rights reserved.
//

#pragma once
#include "ContinuousController.h"
_NAMESPACE_STL_THREAD_POOL_START_

template <typename ThreadJob>
ContinuousController<ThreadJob>::ContinuousController()
{
	// Initialize the job
	m_job = std::make_unique<ThreadJob>();

	// Regitster the Loop() function
	m_thread = std::thread([this] { this->Loop(); });
}

template <typename ThreadJob>
ContinuousController<ThreadJob>::~ContinuousController()
{
	m_job = nullptr;
}

template <typename ThreadJob>
void ContinuousController<ThreadJob>::Begin()
try {
	if (m_thread.joinable()) {
		// Yes, detach but join
		this->m_thread.detach();
	}
	else {
		throw std::runtime_error("[EXCEPTION] Attempt to recall detach method for a unjoinable thread");
	}
}
catch (const std::runtime_error& excp) {
	// Attempt to recall the detach method
	std::cerr << excp.what() << std::endl;
	throw;
}

template <typename ThreadJob>
void ContinuousController<ThreadJob>::Loop()
{
	while (!m_isTerminated) {
		if (m_hasJob) {
			// Maybe you can see it as a 'tick'
			// Do something here
			m_job->Task();


			// Set the semaphore back
			m_hasJob = false;
		}

		// Sleep for a few ms
		//Sleep(33);
	}
}




_NAMESPACE_STL_THREAD_POOL_END_