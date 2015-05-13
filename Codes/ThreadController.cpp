#include "ThreadController.h"


ThreadController::ThreadController(unsigned int id, cv::CascadeClassifier cas):
	isTerminated(false), hasJob(false), id(id),
	cascade(cas)
{
	// Register the thread
	this->m_thread = std::thread( [this] { 
		this->runLoopControl(); } );
}

ThreadController::~ThreadController(void)
{
}

void ThreadController::start()
{
	// Detach!
	this->m_thread.detach();
}

unsigned int ThreadController::getId()
{
	return this->id;
}

void ThreadController::runLoopControl()
{
	// Use isTerminated to check if needed to terminate
	while ( !isTerminated ) {

		// Use hasJob to check if needed to do a job
		if ( hasJob ) {
			// Do something
			std::cout << "DO SOMETHING!! Thread: " << this->getId() << std::endl;
			//cvShowImage( "result", img );
			this->detectPupil();

			hasJob = false;
		}

		// Use this to wait for 200 ms
		Sleep( 33 );
	}
}

void ThreadController::detectPupil()
{
	int i = 0;
	double t = 0;
	std::vector<cv::Rect> faces;
	IplImage* gray = cvCreateImage(cvGetSize(img),8,1);
	cvCvtColor( img, gray, CV_BGR2GRAY );
	cvEqualizeHist( gray, gray );
	/*
	t = (double)cvGetTickCount();
	cascade.detectMultiScale( gray , faces,
		1.1, 2, 0
		//|CV_HAAR_FIND_BIGGEST_OBJECT
		//|CV_HAAR_DO_ROUGH_SEARCH
		|CV_HAAR_SCALE_IMAGE
		,
		cv::Size(30, 30) );
	t = (double)cvGetTickCount() - t;
	double t0=t/((double)cvGetTickFrequency()*1000.);
	printf( "detection time = %g ms, FPS=%g\n", t0, 1000/t0);

	for( std::vector<cv::Rect>::const_iterator r = faces.begin(); r != faces.end(); r++, i++ )
	{
		cv::Point center;
		cv::Scalar color = CV_RGB(0,128,255);
		int radius;
		//center is the coord of pupil
		center.x = cvRound(r->x + r->width*0.5);
		center.y = cvRound(r->y + r->height*0.5);
		//radius = (int)(cvRound(r->width + r->height)*0.25);
		radius = 2;
		cvCircle( img, center, radius, color, 3, 8, 0 );
		//cvShowImage( "result", img );
	}*/
	
	// Show background img
	cvShowImage( "result", img );
	// Free the memory
	cvReleaseImage( &gray );
	cvReleaseImage( &img );
}