//
//  SingletonGrabber.h
//  FPFWD Project
//
//	Provide a design for singleton pattern. Use static method SingletonGrabber::
//	GetInstance(args...) to get the instance. If there is no instance before, the
//	grabber will use args to initialize one instance and return true while false if
//	there is one.
//
//	Return type is a std::pair<std::unique_ptr<Singleton>, bool>. the unique pointer 
//	is the smart pointer controlling the singleton instance while the bool value shows
//	whether your params passed is used to initialize the instance.
//
//  Created by Steven Chang on 15/6/5.
//  Copyright (c) 2015 Feathergames. All rights reserved.
//

#pragma once
#include "includes.h"

template <typename Singleton>
class SingletonGrabber final {
public:
	// Explicitly set all default functions as deleted
	SingletonGrabber() = delete;
	SingletonGrabber(const SingletonGrabber&) = delete;
	SingletonGrabber operator=(const SingletonGrabber&) = delete;
	~SingletonGrabber() = delete;

	template <typename... Args>
	// ONLY provided this function to get a shared_ptr to the singleton instance
	static std::pair<std::shared_ptr<Singleton>, bool> GetInstance(Args&&... args) {

		// If the m_instance has been existed, omit args...
		bool isInstanceNotExisted = (m_instance == nullptr);

		std::lock_guard<std::mutex> lock(m_tmutex._m_mutex);
		if (isInstanceNotExisted) {
			m_instance = std::make_shared<Singleton>(std::forward(args)...);
		}
		++m_reference;

		std::shared_ptr<Singleton> returnPtr(m_instance);

		return std::make_pair<std::shared_ptr<Singleton>, bool>(
			std::move(returnPtr), std::move(isInstanceNotExisted));
	}

private:
	// Declare this function only to make this class abstract
	virtual void MakeAbstract_() = 0;

	// the Singleton instance
	static std::shared_ptr<Singleton> m_instance;
	// the mutex
	static _TEMPLATED_MUTEX_<Singleton> m_tmutex;
	// the singleton reference (RC)
	static unsigned m_reference;
};

template <typename Singleton>
std::shared_ptr<Singleton> SingletonGrabber<Singleton>::m_instance = nullptr;

template <typename Singleton>
_TEMPLATED_MUTEX_<Singleton> SingletonGrabber<Singleton>::m_tmutex;

template <typename Singleton>
unsigned SingletonGrabber<Singleton>::m_reference = 0;