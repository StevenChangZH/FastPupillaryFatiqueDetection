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
// Get only points distributed in center
const unsigned resultSize = 1000;
// IN the final data processing, the time stride of one tick we set to perform interpolation
const unsigned tickTimeStride = 25;
// The standard diameter variance after FFT performed
const float stdVariance = 2500;

PDThreadPool::PDThreadPool()
{}

PDThreadPool::~PDThreadPool()
{}

void PDThreadPool::Initialize()
{
	// Initialize the ThreadController
	m_controllerVec.reserve(NUM_THREADS);
	for (unsigned int i = 0; i < NUM_THREADS; ++i) {
		auto pController = std::make_unique<PDThreadController>();
		pController->Initialize();
		this->m_controllerVec.push_back(std::move(pController));
	}

	// Turn on the pool
	this->Begin();
}

void PDThreadPool::runLoop()
try {
	int pressedKey = 0;// used to record keyboard event

	// Initialize capture struct
	//cv::VideoCapture imageCapture(0);//camera 
	cv::VideoCapture imageCapture("c:\\software\\Giveaway.mp4");// Open sample video
	if (!imageCapture.isOpened()) {
		std::cerr << "[ERROR] Error occurred when trying to open the video file." << std::endl;
		return;
	}

	// Image captured from imageCapture
	cv::Mat m_image;

	// Create a window named 'result' - USE IT ONLY.
	cvNamedWindow("result", 1);

	// If 'q' pressed terminated
	while (pressedKey != 'q' && m_logVec.size()<=dataSize) {

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

		pressedKey = cvWaitKey(33);
	}

	// Terminate all
	std::for_each(m_controllerVec.begin(), m_controllerVec.end(),
		[](std::unique_ptr<PDThreadController>& pController) {
		pController->terminate(); });

	// Release the memory
	m_image.release();
	cvDestroyAllWindows();
} catch (const std::exception& e) {
	std::cerr << e.what() << std::endl;
	throw;
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

	unsigned logNumber = m_logVec.size();
	logNumber = (logNumber > dataSize) ? dataSize : logNumber;
	std::array<float, dataSize> dataArrayL;
	dataArrayL.fill(0.0f);
	std::array<float, dataSize> dataArrayR;
	dataArrayR.fill(0.0f);
	// 1.1 step one: quantification.
	auto logIt = m_logVec.begin();
	auto startTime = logIt->timepoint;
	for (++logIt; logIt != m_logVec.end(); ++logIt) {
		auto duration = logIt->timepoint - startTime;
		unsigned durationtime = static_cast<unsigned>(std::chrono::duration_cast
			<std::chrono::milliseconds>(duration).count());
		unsigned index = static_cast<unsigned>(durationtime / tickTimeStride);
		index = (index < dataSize) ? index : dataSize - 1;
		//dataArrayL[index] = logIt->diameterL;
		dataArrayR[index] = logIt->diameterR;
	}
	// 1.2 Step two: linear interpolation

	// Define a function used to perform interpolation. 
	auto interpolate = [](std::array<float, dataSize>& dataArray) {
		
		auto istart = dataArray.begin() + 1;
		auto iend = dataArray.end() - 1;
		for (auto it = istart; it != iend; ++it) {
			if (*it == 0.0f) {
				auto lit = it - 1;
				float sections = 1.0f;
				for (; (*it != 0.0f && it != iend); ++sections, ++it) {}
				auto rit = it;
				// Linear interpolation
				float stride = (*rit - *lit) / sections;
				float value = *lit + stride;
				for (auto iter = lit + 1; iter != rit; ++iter) {
					*iter = value;
					value += stride;
				}
			}
		}
	};
	//interpolate(dataArrayL);
	interpolate(dataArrayR);


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

	//std::array<float, resultSize> fftArrayL = dftDim1(dataArrayL);
	std::array<float, resultSize> fftArrayR = dftDim1(dataArrayR);

	// Third, the variance.
	auto getVariance = [](const std::array<float, resultSize>& fftArray) {
		
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
	
	//float varianceL = getVariance(fftArrayL);
	float varianceR = getVariance(fftArrayR);

	// Finally, evaluations of the quantifiable fatique value - PUI
	// Also known as Pupillary Unrest Index.
	float PUI = ((varianceR - stdVariance) / max(varianceR, stdVariance));
	std::cout << "The PUI value detected is:" << PUI * 100 << "%"<< std::endl;
	t = static_cast<long>(cvGetTickCount()) - t;
	double t0 = static_cast<double>(t / static_cast<long>((cvGetTickFrequency()*1000.)));
	std::cout << "[STATE] Process completed. Total analysis time: " << t0 << " ms." << std::endl;
	//cvWaitKey();
}