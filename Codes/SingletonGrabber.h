#include "stdafx.h"
#pragma once

#ifndef _SINGLETON_GRABBER_
#define _SINGLETON_GRABBER_
template<class T>
// Use this instead of singleton class directly
class SingletonGrabber
{
public:
	SingletonGrabber(void);
	~SingletonGrabber(void);
	
	// Use this to get instance instead of default constructors
	static T* getInstance();

private:
	// the instance
	static T* m_pInstance;
	// the reference of this grabber
	static unsigned int m_reference;
};

#endif

template<class T>
T* SingletonGrabber<T>::m_pInstance = nullptr;
template<class T>
unsigned int SingletonGrabber<T>::m_reference = 0;

template<class T>
T* SingletonGrabber<T>::getInstance()
{
	if ( m_pInstance == nullptr ) {
		m_pInstance = new ThreadPool();
	}
	
	return m_pInstance;
}

template<class T>
SingletonGrabber<T>::SingletonGrabber(void)
{
	// Create instance
	if ( m_pInstance == nullptr ) {
		m_pInstance = new ThreadPool();
	}

	++m_reference;
}

template<class T>
SingletonGrabber<T>::~SingletonGrabber(void)
{
	--m_reference;

	if ( m_reference == 0 ) {
		// release
		delete m_pInstance;
		m_pInstance = nullptr;
	}
}
