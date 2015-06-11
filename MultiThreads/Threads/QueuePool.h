//
//  QueuePool.h
//  FPFWD Project
//
//	Class QueuePool
//
//	Tranditional thread pool. It will assign three queues: a job queue, a working
//	threads queue and a rest threads queue. If client assigns a job, a rest thread in
//	the rest threads queue will get the job and become a busy thread. After processing 
//	the job it will get back to a rest thread again.
//
//	Another thread(m_thread) controlled by this QueuePool controlling everything via
//	runLoop() method. It will not terminate until the client call Terminate() method.
//
//  Created by Steven Chang on 15/6/5.
//  Copyright (c) 2015 Feathergames. All rights reserved.
//

#pragma once
#include "AbstractThreadPool.h"
#include "AbstractThreadController.h"

_NAMESPACE_STL_THREAD_POOL_START_

template <typename ThreadController, typename ThreadJob, size_t NUM_THREADS = 4>
class QueuePool : protected AbstractThreadPool {
	using QPool = QueuePool < ThreadController, ThreadJob, NUM_THREADS >;
	friend class JobAssignController;

public:
	QueuePool();
	virtual ~QueuePool();
	
	// Initialization. Called after ctors.
	virtual void Initialize() override;

	// Assign Job
	virtual void AssignJob(const ThreadJob& job_);
	// Check number of working threads
	virtual size_t GetWorkingThreadNum();
	// Check number of rest threads
	virtual size_t GetRestThreadNum();
	// Get number of all threads, including the member thread in this class.
	virtual size_t GetThreadNum();
	// Terminate
	virtual void Terminate();

protected:
	// Begin running once initialized - use this to start processing jobs
	virtual void Begin();
	// Run loop method called by job assignment controller
	virtual void runLoop();
	// Pop the front rest thread into working queue
	virtual bool moveToWork();
	// Move all rest thread into rest queue
	virtual void moveAllToRest();

	// Job assignment controller
	std::unique_ptr<JobAssignController> m_assignCtrlr;

	// Queues of controllers - working
	std::vector<std::unique_ptr<ThreadController>> m_workingVec;
	// Queues of controllers - rest
	std::queue<std::unique_ptr<ThreadController>> m_restQueue;
	// Queue of jobs
	std::queue<ThreadJob> m_jobQueue;

private:
	QueuePool(const QueuePool&) = delete;
	QueuePool(QueuePool&&) = delete;
	QueuePool& operator=(const QueuePool&) = delete;


protected:

	// Thread controller within this class controlling the job 
	// assignment loop - only used in this class and its derived
	class JobAssignController : protected AbstractThreadController {
	public:
		JobAssignController(const QPool* pool_);
		virtual ~JobAssignController();

		// Begin running - call detach() function
		// may throw std::runtime_error exception if tried to call twice
		virtual void Begin();
		// Run loop assigning the jobs - call runLoop() method only
		virtual void Loop();

	protected:
		// pool ptr
		QPool* pool;

	private:
		JobAssignController() = delete;
	};
};

_NAMESPACE_STL_THREAD_POOL_END_
#include "QueuePool.inl"
