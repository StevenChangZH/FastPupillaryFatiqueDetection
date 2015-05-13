#include "stdafx.h"
#include "SingletonGrabber.h"
#include "ThreadPool.h"

// Notice: only use using in this file
using namespace std;
using namespace cv;

int main(int argc, char* argv[])
{
	SingletonGrabber<ThreadPool> grabber;
	ThreadPool* pool = grabber.getInstance();
	pool->start();
	pool->runLoop();
	return 0;
}