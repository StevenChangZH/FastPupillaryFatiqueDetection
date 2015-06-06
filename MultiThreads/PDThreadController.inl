//
//  PDThreadController.inl
//  FPFWD Project
//
//  Created by Steven Chang on 15/6/5.
//  Copyright (c) 2015 Feathergames. All rights reserved.
//

#pragma once
#include "PDThreadController.h"


PDThreadController::PDThreadController()
{}

PDThreadController::~PDThreadController()
{}

void PDThreadController::SynchronizeData(cv::Mat& img_)
{
	m_job->SynchronizeData(img_);
}
