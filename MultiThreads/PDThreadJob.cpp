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

PDThreadJob::PDThreadJob(const std::string& cascadename)
{
	// Load the file to construct a cascade classifier
	cc_cascade.load(cascadename);
}

PDThreadJob::~PDThreadJob()
{}

void PDThreadJob::SynchronizeData(cv::Mat& img_, std::chrono::system_clock::time_point& tp_)
{
	frame_img = img_(cv::Rect(200, 90, 300, 100));
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
	if (eyeRectVec.size() != 0) {
		// Perhaps we can process
		cv::Rect rightRect;
		if (eyeRectVec.size() == 2) {
			rightRect = eyeRectVec[1];
		} else {
			rightRect = eyeRectVec[0];
		}

		cv::Mat rightROI = frame_gray(rightRect);
		float diameterR = ProcessingSingleEye(rightROI);
		float diameterL = diameterR;// ProcessingSingleEye(leftROI);

		// Return back the result
		float distOfTwoEyes = 1.0f;
		DataLog log(timepoint, diameterL, diameterR, distOfTwoEyes);
		SingletonGrabber<PDThreadPool>::GetInstance().first->GetSynchronizedDataFromThread(log);

		rightROI.release();
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



float PDThreadJob::ProcessingSingleEye(cv::Mat& eyeGray)
{
	//cv::Mat eyeGray(eyeROI);
	//cvtColor(eyeROI, eyeGray, CV_BGR2GRAY);

	// 1.1. Discard skins
	unsigned rows = static_cast<unsigned>(eyeGray.rows);
	unsigned cols = static_cast<unsigned>(eyeGray.cols);
	const uchar threshold = 70;

	for (unsigned j = 0; j < cols; ++j) {
		unsigned i = 0;
		// Upper
		for (; i < rows; ++i) {
			uchar& c = eyeGray.at<uchar>(i, j);
			if (c < threshold) {
				break;
			}
			else {
				c = 255;
			}
		}
		// Lower
		for (unsigned k = rows - 1; k > i; --k)  {
			uchar& c = eyeGray.at<uchar>(k, j);
			if (c < threshold) {
				i = -1;
			}
			else {
				c = 255;
			}
		}
	}

	// 1.2. Assign data and get means
	std::vector<unsigned> sliceVec(cols, 0);
	cv::Mat slice = eyeGray.row((unsigned)(rows / 2)).clone();
	cvEx::mat_foreach<uchar>(eyeGray, [&sliceVec, cols](uchar& c, int i) {
		if (c != 255) {
			sliceVec[i % cols] += c;
		}
	});
	
	uchar maxVal = 0;
	// 1.3. Equalization - Extend intensity range to 255
	cvEx::mat_foreach<uchar>(slice, [&sliceVec, &rows, &maxVal](uchar& c, int i) {
		if (c == 255) { c = 0; }
		c = c / 2 + static_cast<uchar>(sliceVec[i] / rows / 2);
		if (c > maxVal) { maxVal = c; }
	});
	float eqratio = static_cast<float>(maxVal) / 255.0f;
	cvEx::mat_foreach<uchar>(slice, [&eqratio](uchar& c, int i) {
		c = (unsigned)(c / eqratio);
	});
	cvEx::mat_foreach<uchar>(slice, [&maxVal, &sliceVec](uchar& c, int i) {
		if (c > 150) { sliceVec[i] = 0; } //c = 0; }
		else { sliceVec[i] = c; }
	});
	// 1.3. Extract the boundary (x4)
	// The sequence of the following data is:
	// lBound -lcBound-pupilla(catchlights)-rcBound-rBound
	unsigned sec = 20;
	// Left 
	unsigned lBound = 0;
	unsigned lcBound = 0;
	{
		auto istart = [&sliceVec]() {
			auto it = sliceVec.begin();
			while (!*it){ 
				++it;
				if (it == sliceVec.end()){ 
					return sliceVec.end() - 1; } 
			} 
			return it; 
		}();
		auto iend = sliceVec.end();
		unsigned val = *istart;
		// lBound
		for (auto it = istart + 1; it != iend; ++it) {
			unsigned currVal = *it;
			if (currVal<val - sec || currVal >val + sec) {
				lBound = it - sliceVec.begin();
				lcBound = lBound - 1;
				break;
			}
		}
		// lcBound
		unsigned sumVal = sliceVec[lBound];
		unsigned sumnum = 1;
		unsigned count = 0;
		if ((sliceVec.begin() + lBound) == sliceVec.end()){ --lBound; }
		for (auto it = sliceVec.begin() + lBound + 1; it != iend; ++it, ++count) {
			if (*it == 0) {
				// Discard this value
			}
			else if (*it < static_cast<unsigned>(sumVal / sumnum) && count >3) {
				lcBound = it - sliceVec.begin() - 1;
				break;
			}
			else {
				sumVal += *it;
				++sumnum;
			}
		}
	}
	// Right
	unsigned rBound = 0;
	unsigned rcBound = 0;
	{
		auto istart = [&sliceVec]() {
			auto it = sliceVec.end() - 1;
			while (!*it){
				--it;
				if (it == sliceVec.begin()){
					return sliceVec.begin() + 1;
				}
			}
			return it;
		}();
		unsigned val = *istart;
		// rBound
		for (auto it = istart - 1; it != sliceVec.begin(); --it) {
			unsigned currVal = *it;
			if (currVal<(val - sec) || currVal >val + sec) {
				rBound = it - sliceVec.begin();
				break;
			}
		}
		// rcBound
		unsigned sumVal = sliceVec[rBound];
		unsigned sumnum = 1;
		unsigned count = 0;
		if (rBound == 0){ rBound = 1; }
		for (auto it = sliceVec.begin() + rBound - 1; it != sliceVec.begin(); --it, ++count) {
			if (*it == 0) {
				// Discard this value
			} else if (*it < static_cast<unsigned>(sumVal / sumnum) && count >3) {
				rcBound = it - sliceVec.begin() - 1;
				break;
			} else {
				sumVal += *it;
				++sumnum;
			}
		}
	}
	unsigned diameterRoughtp = (rcBound < lcBound+6) ? 7 : (rcBound - lcBound);
	const unsigned diameterRough = (diameterRoughtp > 20) ? 20 : diameterRoughtp;
	const unsigned centerCol = (lcBound + rcBound) / 2 + 2;
	
	
	// 2. Match the kernel to find the center of pupilla
	// 2.1. Construct the kernel (a semi-circle, part of the lower pupil)
	cv::Mat eyeCanny;
	cv::Canny(eyeGray, eyeCanny, 55, 120);
	cv::Mat kernel = constructSemiCircleKernel(diameterRough);

	// 2.2. Match the kernel and get the centerRow
	std::vector<unsigned> matchVec;
	matchVec.reserve(diameterRough);
	for (unsigned row = (rows - diameterRough) / 2; row < (rows + diameterRough) / 2; ++row) {
		unsigned val = 0;
		unsigned r = 0;
		unsigned c = 0;
		for (unsigned i = 0; i < (unsigned)kernel.rows; ++i) {
			for (unsigned j = 0; j < (unsigned)kernel.cols; ++j) {
				r = (row + i>rows) ? rows - 1 : row + i;
				c = (centerCol + j>cols) ? cols - 1 : centerCol + j;
				if (r < rows&&c < cols) {
					val += (kernel.at<uchar>(i, j)*eyeCanny.at<uchar>(r, c));
				}
			}
		}
		matchVec.push_back(val);
	}
	auto it = std::max_element(matchVec.begin(), matchVec.end(), [](unsigned& _x, unsigned& _y) {
		return _x < _y;
	});
	const unsigned centerRow = it - matchVec.begin() + (rows - diameterRough) / 2 + 2;

	
	// Now we we've got a more concise position of the center.
	// 3. Calculate the diameter
	float diameter = 0.0f;
	try{
		// 3.1. Generate means of semicircles with different diameters
		kernel = constructSemiCircleKernel(diameterRough);
		unsigned numberOfJudge = (kernel.cols / 5 > 2) ? 5 : 3;
		std::vector<unsigned> circleSumVec(numberOfJudge, 0);
		std::vector<unsigned> circleCountVec(numberOfJudge, 0);
		std::vector<unsigned> circleBoundVec(kernel.cols, 0);
		unsigned kernelCols = kernel.cols;
		for (unsigned i = 0; i < (unsigned)kernel.rows; ++i) {
			for (unsigned j = 0; j < (unsigned)kernel.cols; ++j) {
				uchar c = kernel.at<uchar>(i, j);
				if (circleBoundVec[j] == 0 && c == 1) {
					circleBoundVec[j] = i;
				}
			}
		}/*
		cvEx::mat_foreach<uchar>(kernel, [&circleBoundVec, &kernelCols](uchar& c, int i) {
			if (circleBoundVec[i%kernelCols] == 0 && c == 1) {
				circleBoundVec[i%kernelCols] = i / kernelCols;
			}
		});*/
		unsigned eyeSemiCircleRow = min((unsigned)(centerRow + kernel.rows), (unsigned)(eyeGray.rows - 1));
		unsigned eyeSemiCircleCols = (unsigned)max(0, (int)(centerCol - kernel.cols / 2));
		unsigned eyeSemiCircleCole = (unsigned)min((unsigned)eyeGray.cols-1, (unsigned)centerCol + kernel.cols / 2);
		// FORCE to return
		if ((int)eyeSemiCircleCols>=eyeGray.cols || eyeSemiCircleCols >= eyeSemiCircleCole
			|| centerRow >= eyeSemiCircleRow){
			return 0.0f;
		}
		//std::cout << eyeSemiCircleCols << std::endl << eyeSemiCircleCole << std::endl; 
		cv::Mat eyeSemiCircle = eyeGray.rowRange(centerRow, eyeSemiCircleRow)
			.colRange(eyeSemiCircleCols, eyeSemiCircleCole);
		for (unsigned k = 0; k < numberOfJudge; ++k) {
			unsigned circleSum = 0;
			unsigned circleCount = 0;
			for (unsigned j = 0; j < (unsigned)eyeSemiCircle.cols; ++j) {
				for (unsigned i = 0; i < (unsigned)eyeSemiCircle.rows; ++i) {
					if (i < circleBoundVec[j] + k) {
						uchar val = eyeSemiCircle.at<uchar>(i, j);
						if (val != 0) {
							circleSum += val;
							++circleCount;
						}
					}
					else {
						i = (unsigned)eyeSemiCircle.rows;
					}
				}
			}
			circleSumVec[k] = circleSum;
			circleCountVec[k] = circleCount;
		}
		// Get the means
		std::vector<float> circleMeanVec(numberOfJudge, 0.0f);
		for (unsigned i = 0; i < numberOfJudge; ++i) {
			circleMeanVec[i] = static_cast<float>((float)circleSumVec[i] /
				(float)circleCountVec[i]);
		}
		
		// 3.2. Generate the diameter using former data
		if (numberOfJudge == 5) {
			auto it = circleMeanVec.begin();
			float alpha1 = *(it + 4) + *it - *(it + 3) - *(it + 1);
			float alpha2 = *(it + 3) + *(it + 1) - (*(it + 2)) * 2;
			diameter = (float)diameterRough / 2 + (alpha1 * 952 / 1205 + alpha2 * 253 / 1205)
				/ (alpha1 + alpha2);
		}
		else if (numberOfJudge == 3) {
			auto it = circleMeanVec.begin();
			float alpha = *(it + 2) + *it - 2 * (*(it + 1));
			diameter = (float)diameterRough / 2 + 2 * alpha / ((float)*(it + 2) - *it) + 2;
		}
	} catch (const std::exception& e){
		std::cerr << e.what() << std::endl;
		throw;
	}
	

	kernel.release();
	eyeCanny.release();
	slice.release();
	return 10.0f;
}

cv::Mat PDThreadJob::constructSemiCircleKernel(unsigned width)
try{
	// The kernel is a ((width)*0.7, width) matrix.
	unsigned cols = width;
	unsigned rows = static_cast<unsigned>(width*0.8);
	cv::Mat kernel(rows, cols, CV_8U, cv::Scalar(0));
	for (unsigned i = 0; i < cols / 2; ++i) {
		unsigned j = static_cast<unsigned>(sqrt((float)i*(cols - i)));
		j = (j>rows / 2 - 1) ? (unsigned)(rows / 2 - 1) : j;
		j = (j < 0) ? 0 : j;
		kernel.at<uchar>(cols / 2 - i, cols / 2 + j) = 1;
		kernel.at<uchar>(cols / 2 - i, cols / 2 - j - 1) = 1;
	}
	for (unsigned j = 0; j < cols; ++j) {
		unsigned i = static_cast<unsigned>(sqrt((float)j*(cols - 1 - j)));
		i = (i>rows *0.7 / 2 - 1) ? (unsigned)(rows*0.7 / 2 - 1) : i;
		i = (i < 0) ? 0 : i;
		kernel.at<uchar>(i, j) = 1;
	}
	return kernel.rowRange(rows / 8, rows - 1).clone();
} catch(const std::exception& e) {
		std::cerr << e.what() << std::endl; 
		throw;
}