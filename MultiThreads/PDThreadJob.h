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

class PDThreadJob : public stl_tp::AbstractThreadJob {
public:
	PDThreadJob();
	PDThreadJob(const std::string& cascadename);
	virtual ~PDThreadJob();

	// Called before Task() method to exchange data
	void SynchronizeData(cv::Mat& img_);

	// Task method, used in a tick
	virtual void Task();

	template <class FUNC_>
	// Task method with a function callback
	void Task(FUNC_& callback){}

protected:
	////////////////////// OpenCV Variables ///////////////////////

	// color image of processing
	cv::Mat frame_img;
	// Cascade classifier
	cv::CascadeClassifier cc_cascade;

	////////////////////// Processing Methods ////////////////////

	float ProcessSingleEye(cv::Mat& eyeROI);
	float DetectChord(cv::Mat& row, const int& criticalValue = 18);
	float CalculateDiameter(float c1, float c2, float c3, float dx);
};

#include "PDThreadJob.inl"