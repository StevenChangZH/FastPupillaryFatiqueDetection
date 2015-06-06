//
//  ContinuousPool.inl
//  FPFWD Project
//
//  Created by Steven Chang on 15/6/5.
//  Copyright (c) 2015 Feathergames. All rights reserved.
//

#pragma once
#include "ContinuousPool.h"

_NAMESPACE_STL_THREAD_POOL_START_

template <typename TC, size_t NUM_THREADS>
ContinuousPool<TC, NUM_THREADS>::ContinuousPool()
{
	// Initialize the ThreadController
	for (unsigned int i = 0; i < NUM_THREADS; ++i) {
		std::unique_ptr<TC> pController(new TC());
		this->m_controllerVec.push_back(std::move(pController));
	}

	// Turn on the pool
	this->Begin();
}

template <typename TC, size_t NUM_THREADS>
ContinuousPool<TC, NUM_THREADS>& ContinuousPool<TC, NUM_THREADS>::operator=
	(const ContinuousPool<TC, NUM_THREADS>&& pool)const
{
	return pool;
}

template <typename TC, size_t NUM_THREADS>
ContinuousPool<TC, NUM_THREADS>::~ContinuousPool()
{}

template <typename TC, size_t NUM_THREADS>
void ContinuousPool<TC, NUM_THREADS>::runLoop()
{
	int pressedKey = 0;// used to record keyboard event

	// If 'q' pressed terminated
	while (pressedKey != 'q') {

		try {
			std::unique_ptr<TC>& pController = this->nextController();
			// If excepton throwed then the following work will not be done

			// First synchronize your data & set callback
			// Set the semaphore to trigger the event
			pController->doJob();

		}
		catch (const std::bad_exception&) {
			// do nothing, skip this loop
		}

		// Quit
		//pressedKey = cvWaitKey(30);
	}

	// Terminate all
	std::for_each(m_controllerVec.begin(), m_controllerVec.end(),
		[](std::unique_ptr<TC>& pController) {
		pController->terminate(); });
}

template <typename TC, size_t NUM_THREADS>
std::unique_ptr<TC>& ContinuousPool<TC, NUM_THREADS>::nextController()
{
	// There can be various of assignment methdos
	// returns the index as current threadController
	// if all threadController having jobs, return the null ref. (prioritized order)
	auto iter = m_controllerVec.begin();
	for (iter = m_controllerVec.begin(); iter != m_controllerVec.end(); ++iter) {
		if (!(*iter)->hasJob()) return *iter;
	}

	throw std::bad_exception("[Exception] Return an nullref");
	return NULLREF(std::unique_ptr<TC>);//Placeholder
}

template <typename TC, size_t NUM_THREADS>
void ContinuousPool<TC, NUM_THREADS>::Begin()
{
	// Call TC::Begin method
	std::for_each(m_controllerVec.begin(), m_controllerVec.end(),
		[](std::unique_ptr<TC>& pController) { pController->Begin(); });
}


_NAMESPACE_STL_THREAD_POOL_END_