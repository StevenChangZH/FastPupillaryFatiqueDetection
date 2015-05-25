#pragma once
#ifndef _SINGLETON_GRABBER_
#define _SINGLETON_GRABBER_
#include "includes.h"

// Use this instead of Singleton class directly.
template <class Singleton> class SingletonGrabber final
{
public:
	explicit SingletonGrabber();
	// ONLY use this if Singleton class has no default ctor
	// that is, singleton must be a temporary object
	explicit SingletonGrabber(Singleton*&& singleton) throw();
	explicit SingletonGrabber(const SingletonGrabber& grabber) { ++m_reference; }
	SingletonGrabber& operator = (const SingletonGrabber& singleton) { return *this; }
	virtual ~SingletonGrabber();
	
	// Use this to get instance instead of default constructors
	static std::unique_ptr<Singleton>& getInstance() { return m_pInstance; }

private:
	// the instance
	static std::unique_ptr<Singleton> m_pInstance;
	// the reference of this grabber
	static unsigned int m_reference;
};

#endif

template <class Singleton>
std::unique_ptr<Singleton> SingletonGrabber<Singleton>::m_pInstance = nullptr;

template <class Singleton>
unsigned int SingletonGrabber<Singleton>::m_reference = 0;

template <class Singleton>
SingletonGrabber<Singleton>::SingletonGrabber() 
{
	if ( m_pInstance == nullptr ) {
		//std::unique_ptr<Singleton> tp( new Singleton() );
		m_pInstance = std::move( std::unique_ptr<Singleton>() );
	}
	++m_reference;
}

template <class Singleton>
SingletonGrabber<Singleton>::SingletonGrabber(Singleton*&& singleton) throw() 
{
	if ( m_pInstance == nullptr ) {
		m_pInstance = std::move( std::unique_ptr<Singleton>( singleton ) );
		singleton = nullptr;
	}
	++m_reference;
}

template <class Singleton>
SingletonGrabber<Singleton>::~SingletonGrabber() 
{
	--m_reference;
	if ( m_reference == 0 ) {
		m_pInstance = nullptr;
	}
}
