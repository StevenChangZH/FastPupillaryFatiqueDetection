#include "includes.h"
#include "CVEx.h"

float ProcessSingleEye(cv::Mat& eyeROI);
float DetectChord(cv::Mat& row, const int& criticalValue = 18);
float CalculateDiameter(float c1, float c2, float c3, float dx);

cv::Mat kernel = cv::getGaussianKernel(3, 0.05);

int main(int argc, const char* argv[])
{
	// It is more like a MatLab Command Window but C++ programming Design.
	// Realization without performance optimization.

	cv::CascadeClassifier cv_cascade;
	cv_cascade.load( "haarcascade_eye_tree_eyeglasses.xml" );
	// Get the gray image and perform hist equalization
	std::vector<cv::Rect> eyeRectVec;
	cv::Mat frame_img = cvLoadImage( "C:\\software\\frontface.png" );
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
		cv::Scalar color = CV_RGB( 255, 255, 255 );
		rectangle( frame_img, lefttop, rightdown, color, 1, 8, 0 );
	}

	cv::Mat leftROI = frame_img( eyeRectVec[0] );
	cv::Mat rightROI = frame_img( eyeRectVec[1] );
	/*
	cv::line( leftROI, cv::Point( 0, leftRect.height/2 ), cv::Point( leftRect.width, leftRect.height/2 ),
		CV_RGB( 255, 255, 255 ), 1, 8, 0 );
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
	float diameterR = ProcessSingleEye(rightROI);
	float diameterL = ProcessSingleEye(leftROI);

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



float ProcessSingleEye(cv::Mat& eyeROI)
{
	// Preprocessing
	// Eliminate catchlights
	cv::Mat eyeGray;
	cvtColor(eyeROI, eyeGray, CV_BGR2GRAY);
	cvEx::mat_foreach<uchar>(eyeGray, [](uchar& c, int) {
		if (c > 130)c = 20;
	});
	//imshow("result", eyeGray);
	cv::Mat eyeGrayAdjusted;
	morphologyEx(eyeGray,eyeGrayAdjusted, CV_MOP_OPEN, kernel);
	//imshow("result2", eyeGrayAdjusted);

	// Half of chords
	float chord1, chord2, chord3;
	// Width of two rows
	float dx = 3;
	// Critical of pupilla intensity
	int criticalValue = 18;

	// Split center lines and get chords
	cv::Mat row1(eyeGrayAdjusted.row(static_cast<int>(eyeGrayAdjusted.rows / 2)));
	chord1 = DetectChord(row1,criticalValue);
	cv::Mat row2(eyeGrayAdjusted.row(static_cast<int>(eyeGrayAdjusted.rows / 2+dx)));
	chord2 = DetectChord(row2, criticalValue);
	cv::Mat row3(eyeGrayAdjusted.row(static_cast<int>(eyeGrayAdjusted.rows / 2 +dx+dx)));
	chord3 = DetectChord(row3, criticalValue);
	// Calculate diamter
	float diameter =  CalculateDiameter(chord1, chord2, chord3, dx);

	row3.release();
	row2.release();
	row1.release();
	eyeGrayAdjusted.release();
	eyeGray.release();
	return diameter;
}



float DetectChord(cv::Mat& row, const int& criticalValue)
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

float CalculateDiameter(float c1, float c2, float c3, float dx)
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
	} else {
		// 3,4
		// Use c2 to adjust the deltad
		deltad += abs((c2_sqr - c1_sqr) / 2 / dx - dx / 2 - dx);
		deltad /= 2;
		diameter = sqrt(c3_sqr + deltad*deltad);
	}
	
	return diameter+diameter;
}