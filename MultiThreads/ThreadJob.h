#pragma once
#include "includes.h"

// Thread job class, each ThreadController has a ThreadJob and do Task()  
// method constantly.
class ThreadJob {
public:
	ThreadJob();
	ThreadJob(const std::string&);
	virtual ~ThreadJob();

	// Called before Task() method to exchange data - modify this method
	// use static_cast to call the real method you want
	void SynchronizeData(cv::Mat&);

	// Task method, used in a tick
	void Task();

	template <class FUNC_>
	// Task method with a function callback
	void Task(FUNC_& callback){}

protected:
	////////////////////// OpenCV Variables ///////////////////////

	// color image of processing
	cv::Mat frame_img;
	// Cascade classifier
	cv::CascadeClassifier cc_cascade;

	////////////////////// Processing Methods ////////////////////

	float ProcessSingleEye(cv::Mat& eyeROI);
	float DetectChord(cv::Mat& row, const int& criticalValue = 18);
	float CalculateDiameter(float c1, float c2, float c3, float dx);
};

