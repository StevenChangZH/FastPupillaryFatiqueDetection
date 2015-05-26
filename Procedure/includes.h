#pragma once
// Include this file to get all namespaces, classes and funcitons you need.
// Including C++ standard library

// Prerequisites of OpenCV
#include <opencv2\core\core.hpp>
#include <opencv2\highgui\highgui.hpp>
#include <opencv2\opencv.hpp>
#include <opencv2\objdetect\objdetect.hpp>

// C++ Standard Library
#include <iostream>
#include <memory>
#include <thread>
#include <stdexcept>
// STL
#include <vector>

// Windows headers
#include <windows.h>

// Memory leak analysis - Visual Leak Detector
//#include <vld.h>



// Global defines

// a null ref same as nullptr
// NEVER use comparison operator, use exception if returns an nullref.
#define nullref(type) (*((type*)(nullptr)))