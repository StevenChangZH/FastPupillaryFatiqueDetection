//
//  PDThreadJob.inl
//  FPFWD Project
//
//  Created by Steven Chang on 15/6/5.
//  Copyright (c) 2015 Feathergames. All rights reserved.
//

#include "PDThreadJob.h"

// A static const matrix as a kernel
static const cv::Mat kernel = cv::getGaussianKernel(3, 0.05);



PDThreadJob::PDThreadJob() :
PDThreadJob("haarcascade_eye_tree_eyeglasses.xml")
{}

PDThreadJob::PDThreadJob(const std::string& cascadename)
{
	// Load the file to construct a cascade classifier
	cc_cascade.load(cascadename);
}

PDThreadJob::~PDThreadJob()
{}

void PDThreadJob::SynchronizeData(cv::Mat& img_)
{
	frame_img = img_(cv::Rect(170, 100, 300, 160));
}


void PDThreadJob::Task()
{
	// Get the gray image and perform hist equalization
	std::vector<cv::Rect> eyeRectVec;
	cv::Mat frame_gray = cv::Mat(frame_img);
	cvtColor(frame_gray, frame_gray, CV_BGR2GRAY);
	equalizeHist(frame_gray, frame_gray);

	// Detect eye && count time
	double t = (double)cvGetTickCount();
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
			float diameterR = ProcessSingleEye(rightROI);
			float diameterL = ProcessSingleEye(leftROI);

			// Return back the result
			//func_(diameterL, diameterR);

			rightROI.release();
			leftROI.release();
		}
	}

	t = (double)cvGetTickCount() - t;
	double t0 = t / ((double)cvGetTickFrequency()*1000.);
	// Output - call this will make the std::cout not synchronized
	std::cout << "Processing time = " << t0 << " ms, avg thread FPS= " <<
		1000 / t0 << ". eyeRectVec: " << eyeRectVec.size() << std::endl;


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
	//imshow("result2", eyeGrayAdjusted);

	// Chords
	float chord1, chord2, chord3;
	// Width of two rows
	float dx = 3;
	// Critical of pupilla intensity
	int criticalValue = 18;

	// Split center lines and get chords
	if (eyeROI.rows < 4 * dx) return 0;
	cv::Mat row1(eyeROI.row(static_cast<int>(eyeROI.rows / 2)));
	chord1 = DetectChord(row1, criticalValue);
	cv::Mat row2(eyeROI.row(static_cast<int>(eyeROI.rows / 2 + dx)));
	chord2 = DetectChord(row2, criticalValue);
	cv::Mat row3(eyeROI.row(static_cast<int>(eyeROI.rows / 2 + dx + dx)));
	chord3 = DetectChord(row3, criticalValue);
	// Calculate diamter
	float diameter = CalculateDiameter(chord1, chord2, chord3, dx);

	row3.release();
	row2.release();
	row1.release();
	return diameter;
}



float PDThreadJob::DetectChord(cv::Mat& row, const int& criticalValue)
{
	// Find the pupillary Chord
	// Obtain the uchar data of this row

	std::vector<uchar> dataVecGray;
	cvEx::mat_foreach(row, [&dataVecGray](uchar& c, int){
		dataVecGray.push_back(c); });

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

	return static_cast<float>(rbit - lbit.base());
}

float PDThreadJob::CalculateDiameter(float c1, float c2, float c3, float dx)
{
	// Get the diameter
	float c1_sqr = c1*c1 / 4;
	float c2_sqr = c2*c2 / 4;
	float c3_sqr = c3*c3 / 4;
	float deltad = abs(((c1_sqr - c3_sqr) / 4 / dx - dx));
	//// Difference for delta d
	//std::cout << abs(c2_sqr * 2 / (c1_sqr - c3_sqr - 2 * dx*dx) / 4 / dx / dx);
	//std::cout << std::endl;

	// Judge c1>=c3?
	float diameter = 0;
	if (c1 >= c3) {
		// 1,2
		// Use c2 to adjust the deltad
		deltad += abs((c2_sqr - c3_sqr) / 2 / dx - dx / 2 - dx);
		deltad /= 2;
		diameter = sqrt(c1_sqr + deltad*deltad);
	}
	else {
		// 3,4
		// Use c2 to adjust the deltad
		deltad += abs((c2_sqr - c1_sqr) / 2 / dx - dx / 2 - dx);
		deltad /= 2;
		diameter = sqrt(c3_sqr + deltad*deltad);
	}

	return diameter + diameter;
}
