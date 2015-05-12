#include "ThreadPool.h"


ThreadPool* ThreadPool::instance = nullptr;
ThreadPool* ThreadPool::getInstance()
{
	// NOTICE: not safe for multi-threads scenarios
	// UNFINSHED
	if ( instance == nullptr ) {
		instance = new ThreadPool();
	}
	
	return instance;
}

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
    // Create a window named 'result'
    cvNamedWindow( "result", 1 );

	// If 'q' pressed terminated
	while ( pressedKey != 'q' ) {

		// Run
		ThreadController* pController = this->checkFreedom();
		if ( pController != nullptr ) {
			// not a nullptr, do something
			// Capture image
			m_image = cvQueryFrame( imageCapture );
			// Set the semaphore
			pController->hasJob = true;
			// Copy the frame
			pController->img = cvCloneImage( m_image );
		}

		pressedKey = cvWaitKey( 100 );
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
	// Load the file to construct a cascade
	m_cascade.load( cascadeName );
    // Setup the memory buffer needed by detector
    m_memStorage = cvCreateMemStorage( 0 );
    // Always check 
    assert( &m_cascade && m_memStorage );

	// 4 threads as default
	for ( unsigned int i=0; i<MAX_THREAD_NUM; ++i ) {
		ThreadController* pController = new ThreadController( i );
		this->m_controllerVec.push_back( pController );
	}
}

ThreadPool::~ThreadPool(void)
{
	cvReleaseData( m_image );
	// Release the memstorage
    cvReleaseMemStorage( &m_memStorage );

	// Delete and release controllers
	std::for_each( m_controllerVec.begin(), m_controllerVec.end(),
		[] ( ThreadController* pController ) {
			delete pController; pController = nullptr; } );
	// Pop all null ptr
	for ( unsigned int i=0; i<MAX_THREAD_NUM; ++i ) {
		m_controllerVec.pop_back();
	}
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