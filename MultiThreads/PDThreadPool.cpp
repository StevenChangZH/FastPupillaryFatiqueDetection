//
//  PDThreadPool.cpp
//  FPFWD Project
//
//  Created by Steven Chang on 15/6/5.
//  Copyright (c) 2015 Feathergames. All rights reserved.
//

#include "PDThreadPool.h"


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

	// Initialize camera 
	CvCapture* imageCapture = cvCaptureFromCAM(0);
	assert(imageCapture);

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
			m_image = cvQueryFrame(imageCapture);
			//if ( !m_image ) break;

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
	cvReleaseCapture(&imageCapture);
	cvDestroyAllWindows();
}

void PDThreadPool::GetSynchronizedDataFromThread(std::chrono::system_clock::time_point& duration,
	float& lDiameter, float& rDiameter)
{
	// LOCK this
	std::lock_guard<std::mutex> lock(m_mutex);

	// get data
	T_durationVec.push_back(duration);
	T_lDiameterVec.push_back(lDiameter);
	T_rDiameterVec.push_back(rDiameter);
}