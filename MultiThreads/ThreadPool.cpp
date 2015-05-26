#include "ThreadPool.h"


ThreadPool::ThreadPool():
	ThreadPool(4, "haarcascade_eye_tree_eyeglasses.xml")
{
	// Thank god, now we can use delegate ctors.
}

ThreadPool::ThreadPool(unsigned int tNum, const std::string& cName):
	NUM_THREADS(tNum), cascadeName(cName)
{
	// Initialize the m_image
	m_image = cvCreateImage( cv::Size( 640, 480 ), 8, 1 );

	// Add threads
	for ( unsigned int i=0; i<NUM_THREADS; ++i ) {
		std::unique_ptr<ThreadController> pController( new ThreadController( cascadeName ) );
		this->m_controllerVec.push_back( std::move( pController ) );
	}
	
	// Begin running
	this->Begin();
}

ThreadPool::~ThreadPool()
{
	// Nothing need to do!
}

void ThreadPool::runLoop()
{
	int pressedKey = 0;// used to control keyboard event
	CvCapture* imageCapture;// Capture struct

    // Initialize camera
    imageCapture = cvCaptureFromCAM( 0 );
    // Check 
    assert( imageCapture );
    // Create a window named 'result' - USE IT ONLY.
    cvNamedWindow( "result", 1 );

	//int q=0;
	// If 'q' pressed terminated
	while ( pressedKey != 'q' ) {

		try {
			std::unique_ptr<ThreadController>& pController = this->nextController();
			// If excepton throwed then the following work will not be done

			// Capture image
			m_image = cvQueryFrame( imageCapture );
			//if ( !m_image ) break;

			// Set the semaphore to trigger the event
			pController->doJob();
			// Copy the frame
			pController->frame_img = m_image( cv::Rect( 170, 100, 300, 160 ) );

		} catch (const std::bad_exception&) {
			// do nothing, skip this loop
		}

		//++q;
		pressedKey = cvWaitKey( 30 );
	}

	// Terminate all
	std::for_each( m_controllerVec.begin(), m_controllerVec.end(),
		[] ( std::unique_ptr<ThreadController>& pController ) {
			pController->terminate(); } );

	// Release the memory
	cvReleaseCapture( &imageCapture );
	cvDestroyWindow( "result" );
}

std::unique_ptr<ThreadController>& ThreadPool::nextController()
{
	// returns the index as current threadController
	// if all threadController having jobs, return the null ref.
	auto iter = m_controllerVec.begin();
	for ( iter = m_controllerVec.begin(); iter != m_controllerVec.end(); ++iter ) {
		if ( !(*iter)->hasJob() ) return *iter;
	}

	throw std::bad_exception( "[Exception] Return an nullref" );
	return nullref(std::unique_ptr<ThreadController>);
}

void ThreadPool::Begin()
{
	// Call ThreadController::start method
	std::for_each( m_controllerVec.begin(), m_controllerVec.end(), 
		[] (std::unique_ptr<ThreadController>& pController) {
		pController->start(); } );
}