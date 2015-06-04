#pragma once
#include "includes.h"

// Mutex, assure one type has only one mutex->it is a singleton as well
template <typename T> struct _SINGLETON_GRABBER_MUTEX_ {
	std::mutex _m_mutex;
};


// Use this instead of Singleton class directly.
template <class Singleton> class SingletonGrabber final
{
public:
	explicit SingletonGrabber();
	// Use it ONLY like this: SingletonGrabber( new Singleton( args ) );
	explicit SingletonGrabber(Singleton*&& singleton) throw();//_NOEXCEPT
	explicit SingletonGrabber(const SingletonGrabber& grabber) { ++m_reference; }
	SingletonGrabber& operator = (const SingletonGrabber& singleton) { return *this; } 
	virtual ~SingletonGrabber();
	
	// Use this to get instance instead of default constructors
	virtual std::unique_ptr<Singleton>& getInstance() final;

private:
	// the instance
	static typename std::unique_ptr<Singleton> m_pInstance;
	// Thread mutex
	static struct _SINGLETON_GRABBER_MUTEX_<Singleton> m_sgmutex;
	// Grabber reference of the singleton instance
	static unsigned int m_reference;
};


template <class Singleton>
std::unique_ptr<Singleton> SingletonGrabber<Singleton>::m_pInstance = nullptr;

template <class Singleton>
unsigned int SingletonGrabber<Singleton>::m_reference = 0;

template <class Singleton>
_SINGLETON_GRABBER_MUTEX_<Singleton> SingletonGrabber<Singleton>::m_sgmutex;


template <class Singleton>
SingletonGrabber<Singleton>::SingletonGrabber() 
{
	std::lock_guard<std::mutex> lock(m_sgmutex._m_mutex);
	if ( m_pInstance == nullptr ) {
		m_pInstance = std::move( std::unique_ptr<Singleton>() );
	}
	++m_reference;
}

template <class Singleton>
SingletonGrabber<Singleton>::SingletonGrabber(Singleton*&& singleton) throw() 
{
	std::lock_guard<std::mutex> lock(m_sgmutex._m_mutex);
	if ( m_pInstance == nullptr ) {
		m_pInstance = std::move( std::unique_ptr<Singleton>( singleton ) );
		singleton = nullptr;
	}
	++m_reference;
}

template <class Singleton>
SingletonGrabber<Singleton>::~SingletonGrabber() 
{
	std::lock_guard<std::mutex> lock(m_sgmutex._m_mutex);
	--m_reference;
	if ( m_reference == 0 ) {
		m_pInstance = nullptr;
	}
}
template <class Singleton>
std::unique_ptr<Singleton>& SingletonGrabber<Singleton>::getInstance()
{
	std::lock_guard<std::mutex> lock(m_sgmutex._m_mutex);
	if (m_pInstance == nullptr) {
		m_pInstance = std::move(std::unique_ptr<Singleton>());
	} 
	
	return m_pInstance;
}