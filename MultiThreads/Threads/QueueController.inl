//
//  QueueController.inl
//  FPFWD Project
//
//  Created by Steven Chang on 15/6/5.
//  Copyright (c) 2015 Feathergames. All rights reserved.
//

#pragma once
#include "QueueController.h"

_NAMESPACE_STL_THREAD_POOL_START_

template <typename ThreadJob>
QueueController<ThreadJob>::QueueController()
{
	m_job = nullptr;

	// Regitster the Loop() function
	m_thread = std::thread([this] { this->Loop(); });
}

template <typename ThreadJob>
QueueController<ThreadJob>::~QueueController()
{}

template <typename ThreadJob>
void QueueController<ThreadJob>::Begin()
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
void QueueController<ThreadJob>::Loop()
{
	while (!m_isTerminated) {
		if (m_hasJob) {
			// Maybe you can see it as a 'tick'
			// Do something here
			m_job->Task();


			// Set the semaphore back
			m_job->nullptr;
			m_hasJob = false;
		}

		// Sleep for a few ms
		std::this_thread::sleep_for(std::chrono::milliseconds(33));
	}
}

template <typename ThreadJob>
void QueueController<ThreadJob>::GetJobObject(std::unique_ptr<ThreadJob>&& job)
{
	m_job = job;
	this->doJob();
}

template <typename ThreadJob>
bool QueueController<ThreadJob>::isRunning() const
{
	return !m_thread.joinable();
}

_NAMESPACE_STL_THREAD_POOL_END_