//
//  PDThreadJob.cpp
//  FPFWD Project
//
//  Created by Steven Chang on 15/6/5.
//  Copyright (c) 2015 Feathergames. All rights reserved.
//

#include "PDThreadJob.h"
#include "PDThreadPool.h"

// A static const matrix as a kernel
static const cv::Mat kernel = cv::getGaussianKernel(3, 0.05);

PDThreadJob::PDThreadJob()
{}

PDThreadJob::PDThreadJob(const std::string& cascadename, PDThreadPool* pool_)
{
	// Load the file to construct a cascade classifier
	cc_cascade.load(cascadename);
	
	// assign pool ptr
	this->poolptr = pool_;
}

PDThreadJob::~PDThreadJob()
{}

void PDThreadJob::SynchronizeData(cv::Mat& img_, std::chrono::system_clock::time_point& tp_)
{
	frame_img = img_(cv::Rect(170, 100, 300, 160));
	timepoint = tp_;
}


void PDThreadJob::Task()
{
	// Get the gray image and perform hist equalization
	std::vector<cv::Rect> eyeRectVec;
	cv::Mat frame_gray = cv::Mat(frame_img);
	cvtColor(frame_gray, frame_gray, CV_BGR2GRAY);
	equalizeHist(frame_gray, frame_gray);

	// Detect eye && count time
	long t = static_cast<long>(cvGetTickCount());
	cc_cascade.detectMultiScale(frame_gray, eyeRectVec,
		1.1, 2, 0
		//|CV_HAAR_FIND_BIGGEST_OBJECT
		//|CV_HAAR_DO_ROUGH_SEARCH
		| CV_HAAR_SCALE_IMAGE
		,
		cv::Size(30, 30));

	// Get the position
	for (std::vector<cv::Rect>::iterator r = eyeRectVec.begin();
		r != eyeRectVec.end() && (r - eyeRectVec.begin()) < 2; ++r)
	{
		r->height /= 2;
		r->y += r->height / 2;
		cv::Point lefttop(r->x, r->y);
		cv::Point rightdown(r->x + r->width, r->y + r->height);
		cv::Scalar color = CV_RGB(255, 255, 255);
		rectangle(frame_img, lefttop, rightdown, color, 1, 8, 0);
	}

	// Trying to calculate the diameter
	if (eyeRectVec.size() == 2) {
		// Perhaps we can process
		cv::Rect leftRect = eyeRectVec[0];
		cv::Rect rightRect = eyeRectVec[1];
		if (abs(leftRect.x - rightRect.x) >= 50) {
			// No rect is nested
			cv::Mat leftROI = frame_gray(leftRect);
			cv::Mat rightROI = frame_gray(rightRect);

			// Get diameter
			leftRect.x += leftRect.width/2;
			leftRect.y += leftRect.height/2;
			rightRect.x += rightRect.width/2;
			rightRect.y += rightRect.height/2;
			float distOfTwoEyes = static_cast<float>(sqrt((rightRect.y - leftRect.y)*
				(rightRect.y - leftRect.y) + (rightRect.x - leftRect.x)*
				(rightRect.x - leftRect.x)));
			float diameterR = ProcessSingleEye(rightROI);
			float diameterL = ProcessSingleEye(leftROI);

			// Return back the result
			DataLog log(timepoint, diameterL, diameterR, distOfTwoEyes);
			poolptr->GetSynchronizedDataFromThread(log);

			rightROI.release();
			leftROI.release();
		}
	}

	t = static_cast<long>(cvGetTickCount()) - t;
	double t0 = static_cast<double>(t / static_cast<long>((cvGetTickFrequency()*1000.)));
	// Output - call this will make the std::cout not synchronized
	std::cout << "Processing time = " << t0 << " ms, avg thread FPS= " <<
		1000 / t0 << ". Catch: " << eyeRectVec.size() << std::endl;


	// Show img
	imshow("result", frame_img);
	// Free the memory
	frame_gray.release();
	frame_img.release();
}


