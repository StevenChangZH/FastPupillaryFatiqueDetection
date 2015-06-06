//
//  PDThreadController.h
//  FPFWD Project
//
//	thread controller of this project
//
//  Created by Steven Chang on 15/6/5.
//  Copyright (c) 2015 Feathergames. All rights reserved.
//

#pragma once
#include "ContinuousController.h"
#include "PDThreadJob.h"

// thread controller of this project
class PDThreadController : public stl_tp::ContinuousController<PDThreadJob> {
public:
	PDThreadController();
	virtual ~PDThreadController();

	// Called before Task() method to exchange data
	void SynchronizeData(cv::Mat& img_);

	//// pre-tick callback - use capture list
	//virtual void PreCallback(std::function<void(void)>&) = 0;
	//// post-tick callback - use capture list
	//virtual void PostCallback(std::function<void(void)>&) = 0;

};

#include "PDThreadController.inl"
