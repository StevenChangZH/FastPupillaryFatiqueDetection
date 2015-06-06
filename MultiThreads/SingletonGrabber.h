//
//  SingletonGrabber.h
//  FPFWD Project
//
//	singleton grabber used to assure singleton.
//
//  Created by Steven Chang on 15/6/5.
//  Copyright (c) 2015 Feathergames. All rights reserved.
//

#pragma once
#include "includes.h"

// Mutex, assure one type has only one mutex->it is a singleton as well
template <typename T> struct _SINGLETON_GRABBER_MUTEX_ {
	std::mutex _m_mutex;
};


template <typename Singleton> 
class SingletonGrabber final {
public:
	explicit SingletonGrabber();
	explicit SingletonGrabber(const SingletonGrabber& grabber) { ++m_reference; }
	SingletonGrabber& operator = (const SingletonGrabber& singleton) { return *this; } 
	virtual ~SingletonGrabber();
	
	// Use this to get instance instead of default constructors
	virtual std::unique_ptr<Singleton>& GetInstance() final;

private:
	// the instance
	static std::unique_ptr<Singleton> m_pInstance;
	// Thread mutex
	static struct _SINGLETON_GRABBER_MUTEX_<typename Singleton> m_sgmutex;
	// Grabber reference of the singleton instance
	static unsigned int m_reference;
};


template <typename Singleton>
std::unique_ptr<Singleton> SingletonGrabber<Singleton>::m_pInstance = nullptr;

template <typename Singleton>
unsigned int SingletonGrabber<Singleton>::m_reference = 0;

template <typename Singleton>
_SINGLETON_GRABBER_MUTEX_<Singleton> SingletonGrabber<Singleton>::m_sgmutex;


template <typename Singleton>
SingletonGrabber<Singleton>::SingletonGrabber()
{
	std::lock_guard<std::mutex> lock(m_sgmutex._m_mutex);
	if ( m_pInstance == nullptr ) {
		m_pInstance = std::make_unique<Singleton>();
	}
	++m_reference;
}

template <typename Singleton>
SingletonGrabber<Singleton>::~SingletonGrabber()
{
	std::lock_guard<std::mutex> lock(m_sgmutex._m_mutex);
	--m_reference;
	if ( m_reference == 0 ) {
		m_pInstance = nullptr;
	}
}
template <typename Singleton>
std::unique_ptr<Singleton>& SingletonGrabber<Singleton>::GetInstance()
{
	std::lock_guard<std::mutex> lock(m_sgmutex._m_mutex);
	if (m_pInstance == nullptr) {
		m_pInstance = std::unique_ptr<Singleton>();
	} 
	
	return m_pInstance;
}