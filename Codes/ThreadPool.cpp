#include "ThreadPool.h"


void ThreadPool::start()
{
	// Call ThreadController::start method
	std::for_each( m_controllerVec.begin(), m_controllerVec.end(), 
		[] (ThreadController* pController) {
		pController->start(); } );
}

void ThreadPool::runLoop()
{
	int pressedKey = 0;// used to control keyboard event
	CvCapture *imageCapture;// Capture struct

    // Initialize camera
    imageCapture = cvCaptureFromCAM( 0 );
    // Check 
    assert( imageCapture );
    // Create a window named 'result' - USE IT ONLY.
    cvNamedWindow( "result", 1 );

	int q=0;
	// If 'q' pressed terminated
	while ( pressedKey != 'q' ) {

		// Run
		ThreadController* pController = this->checkFreedom();
		if ( pController != nullptr ) {
			// not a nullptr, do something
			// Capture image
			m_image = cvQueryFrame( imageCapture );
			//if ( !m_image ) break;

			// Set the semaphore to trigger the event
			pController->hasJob = true;
			// Copy the frame
			pController->frame_img = m_image( cv::Rect( 120, 100, 400, 200) );
		}
		++q;
		pressedKey = cvWaitKey( 30 );
	}

	// Terminate all
	std::for_each( m_controllerVec.begin(), m_controllerVec.end(),
		[] ( ThreadController* pController ) {
			pController->isTerminated = true; } );

	// Release the memory
	cvReleaseCapture( &imageCapture );
	cvDestroyWindow( "result" );
}

ThreadPool::ThreadPool(void):
	cascadeName("haarcascade_eye_tree_eyeglasses.xml")
{
	// Initialize the m_image
	m_image = cvCreateImage( cv::Size( 640, 480 ), 8, 1 );

	// 4 threads as default
	for ( unsigned int i=0; i<MAX_THREAD_NUM; ++i ) {
		ThreadController* pController = new ThreadController( i, cascadeName );
		this->m_controllerVec.push_back( pController );
	}
}

ThreadPool::~ThreadPool(void)
{
	// Delete and release controllers
	std::for_each( m_controllerVec.begin(), m_controllerVec.end(),
		[] ( ThreadController* pController ) {
			delete pController; pController = nullptr; } );
	m_controllerVec.clear();
}

ThreadController* ThreadPool::checkFreedom() 
{
	// returns the ptr as current threadController
	// if all threadController having jobs, return nullptr.
	std::vector<ThreadController*>::iterator iter;
	for ( iter = m_controllerVec.begin(); iter != m_controllerVec.end(); ++iter ) {
		ThreadController* pController = (ThreadController*)*iter;
		if ( !pController->hasJob ) return pController;
	}
	return nullptr;
}