float PDThreadJob::ProcessSingleEye(cv::Mat& eyeROI)
{
	// Preprocessing
	// Eliminate catchlights
	cvEx::mat_foreach<uchar>(eyeROI, [](uchar& c, int) {
		if (c > 130)c = 20;
	});
	morphologyEx(eyeROI, eyeROI, CV_MOP_OPEN, kernel);
	//imshow("result", eyeGrayAdjusted);

	// Critical of pupilla intensity
	int criticalValue = 18;

	// Split center lines and get chords

	cv::Point2i lPoint, rPoint;
	int horLine = static_cast<int>(eyeROI.rows / 2);
	lPoint.y = horLine;
	rPoint.y = horLine;
	cv::Mat row(eyeROI.row(horLine));
	std::vector<uchar> rowDataVec1;
	cvEx::mat_foreach(row, [&rowDataVec1](uchar& c, int){
		rowDataVec1.push_back(c); });
	DetectChord_Hor(rowDataVec1, lPoint, rPoint, criticalValue);
	int verLine = static_cast<int>((rPoint.x + lPoint.x) / 2);

	std::vector<uchar> colDataVec1;
	int eyeGrayAdjustedCols = eyeROI.cols;
	cvEx::mat_foreach(eyeROI, [&colDataVec1, &verLine, &eyeGrayAdjustedCols]
		(uchar& c, int num){
		if (((num + 1) % eyeGrayAdjustedCols) == (verLine%eyeGrayAdjustedCols)) {
			colDataVec1.push_back(c);
		}});
	cv::Point2i lowerPoint(verLine, horLine);
	DetectChord_Ver(colDataVec1, lowerPoint, criticalValue);

	float temp_a = static_cast<float>(rPoint.x - verLine);
	float radius = (pow(static_cast<float>(lowerPoint.y - horLine), 2) + pow(temp_a, 2))
		/ 2 / temp_a;
	cv::Point2f center((float)verLine, (float)lowerPoint.y - radius);
	/*
	cv::circle(eyeROI, center, (int)radius, CV_RGB(255, 255, 255));
	cv::line(eyeROI, lPoint, rPoint, CV_RGB(255, 255, 255));
	cv::line(eyeROI, center, lowerPoint, CV_RGB(0, 0, 255));
	imshow("NewWindow", eyeROI);
	*/

	row.release();
	return radius;
}


void PDThreadJob::DetectChord_Hor(std::vector<uchar>& dataVecGray, cv::Point2i& lPoint,
	cv::Point2i& rPoint, int criticalValue)
{
	// Find the pupillary Chord
	// Obtain the uchar data of this row

	// initialization
	auto lit = dataVecGray.rbegin() + static_cast<int>(dataVecGray.size() / 2);
	auto rit = lit.base();
	int centerIntensity = static_cast<int>(*lit);
	int lBound = centerIntensity - criticalValue;
	int uBound = centerIntensity + criticalValue;
	auto bindLogicalOp = [&lBound, &uBound](uchar& val) {
		return uBound < val || val < lBound; };

	// Left bounds
	auto lbit = std::find_if(lit, dataVecGray.rend(), bindLogicalOp);
	// Right bounds
	auto rbit = std::find_if(rit, dataVecGray.end(), bindLogicalOp);

	lPoint.x = lbit.base() - dataVecGray.begin();
	rPoint.x = rbit - dataVecGray.begin();
}

void PDThreadJob::DetectChord_Ver(std::vector<uchar>& colDataVec, cv::Point2i& lowerPoint, int criticalValue)
{
	// Find the lower bound of the point
	// Obtain the uchar data of this col

	// initialization
	auto centerIter = colDataVec.begin() + lowerPoint.y;
	int centerIntensity = static_cast<int>(*centerIter);
	int lBound = centerIntensity - criticalValue;
	int uBound = centerIntensity + criticalValue;
	auto bindLogicalOp = [&lBound, &uBound](uchar& val) {
		return uBound < val || val < lBound; };

	// Lower bounds
	auto lowerIter = std::find_if(centerIter, colDataVec.end(), bindLogicalOp);
	lowerPoint.y = lowerIter - colDataVec.begin();
}