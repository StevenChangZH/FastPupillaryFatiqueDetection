#include "ThreadController.h"


ThreadController::ThreadController():
	ThreadController("haarcascade_eye_tree_eyeglasses.xml")
{}

ThreadController::ThreadController(const std::string& s_cascade):
	m_hasJob(false), m_isTerminated(false)
{
	// Initialize the job
	m_job = std::make_unique<ThreadJob>(s_cascade);

	// Register the thread
	this->m_thread = std::thread( [this] { 
		this->runLoop(); } );
}

ThreadController::~ThreadController(void)
{
	m_job = nullptr;
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
			// Maybe you can see it as a 'tick'
			// Do something
			//std::function<void(float&, float&)> method = [](float&, float&){};
			m_job->Task();

			// Set the semaphore back
			m_hasJob = false;
		}

		// Sleep for a few ms
		Sleep( 33 );
	}
}

void ThreadController::SynchronizeData(cv::Mat& img_)
{
	m_job->SynchronizeData(img_);
}

void ThreadController::doJob()
{ m_hasJob = true; }

bool ThreadController::hasJob() const
{ return m_hasJob; }

void ThreadController::terminate()
{ m_isTerminated = true; }

bool ThreadController::isTerminated() const
{ return m_isTerminated; }