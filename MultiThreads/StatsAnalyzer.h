//
//  StatsAnalyzer.h
//  FPFWD Project
//
//	Each thread pool has a Stats Analyzer to generate data from threadcontrollers
//	and perform proceding analysis (for this project, we need to perform FFT for 
//	pupillary diameters)
//
//  Created by Steven Chang on 15/6/5.
//  Copyright (c) 2015 Feathergames. All rights reserved.
//

#pragma once
#include "includes.h"

class StatsAnalyzer
{
public:
	StatsAnalyzer();
	virtual ~StatsAnalyzer();
};

