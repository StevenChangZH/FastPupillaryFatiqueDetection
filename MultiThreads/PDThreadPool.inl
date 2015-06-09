//
//  PDThreadPool.inl
//  FPFWD Project
//
//  Created by Steven Chang on 15/6/5.
//  Copyright (c) 2015 Feathergames. All rights reserved.
//

#pragma once
#include "PDThreadPool.h"

template <size_t NUM_THREADS>
PDThreadPool<NUM_THREADS>::PDThreadPool()
{}

template <size_t NUM_THREADS>
PDThreadPool<NUM_THREADS>::~PDThreadPool()
{}

template <size_t NUM_THREADS>
void PDThreadPool<NUM_THREADS>::runLoop()
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
			pController->SynchronizeData(m_image);
			// Set the semaphore to trigger the event
			pController->doJob();

		} catch (const std::bad_exception&) {
			// do nothing, skip this loop
		}

		pressedKey = cvWaitKey(30);
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
