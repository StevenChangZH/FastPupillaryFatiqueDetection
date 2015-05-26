#include "includes.h"
#include "SingletonGrabber.h"
#include "ThreadPool.h"

// Standards: only using namespace within main.cpp
using namespace std;
using namespace cv;

int main(int argc, char* argv[])
{
	SingletonGrabber<ThreadPool> grabber(new ThreadPool());
	std::unique_ptr<ThreadPool>& pool = grabber.getInstance();
	pool->runLoop();
	return 0;
}