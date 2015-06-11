//
//  QueuePool.inl
//  FPFWD Project
//
//  Created by Steven Chang on 15/6/5.
//  Copyright (c) 2015 Feathergames. All rights reserved.
//

#pragma once
#include "QueuePool.h"

_NAMESPACE_STL_THREAD_POOL_START_

template <typename ThreadController, typename ThreadJob, size_t NUM_THREADS>
QueuePool<ThreadController, ThreadJob, NUM_THREADS>::QueuePool()
{}

template <typename ThreadController, typename ThreadJob, size_t NUM_THREADS>
QueuePool<ThreadController, ThreadJob, NUM_THREADS>::~QueuePool()
{}

template <typename ThreadController, typename ThreadJob, size_t NUM_THREADS>
void QueuePool<ThreadController, ThreadJob, NUM_THREADS>::Initialize()
{
	// Initialize the working queue - for it will be pushed to rest queue
	// and call Begin()
	pController.reserve(NUM_THREADS);
	for (unsigned i = 0; i < NUM_THREADS; ++i) {
		auto pController = std::make_unique<ThreadController>();
		pController->Initialize();
		this->m_workingVec.push_back(std::move(pController));
	}
	m_assignCtrlr = std::make_unique(this);

	// Detach all threads
	this->Begin();
}

template <typename ThreadController, typename ThreadJob, size_t NUM_THREADS>
void QueuePool<ThreadController, ThreadJob, NUM_THREADS>::AssignJob(const ThreadJob& job_)
{
	m_jobQueue.push(job_);
}

template <typename ThreadController, typename ThreadJob, size_t NUM_THREADS>
size_t QueuePool<ThreadController, ThreadJob, NUM_THREADS>::GetWorkingThreadNum()
{
	return m_workingQueue.size();
}

template <typename ThreadController, typename ThreadJob, size_t NUM_THREADS>
size_t QueuePool<ThreadController, ThreadJob, NUM_THREADS>::GetRestThreadNum()
{
	return m_restQueue.size();
}

template <typename ThreadController, typename ThreadJob, size_t NUM_THREADS>
size_t QueuePool<ThreadController, ThreadJob, NUM_THREADS>::GetThreadNum()
{
	return NUM_THREADS + 1;
}

template <typename ThreadController, typename ThreadJob, size_t NUM_THREADS>
void QueuePool<ThreadController, ThreadJob, NUM_THREADS>::Terminate()
{
	// terminate and pop
	for (auto& ctrlr : m_workingVec) {
		ctrlr->terminate();
	}

	for (size_t i = 0; i < m_restQueue.size(); ++i) {
		std::unique_ptr<ThreadController> ctrlr(std::move(m_restQueue.front()));
		ctrlr->terminate();
		m_restQueue.pop();
	}
}

template <typename ThreadController, typename ThreadJob, size_t NUM_THREADS>
void QueuePool<ThreadController, ThreadJob, NUM_THREADS>::Begin()
{
	// Do nothing...
	m_assignCtrlr->Begin();

	// Call begin for every working threads and move them to rest queue
	for (size_t i = 0; i < NUM_THREADS; ++i) {
		std::unique_ptr<ThreadController> ctrlr(std::move(m_workingVec[0]));
		m_workingVec.erase(m_workingVec.begin());
		ctrlr->Begin();
		m_restQueue.push(std::move(ctrlr));
	}
}

template <typename ThreadController, typename ThreadJob, size_t NUM_THREADS>
void QueuePool<ThreadController, ThreadJob, NUM_THREADS>::runLoop()
{
	int pressedKey = 0;// used to record keyboard event

	// If 'q' pressed terminated
	while (pressedKey != 'q') {

		try {
			std::unique_ptr<TC>& pController = this->nextController();
			// If excepton throwed then the following work will not be done

			// Do a job
			if (!m_jobQueue.empty() && !m_restQueue.empty()) {
				// First synchronize your data & set callback
				// Set the semaphore to trigger the event
				this->moveToWork();
			}

			// If a thread finished its job then set it back to restQueue
			this->moveAllToRest();
		}
		catch (const std::bad_exception&) {
			// do nothing, skip this loop
		}

		// Quit
		//pressedKey = cvWaitKey(30);
	}

	// Terminate all
	this->Terminate();
}

template <typename ThreadController, typename ThreadJob, size_t NUM_THREADS>
bool QueuePool<ThreadController, ThreadJob, NUM_THREADS>::moveToWork()
{
	if (!m_restQueue.empty() && !m_jobQueue.empty()) {
		std::unique_ptr<ThreadController> ctrlr(std::move(m_restQueue.front()));
		m_restQueue.pop();
		ctrlr->GetJobObject(m_jobQueue.front());
		m_jobQueue.pop();
		m_workingVec.push_back(std::move(ctrlr));
		return true;
	} 
	return false;
}

template <typename ThreadController, typename ThreadJob, size_t NUM_THREADS>
void QueuePool<ThreadController, ThreadJob, NUM_THREADS>::moveAllToRest()
{
	for (auto it = m_workingVec.begin(); it != m_workingVec.end(); ++it) {
		if (!it->hasJob()) {
			auto ctrlr = std::make_unique(std::move(*it));
			it = m_workingVec.erase(it);
			m_restQueue.push(std::move(ctrlr));
		}
	}
}






template <typename ThreadController, typename ThreadJob, size_t NUM_THREADS>
QueuePool<ThreadController, ThreadJob, NUM_THREADS>::JobAssignController::
	JobAssignController(const QueuePool<ThreadController, ThreadJob, NUM_THREADS>* pool_)
{
	this->pool = pool_;

	// Regitster the Loop() function
	m_thread = std::thread([this] { this->Loop(); });
}

template <typename ThreadController, typename ThreadJob, size_t NUM_THREADS>
QueuePool<ThreadController, ThreadJob, NUM_THREADS>::JobAssignController::
	~JobAssignController()
{}

template <typename ThreadController, typename ThreadJob, size_t NUM_THREADS>
void QueuePool<ThreadController, ThreadJob, NUM_THREADS>::JobAssignController::
	Begin()
try {
	if (m_thread.joinable()) {
		// Yes, detach but join
		this->m_thread.detach();__super
	} else {
		throw std::runtime_error("[EXCEPTION] Attempt to recall detach"+
			"method for a unjoinable thread");
	}
}
catch (const std::runtime_error& excp) {
	// Attempt to recall the detach method
	std::cerr << excp.what() << std::endl;
	throw;
}

template <typename ThreadController, typename ThreadJob, size_t NUM_THREADS>
void QueuePool<ThreadController, ThreadJob, NUM_THREADS>::JobAssignController::
	Loop()
{
	// Only call pool->runLoop() method
	pool->runLoop();
}

_NAMESPACE_STL_THREAD_POOL_END_