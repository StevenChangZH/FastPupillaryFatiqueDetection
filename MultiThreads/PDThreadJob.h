//
//  PDThreadJob.h
//  FPFWD Project
//
//	Thread job in this project
//
//  Created by Steven Chang on 15/6/5.
//  Copyright (c) 2015 Feathergames. All rights reserved.
//

#pragma once
#include "Threads\AbstractThreadJob.h"
#include "SingletonGrabber.h"
#include "Log.h"

class PDThreadPool;

class PDThreadJob : public stl_tp::AbstractThreadJob {
public:
	PDThreadJob();
	PDThreadJob(const std::string& cascadename);
	virtual ~PDThreadJob();

	// Called before Task() method to exchange data
	void SynchronizeData(cv::Mat& img_, std::chrono::system_clock::time_point& tp_);

	// Task method, used in a tick
	virtual void Task() override;

	template <class FUNC_>
	// Task method with a function callback
	void Task(FUNC_& callback){}

protected:
	////////////////////// OpenCV Variables ///////////////////////

	// color image of processing
	cv::Mat frame_img;
	// Cascade classifier
	cv::CascadeClassifier cc_cascade;

	////////////////////// OpenCV Processing Methods ////////////////////

	cv::Mat constructSemiCircleKernel(unsigned width);
	float ProcessingSingleEye(cv::Mat& eyeROI);

	////////////////////// Protected Variables ////////////////////

	// Time duration
	std::chrono::system_clock::time_point timepoint;
};
