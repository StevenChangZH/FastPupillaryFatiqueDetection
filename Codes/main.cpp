#include "stdafx.h"
#include "ThreadPool.h"

// Notice: only use using in this file
using namespace std;
using namespace cv;
 
void DetectAndDraw(IplImage* img);

int main(int argc, char** argv[])
{
	ThreadPool* pool = ThreadPool::getInstance();
	pool->start();
	pool->runLoop();
    return 0;
}