#include "ThreadController.h"


ThreadController::ThreadController(unsigned int id, const std::string &cascadeName):
	isTerminated(false), hasJob(false), id(id)
{
	// Load the file to construct a cascade
	cv_cascade.load( cascadeName );

	// Register the thread
	this->m_thread = std::thread( [this] { 
		this->runLoopControl(); } );
}

ThreadController::~ThreadController(void)
{
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
			this->detectPupil();

			// Set the semaphore back
			hasJob = false;
		}

		// Use this to wait for 200 ms
		Sleep( 33 );
	}
}

ThreadController::ThreadController():id(0)
{
	// Default params
	ThreadController( 0, std::string( "haarcascade_eye_tree_eyeglasses.xml" ) );
}

void ThreadController::start()
{
	// Detach!
	this->m_thread.detach();
}

void ThreadController::detectPupil()
{
	// Get the gray image and perform hist equalization
	std::vector<cv::Rect> eyes;
	cv::Mat frame_gray = cv::Mat( frame_img );
	cvtColor( frame_gray, frame_gray, CV_BGR2GRAY );
	equalizeHist( frame_gray, frame_gray );
	
	// Detect eyes && get time
	double t = (double)cvGetTickCount();
	cv_cascade.detectMultiScale( frame_gray , eyes,
		1.1, 2, 0
		//|CV_HAAR_FIND_BIGGEST_OBJECT
		//|CV_HAAR_DO_ROUGH_SEARCH
		|CV_HAAR_SCALE_IMAGE
		,
		cv::Size(30, 30) );
	t = (double)cvGetTickCount() - t;
	double t0=t/((double)cvGetTickFrequency()*1000.);
	// Output
	std::cout << "Thread: " << this->getId() <<
		". Detection time = " << t0 << " ms, avg thread FPS= " <<
		1000/t0 << ". Eyes: " << eyes.size() << std::endl;
	
	// Calculate the pos
	for( std::vector<cv::Rect>::const_iterator r = eyes.begin(); r != eyes.end(); r++ )
	{
		cv::Point center;
		cv::Scalar color = CV_RGB(0,128,255);
		int radius;
		//center is the coord of pupil
		center.x = cvRound(r->x + r->width*0.5);
		center.y = cvRound(r->y + r->height*0.5);
		//radius = (int)(cvRound(r->width + r->height)*0.25);
		radius = 2;
		circle( frame_img, center, radius, color, 3, 8, 0 );
	}
	
	// Show img
	imshow( "result", frame_img );
	// Free the memory
	frame_gray.release();
	frame_img.release();
}
