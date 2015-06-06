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

// Standards: only using namespace within main.cpp
using namespace std;
using namespace cv;

int main(int argc, char* argv[])
{
	SingletonGrabber<PDThreadPool<>> grabber;
	std::unique_ptr<PDThreadPool<>>& pool = grabber.GetInstance();
	pool->runLoop();
	return 0;
}