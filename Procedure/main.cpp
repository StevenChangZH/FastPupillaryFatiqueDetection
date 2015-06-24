#include "includes.h"
#include "CVEx.h"

cv::Mat constructSemiCircleKernel(unsigned width);
float ProcessingSingleEye(cv::Mat& eyeROI);

int main(int argc, const char* argv[])
{
	// It is more like a MatLab Command Window but C++ programming Design.
	// Realization without performance optimization.

	cv::CascadeClassifier cv_cascade;
	cv_cascade.load( "haarcascade_eye_tree_eyeglasses.xml" );
	// Get the gray image and perform hist equalization
	std::vector<cv::Rect> eyeRectVec;
	//cv::Mat frame_img = cvLoadImage( "C:\\software\\frontface.png" );
	cv::Mat frame_img = cvLoadImage("C:\\software\\test2.png");
	cv::Mat frame_gray = cv::Mat( frame_img );
	cvtColor( frame_gray, frame_gray, CV_BGR2GRAY );
	equalizeHist( frame_gray, frame_gray );
	
	// Detect eye && get time
	//double t = (double)cvGetTickCount();
	cv_cascade.detectMultiScale( frame_gray , eyeRectVec,
		1.1, 2, 0
		//|CV_HAAR_FIND_BIGGEST_OBJECT
		//|CV_HAAR_DO_ROUGH_SEARCH
		|CV_HAAR_SCALE_IMAGE
		,
		cv::Size(30, 30) );
	/*
	t = (double)cvGetTickCount() - t;
	double t0=t/((double)cvGetTickFrequency()*1000.);
	// Output
	std::cout << "Detection time = " << t0 << " ms, avg thread FPS= " <<
		1000/t0 << ". eyeRectVec: " << eyeRectVec.size() << std::endl;
	*/

	double t = (double)cvGetTickCount();
	// Calculate the pos
	for( std::vector<cv::Rect>::iterator r = eyeRectVec.begin(); 
		r != eyeRectVec.end() && ( r - eyeRectVec.begin() ) < 2; ++r )
	{
		r->height /= 2;
		r->y += r->height/2;
		
		cv::Point lefttop( r->x, r->y );
		cv::Point rightdown( r->x+r->width, r->y+r->height );
		cv::Scalar color = CV_RGB( 255, 0, 0 );
		rectangle( frame_img, lefttop, rightdown, color, 1, 8, 0 );
	}

	cv::Rect leftRect = eyeRectVec[0];
	cv::Rect rightRect = eyeRectVec[1];
	cv::Mat leftROI = frame_img(leftRect);
	cv::Mat rightROI = frame_img(rightRect);
	/*
	cv::line( leftROI, cv::Point( 0, leftRect.height/2 ), cv::Point( leftRect.width, leftRect.height/2 ),
		CV_RGB(255, 255, 255), 1, 8, 0);
	cv::line( leftROI, cv::Point( 0, leftRect.height/2+3 ), cv::Point( leftRect.width, leftRect.height/2+3 ),
		CV_RGB( 255, 255, 255 ), 1, 8, 0 );
	cv::line( leftROI, cv::Point( 0, leftRect.height/2-3 ), cv::Point( leftRect.width, leftRect.height/2-3 ),
		CV_RGB( 255, 255, 255 ), 1, 8, 0 );
	cv::line( leftROI, cv::Point( leftRect.width/2, 0 ), cv::Point( leftRect.width/2, leftRect.height ),
		CV_RGB( 255, 255, 255 ), 1, 8, 0 );
	cv::line( leftROI, cv::Point( leftRect.width/2+3, 0 ), cv::Point( leftRect.width/2+3, leftRect.height ),
		CV_RGB( 255, 255, 255 ), 1, 8, 0 );
	cv::line( leftROI, cv::Point( leftRect.width/2-3, 0 ), cv::Point( leftRect.width/2-3, leftRect.height ),
		CV_RGB( 255, 255, 255 ), 1, 8, 0 );*/
	/*
	cv::line(rightROI, cv::Point(0, rightRect.height / 2 + 6), cv::Point(rightRect.width, rightRect.height / 2 + 6),
		CV_RGB( 255, 255, 255 ), 1, 8, 0 );
	cv::line(rightROI, cv::Point(0, rightRect.height / 2), cv::Point(rightRect.width, rightRect.height / 2),
		CV_RGB( 255, 255, 255 ), 1, 8, 0 );
	cv::line(rightROI, cv::Point(0, rightRect.height / 2 + 3), cv::Point(rightRect.width, rightRect.height / 2 + 3),
		CV_RGB(255, 255, 255), 1, 8, 0); 
	cv::line(rightROI, cv::Point(rightRect.width / 2, 0), cv::Point(rightRect.width / 2, rightRect.height),
		CV_RGB( 255, 255, 255 ), 1, 8, 0 );
	cv::line(rightROI, cv::Point(rightRect.width / 2 + 3, 0), cv::Point(rightRect.width / 2 + 3, rightRect.height),
		CV_RGB( 255, 255, 255 ), 1, 8, 0 );
	cv::line( rightROI, cv::Point( rightRect.width/2-3, 0 ), cv::Point( rightRect.width/2-3, rightRect.height ),
		CV_RGB( 255, 255, 255 ), 1, 8, 0 );
	*/
	// Show img
	cv::imshow( "Raw Image", frame_img );
	//cv::imshow( "result", leftROI );
	//cv::imshow("result", rightROI);


	// Get the diameter
	float diameterR = ProcessingSingleEye(rightROI);
	float diameterL = ProcessingSingleEye(leftROI);
	//int q = 3;

	t = (double)cvGetTickCount() - t;
	double t0 = t / ((double)cvGetTickFrequency()*1000.);
	// Output
	std::cout << "Processing time = " << t0 << " ms, avg thread FPS= " <<
		1000 / t0 << ". eyeRectVec: " << eyeRectVec.size() << std::endl;

	cvWaitKey(0);

	// Free the memory
	cv::destroyAllWindows();
	rightROI.release();
	leftROI.release();
	frame_gray.release();
	frame_img.release();
	return 0;
}

