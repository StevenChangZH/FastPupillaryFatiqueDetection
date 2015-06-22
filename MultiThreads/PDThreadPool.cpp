//
//  PDThreadPool.cpp
//  FPFWD Project
//
//  Created by Steven Chang on 15/6/5.
//  Copyright (c) 2015 Feathergames. All rights reserved.
//

#include "PDThreadPool.h"

// Data set size 
const unsigned dataSize = 1200;
const unsigned resultSize = 1000;

PDThreadPool::PDThreadPool()
{}

PDThreadPool::~PDThreadPool()
{}

void PDThreadPool::Initialize()
{
	// Initialize the ThreadController
	m_controllerVec.reserve(NUM_THREADS);
	for (unsigned int i = 0; i < NUM_THREADS; ++i) {
		auto pController = std::make_unique<PDThreadController>(this);
		pController->Initialize();
		this->m_controllerVec.push_back(std::move(pController));
	}

	// Turn on the pool
	this->Begin();
}

void PDThreadPool::runLoop()
{
	int pressedKey = 0;// used to record keyboard event

	// Initialize capture struct
	cv::VideoCapture imageCapture(0);//camera 
	//cv::VideoCapture imageCapture("");// Open sample video
	if (!imageCapture.isOpened()) {
		std::cerr << "[ERROR] Error occurred when trying to open the video file." << std::endl;
		return;
	}

	// Image captured from imageCapture
	cv::Mat m_image;

	// Create a window named 'result' - USE IT ONLY.
	cvNamedWindow("result", 1);

	// If 'q' pressed terminated
	while (pressedKey != 'q') {

		try {
			std::unique_ptr<PDThreadController>& pController = this->nextController();
			// If excepton throwed then the following work will not be done
			
			// Capture image
			imageCapture >> m_image;

			// Copy the frame first
			pController->SynchronizeData(m_image, std::chrono::system_clock::now());
			// Set the semaphore to trigger the event
			pController->doJob();

		} catch (const std::bad_exception&) {
			// do nothing, skip this loop
		}

		pressedKey = cvWaitKey(16);
	}

	// Terminate all
	std::for_each(m_controllerVec.begin(), m_controllerVec.end(),
		[](std::unique_ptr<PDThreadController>& pController) {
		pController->terminate(); });

	// Release the memory
	m_image.release();
	cvDestroyAllWindows();
}

void PDThreadPool::GetSynchronizedDataFromThread(DataLog& log_)
{
	// LOCK this
	std::lock_guard<std::mutex> lock(m_mutex);

	// get data
	m_logVec.push_back(std::move(log_));
}

void PDThreadPool::analyze() 
{
	// Wait for other threads to terminate; though those data points will make
	// no large difference.
	cvWaitKey(100);
	// Try lock
	std::lock_guard<std::mutex> lock(m_mutex);
	long t = static_cast<long>(cvGetTickCount());

	// First, interpolations and adjustments.
	std::array<float, dataSize> dataArrayL;
	std::array<float, dataSize> dataArrayR;



	// Second, FFT.
	auto dftDim1 = [](std::array<float, dataSize>& dataArray) 
		->std::array<float, resultSize> {

		// Use resultSize instead of dataSize to get data distributed in center 
		// with low bias.
		std::array<float, resultSize> dst;
		
		float unitStride = 2 * static_cast<float>(M_PI) / static_cast<float>(dataSize);
		float stride = unitStride * 100;
		for (unsigned k = 0; k<resultSize; ++k){
			float tmp = 0.0f;

			float angle = 0.0f;
			for (unsigned n = 0; n<dataSize; ++n) {
				// Only the real section of the result is needed
				// So here the FFT is optimized.
				float c = cos(angle);
				tmp += dataArray[n] * c;
				angle += stride;
			}
			dst[k] = tmp;
			stride += unitStride;
		}
		return dst;
	};

	std::array<float, resultSize> fftArrayL = dftDim1(dataArrayL);
	std::array<float, resultSize> fftArrayR = dftDim1(dataArrayR);

	// Third, the variance.
	auto getVariance = [](const std::array<float, resultSize> fftArray) {
		
		// Based on formula: Variance = E(x^2) - (Ex)^2
		float E_xsquared = 0.0f;// E(x^2)
		float Ex_squared = 0.0f;//(Ex)^2
		for (const float& val : fftArray) {
			E_xsquared += val*val;
			Ex_squared += val;
		}
		E_xsquared /= resultSize;
		Ex_squared /= resultSize;
		Ex_squared *= Ex_squared;
		return E_xsquared - Ex_squared;
	};
	
	float varianceL = getVariance(fftArrayL);
	float varianceR = getVariance(fftArrayR);

	// Evaluations of the quantifiable fatique value - PUI

	// This data is used as a standard
	const float std = 20.0f;

	float PUI = 1 - (((varianceL + varianceR) / 2 - std) /
		max(abs(varianceL - std), abs(varianceR - std)));
	std::cout << "The PUI value detected is:" << PUI << std::endl;
	t = static_cast<long>(cvGetTickCount()) - t;
	double t0 = static_cast<double>(t / static_cast<long>((cvGetTickFrequency()*1000.)));
	std::cout << "[STATE] Process completed. Total analysis time: " << t0 << " ms." << std::endl;
	cvWaitKey();
}