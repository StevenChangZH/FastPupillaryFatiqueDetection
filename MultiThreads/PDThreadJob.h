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
#include "Log.h"

class PDThreadPool;

class PDThreadJob : public stl_tp::AbstractThreadJob {
public:
	PDThreadJob();
	PDThreadJob(const std::string& cascadename, PDThreadPool* pool_);
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

	////////////////////// Processing Methods ////////////////////

	inline float ProcessSingleEye(cv::Mat& eyeROI);
	inline void DetectChord_Hor(std::vector<uchar>& dataVecGray, cv::Point2i& lPoint,
		cv::Point2i& rPoint, int criticalValue);
	inline void DetectChord_Ver(std::vector<uchar>& dataVec, cv::Point2i& lowerPoint, int criticalValue);

	////////////////////// Protected Variables ////////////////////

	// Set the Pool ptr explicitly. DO not try to release it.
	PDThreadPool* poolptr;
	// Time duration
	std::chrono::system_clock::time_point timepoint;
};