float ProcessingSingleEye(cv::Mat& eyeROI)
{
	cv::Mat eyeGray;
	cvtColor(eyeROI, eyeGray, CV_BGR2GRAY);

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
	int sec = 20;
	// Left 
	unsigned lBound = sliceVec.size();
	unsigned lcBound = 0;
	{
		auto istart = [&sliceVec](){auto it = sliceVec.begin(); while (!*it){ ++it; } return it; }();
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
		auto istart = [&sliceVec](){auto it = sliceVec.end() - 1; while (!*it){ --it; } return it; }();
		unsigned val = *istart;
		// rBound
		for (auto it = istart - 1; it != sliceVec.begin(); --it) {
			unsigned currVal = *it;
			if ((int)currVal<(int)(val - sec) || currVal >val + sec) {
				rBound = it - sliceVec.begin();
				break;
			}
		}
		// rcBound
		unsigned sumVal = sliceVec[rBound];
		unsigned sumnum = 1;
		unsigned count = 0;
		for (auto it = sliceVec.begin() + rBound - 1; it != sliceVec.begin(); --it, ++count) {
			if (*it == 0) {
				// Discard this value
			}
			else if (*it < static_cast<unsigned>(sumVal / sumnum) && count >3) {
				rcBound = it - sliceVec.begin() - 1;
				break;
			}
			else {
				sumVal += *it;
				++sumnum;
			}
		}
	}
	unsigned diameterRough = rcBound - lcBound;
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
				r = row + i;
				c = centerCol + j;
				if (r < rows&&c < cols) {
					unsigned kernelVal = kernel.at<uchar>(i, j);
					unsigned cannyVal = eyeCanny.at<uchar>(r, c);
					//val += (kernelVal*cannyVal);
					if (kernelVal && cannyVal) {
						val += (kernelVal*cannyVal);
					}
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
	{
		// 3.1. Generate means of semicircles with different diameters
		unsigned numberOfJudge = (kernel.cols / 5 > 2) ? 5 : 3;
		std::vector<unsigned> circleSumVec(numberOfJudge, 0);
		std::vector<unsigned> circleCountVec(numberOfJudge, 0);
		std::vector<unsigned> circleBoundVec(kernel.cols, 0);
		unsigned kernelCols = kernel.cols;
		cvEx::mat_foreach<uchar>(kernel, [&circleBoundVec, &kernelCols](uchar& c, int i) {
			if (circleBoundVec[i%kernelCols] == 0 && c == 1) {
				circleBoundVec[i%kernelCols] = i / kernelCols;
			}
		});
		cv::Mat eyeSemiCircle = eyeGray.rowRange(centerRow, min(centerRow + kernel.rows, eyeGray.rows - 1))
			.colRange(centerCol - kernel.cols / 2, centerCol + kernel.cols / 2);
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
	}

	/*
	// Display and release
	cv::line(eyeROI, cv::Point(0, centerRow), cv::Point(eyeCanny.cols - 1, centerRow), CV_RGB(255, 255, 255));
	cv::line(eyeROI, cv::Point(centerCol, 0), cv::Point(centerCol, eyeCanny.rows - 1), CV_RGB(255, 255, 255));
	cv::line(eyeCanny, cv::Point(0, 6), cv::Point(eyeCanny.cols - 1, 6), CV_RGB(255, 255, 255));
	cv::line(eyeCanny, cv::Point(0, centerRow), cv::Point(eyeCanny.cols - 1, centerRow), CV_RGB(255, 255, 255));
	cv::line(eyeCanny, cv::Point(centerCol, 0), cv::Point(centerCol, eyeCanny.rows - 1), CV_RGB(255, 255, 255));
	cv::circle(eyeROI, cv::Point(centerCol, centerRow), diameter, CV_RGB(255, 255, 255));

	cvEx::mat_foreach<uchar>(kernel, [](uchar& c, int) {
	c = c * 255;
	});
	imshow("kernel", kernel);
	imshow("eyeROI", eyeROI);
	imshow("eyeCanny", eyeCanny);*/
	kernel.release();
	eyeCanny.release();
	slice.release();
	eyeGray.release();
	return diameter;
}

cv::Mat constructSemiCircleKernel(unsigned width)
{
	// The kernel is a ((width)*0.7, width) matrix.
	unsigned cols = width;
	unsigned rows = static_cast<unsigned>(width*0.8);
	cv::Mat kernel(rows, cols, CV_8U, cv::Scalar(0));
	for (unsigned i = 0; i < cols / 2; ++i) {
		unsigned j = static_cast<unsigned>(sqrt((float)i*(cols - i)));
		kernel.at<uchar>(cols / 2 - i - 1, cols / 2 + j) = 1;
		kernel.at<uchar>(cols / 2 - i - 1, cols / 2 - j - 1) = 1;
	}
	for (unsigned j = 0; j < cols; ++j) {
		unsigned i = static_cast<unsigned>(sqrt((float)j*(cols - 1 - j)));
		kernel.at<uchar>(i, j) = 1;
	}
	return kernel.rowRange(rows / 8, rows - 1).clone();
}