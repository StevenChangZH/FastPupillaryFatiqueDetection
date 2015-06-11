//
//  includes.h
//  FPFWD Project
//
//	Include this file to get all namespaces, classes and funcitons you need.
//	Including C++ standard library.
//
//  Created by Steven Chang on 15/6/5.
//  Copyright (c) 2015 Feathergames. All rights reserved.
//

#pragma once

// Prerequisites of OpenCV
#include <opencv2\core\core.hpp>
#include <opencv2\highgui\highgui.hpp>
#include <opencv2\opencv.hpp>
#include <opencv2\objdetect\objdetect.hpp>

// C++ Standard Library
#include <iostream>
#include <memory>
#include <thread>
#include <mutex>
#include <future>
#include <chrono>
#include <stdexcept>
#include <functional>
#include <utility>
// STL
#include <vector>
#include <queue>
#include <algorithm>

// Memory leak analysis - Visual Leak Detector
//#include <vld.h>

// Personal includes
#include "nullref.h"
#include "CVEx.h"

// For stl_tp suit: under /threads folder
#define _NAMESPACE_STL_THREAD_POOL_START_ namespace stl_tp{
#define _NAMESPACE_STL_THREAD_POOL_END_ }

// Templated mutex, used to create a static constant
template <typename T> struct _TEMPLATED_MUTEX_ {
	std::mutex _m_mutex;
};