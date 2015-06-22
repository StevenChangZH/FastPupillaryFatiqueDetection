//
//  Log.h
//  FPFWD Project
//
//	Used to pass a set of values from ThreadJob to the Pool.
//
//  Created by Steven Chang on 15/6/14.
//  Copyright (c) 2015 Feathergames. All rights reserved.
//

#pragma once
#include "includes.h"

struct DataLog {
	std::chrono::system_clock::time_point timepoint;
	float diameterL;
	float diameterR;
	float eyeDist;

	DataLog(std::chrono::system_clock::time_point timepoint_, float diameterL_, float diameterR_,
		float eyeDist_) :timepoint(timepoint_), diameterL(diameterL_), diameterR(diameterR_),
		eyeDist(eyeDist_){}
};

