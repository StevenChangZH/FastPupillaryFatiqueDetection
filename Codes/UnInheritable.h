#include "stdafx.h"
#pragma once

#ifndef _UNINHERITABLE_
#define _UNINHERITABLE_

class ThreadPool;
class ThreadController;

template<typename T>
// Used to assure class cannot be inherited 
class Uninheritable
{
	// Set friend
	friend T;

private:
	Uninheritable(void) {}
	Uninheritable(const Uninheritable*) {}
	Uninheritable* operator=(const Uninheritable*) {}
	~Uninheritable(void) {}
};

#endif