#include "includes.h"
#include "CVEx.h"

float ProcessingSingleEye(cv::Mat& eyeROI);
void DetectChord_Hor(std::vector<uchar>& dataVecGray, cv::Point2i& lPoint,
	cv::Point2i& rPoint, int criticalValue);
void DetectChord_Ver(std::vector<uchar>& dataVec, cv::Point2i& lowerPoint, int criticalValue);

cv::Mat kernel = cv::getGaussianKernel(3, 0.05);

int main(int argc, const char* argv[])
{
	// It is more like a MatLab Command Window but C++ programming Design.
	// Realization without performance optimization.

	cv::CascadeClassifier cv_cascade;
	cv_cascade.load( "haarcascade_eye_tree_eyeglasses.xml" );
	// Get the gray image and perform hist equalization
	std::vector<cv::Rect> eyeRectVec;
	//cv::Mat frame_img = cvLoadImage( "C:\\software\\frontface.png" );
	cv::Mat frame_img = cvLoadImage("C:\\software\\test.png");
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
	// Preprocessing
	// Eliminate catchlights
	cv::Mat eyeGray;
	cvtColor(eyeROI, eyeGray, CV_BGR2GRAY);
	cvEx::mat_foreach<uchar>(eyeGray, [](uchar& c, int) {
		//if (c > 130)c = 20;
	});
	//imshow("result", eyeGray);
	cv::Mat eyeGrayAdjusted;
	morphologyEx(eyeGray, eyeGrayAdjusted, CV_MOP_OPEN, kernel);
	//imshow("result2", eyeGrayAdjusted);

	// Critical of pupilla intensity
	int criticalValue = 18;

	// Split center lines and get chords

	cv::Point2i lPoint, rPoint;
	int horLine = static_cast<int>(eyeGrayAdjusted.rows / 2);
	lPoint.y = horLine;
	rPoint.y = horLine;
	cv::Mat row1(eyeGrayAdjusted.row(horLine));
	std::vector<uchar> rowDataVec1;
	cvEx::mat_foreach(row1, [&rowDataVec1](uchar& c, int){
		rowDataVec1.push_back(c); });
	DetectChord_Hor(rowDataVec1, lPoint, rPoint, criticalValue);
	int verLine = static_cast<int>((rPoint.x + lPoint.x) / 2);

	std::vector<uchar> colDataVec1;
	int eyeGrayAdjustedCols = eyeGrayAdjusted.cols;
	cvEx::mat_foreach(eyeGrayAdjusted, [&colDataVec1, &verLine, &eyeGrayAdjustedCols]
		(uchar& c, int num){
		if (((num + 1) % eyeGrayAdjustedCols) == (verLine%eyeGrayAdjustedCols)) {
			colDataVec1.push_back(c); }});
	cv::Point2i lowerPoint(verLine, horLine);
	DetectChord_Ver(colDataVec1, lowerPoint, criticalValue);

	float temp_a = static_cast<float>(rPoint.x - verLine);
	float radius = (pow(static_cast<float>(lowerPoint.y - horLine), 2) + pow(temp_a, 2))
		/ 2 / temp_a;
	cv::Point2f center((float)verLine, (float)lowerPoint.y - radius);
	cv::circle(eyeGrayAdjusted, center, (int)radius, CV_RGB(255, 255, 255));
	cv::line(eyeGrayAdjusted, lPoint, rPoint, CV_RGB(255, 255, 255));
	cv::line(eyeGrayAdjusted, center, lowerPoint, CV_RGB(0, 0, 255));
	imshow("NewWindow", eyeGrayAdjusted);


	row1.release();
	eyeGrayAdjusted.release();
	eyeGray.release();
	return 2.0;
}


void DetectChord_Hor(std::vector<uchar>& dataVecGray, cv::Point2i& lPoint, 
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

void DetectChord_Ver(std::vector<uchar>& colDataVec, cv::Point2i& lowerPoint, int criticalValue)
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