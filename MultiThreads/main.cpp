//
//  main.cpp
//  FPFWD Project
//
//  Created by Steven Chang on 15/6/5.
//  Copyright (c) 2015 Feathergames. All rights reserved.
//

#include "includes.h"
#include "SingletonGrabber.h"
#include "PDThreadPool.h"

// Standard: only using namespace within main.cpp
using namespace std;
using namespace cv;

int main(int argc, char* argv[])
{
	auto pair = SingletonGrabber<PDThreadPool>::GetInstance();
	auto pool = pair.first;
	pool->Initialize();
	pool->runLoop();
	
	// Display all diameters data
	std::cout << std::endl << std::endl << "Left eye:" << std::endl;
	for (auto& diameterL : pool->T_lDiameterVec) {
		std::cout << diameterL << std::endl;
	}
	std::cout << std::endl << std::endl << "Right eye:" << std::endl;
	for (auto& diameterR : pool->T_lDiameterVec) {
		std::cout << diameterR << std::endl;
	}
	return 0;
}