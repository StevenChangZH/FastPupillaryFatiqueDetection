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
	SingletonGrabber();
	SingletonGrabber(const SingletonGrabber<Singleton>& grabber) { ++m_reference; }
	SingletonGrabber(const SingletonGrabber<Singleton>&& grabber) { }
	template <class... Args>
	// It's silly to define a bool DIRECTCONSTRUCT, but we need a way to separate
	// Input true - or use GetGrabber() function instead.
	SingletonGrabber(bool&& DIRECTCONSTRUCT, Args&... args);

	SingletonGrabber& operator = (const SingletonGrabber& singleton) { return *this; } 
	virtual ~SingletonGrabber();
	
	template <typename... Args>
	// return a grabber with constructing a singleton instance with arguments.
	static SingletonGrabber GetGrabber(Args& ...args);
	// Use this to get instance instead of default constructors
	static std::unique_ptr<Singleton>& GetInstance();

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
template <typename... Args>
SingletonGrabber<Singleton>::SingletonGrabber(bool&& DIRECTCONSTRUCT, Args&... args)
{
	// If the m_pInstance has been existed, omit the args
	std::lock_guard<std::mutex> lock(m_sgmutex._m_mutex);
	if (m_pInstance == nullptr) {
		m_pInstance = std::make_unique<typename Singleton>(args...);
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
template <typename... Args>
static SingletonGrabber<Singleton> SingletonGrabber<Singleton>::GetGrabber(Args& ...args) {
	SingletonGrabber tpgrabber_(true, args...);
	return tpgrabber_;
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