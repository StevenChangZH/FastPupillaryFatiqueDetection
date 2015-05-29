#include "ThreadController.h"


ThreadController::ThreadController():
	ThreadController("haarcascade_eye_tree_eyeglasses.xml")
{}

ThreadController::ThreadController(const std::string& s_cascade):
	m_hasJob(false), m_isTerminated(false)
{
	// Load the file to construct a cascade classifier
	classifier.load( s_cascade );

	// Register the thread
	this->m_thread = std::thread( [this] { 
		this->runLoop(); } );
}

ThreadController::~ThreadController(void)
{
}

void ThreadController::start()
try {
	if ( m_thread.joinable() ) {
		// Yes, detach but join
		this->m_thread.detach();
	} else {
		throw std::runtime_error( "[EXCEPTION] Attempt to recall detach method for a unjoinable thread" );
	}
} catch (const std::runtime_error& excp) {
	// Attempt to recall the detach method
	std::cerr << excp.what() << std::endl;
	throw;
}

void ThreadController::runLoop()
{
	while ( !m_isTerminated ) {
		if ( m_hasJob ) {
			// Do something
			this->tick();

			// Set the semaphore back
			m_hasJob = false;
		}

		// Sleep for a few ms
		Sleep( 33 );
	}
}

void ThreadController::tick()
{
	// Get the gray image and perform hist equalization
	std::vector<cv::Rect> eyeRectVec;
	cv::Mat frame_gray = cv::Mat( frame_img );
	cvtColor( frame_gray, frame_gray, CV_BGR2GRAY );
	equalizeHist( frame_gray, frame_gray );
	
	// Detect eye && get time
	double t = (double)cvGetTickCount();
	classifier.detectMultiScale( frame_gray , eyeRectVec,
		1.1, 2, 0
		//|CV_HAAR_FIND_BIGGEST_OBJECT
		//|CV_HAAR_DO_ROUGH_SEARCH
		|CV_HAAR_SCALE_IMAGE
		,
		cv::Size(30, 30) );
	t = (double)cvGetTickCount() - t;
	double t0=t/((double)cvGetTickFrequency()*1000.);
	// Output
	std::cout << "Detection time = " << t0 << " ms, avg thread FPS= " <<
		1000/t0 << ". eyeRectVec: " << eyeRectVec.size() << std::endl;
	
	// Calculate the pos
	for( std::vector<cv::Rect>::iterator r = eyeRectVec.begin(); 
		r != eyeRectVec.end() && ( r - eyeRectVec.begin() ) < 2; ++r )
	{
		r->height /= 2;
		r->y += r->height/2;
		cv::Point lefttop( r->x, r->y );
		cv::Point rightdown( r->x+r->width, r->y+r->height );
		cv::Scalar color = CV_RGB( 255, 255, 255 );
		rectangle( frame_img, lefttop, rightdown, color, 1, 8, 0 );
	}
	
	// Show img
	imshow( "result", frame_img );
	// Free the memory
	frame_gray.release();
	frame_img.release();
}

void ThreadController::doJob()
{ m_hasJob = true; }

bool ThreadController::hasJob() const
{ return m_hasJob; }

void ThreadController::terminate()
{ m_isTerminated = true; }

bool ThreadController::isTerminated() const
{ return m_isTerminated; }