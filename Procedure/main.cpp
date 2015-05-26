#include "includes.h"

// Notice: only use using in this file
using namespace std;
using namespace cv;

int main(int argc, char* argv[])
{
	cv::CascadeClassifier cv_cascade;
	cv_cascade.load( "haarcascade_eye_tree_eyeglasses.xml" );
	// Get the gray image and perform hist equalization
	std::vector<cv::Rect> eyeRectVec;
	cv::Mat frame_img = cvLoadImage( "C:\\software\\frontface.png" );
	cv::Mat frame_gray = cv::Mat( frame_img );
	cvtColor( frame_gray, frame_gray, CV_BGR2GRAY );
	equalizeHist( frame_gray, frame_gray );
	
	// Detect eye && get time
	double t = (double)cvGetTickCount();
	cv_cascade.detectMultiScale( frame_gray , eyeRectVec,
		1.1, 2, 0
		//|CV_HAAR_FIND_BIGGEST_OBJECT
		//|CV_HAAR_DO_ROUGH_SEARCH
		|CV_HAAR_SCALE_IMAGE
		,
		cv::Size(30, 30) );
	t = (double)cvGetTickCount() - t;
	double t0=t/((double)cvGetTickFrequency()*1000.);
	// Output
	std::cout << "Detection time = " << t0 << " ms, avg thread FPS= " <<
		1000/t0 << ". eyeRectVec: " << eyeRectVec.size() << std::endl;

	
	// Calculate the pos
	int i = 0;// Temp loop counter
	for( std::vector<cv::Rect>::iterator r = eyeRectVec.begin(); 
		r != eyeRectVec.end() && i < 2; ++r, ++i )
	{
		r->height /= 2;
		//r->width /= 2;
		//r->x += r->width/2;
		r->y += r->height/2;

		cv::Point lefttop( r->x, r->y );
		cv::Point rightdown( r->x+r->width, r->y+r->height );
		cv::Scalar color = CV_RGB( 255, 255, 255 );
		rectangle( frame_img, lefttop, rightdown, color, 1, 8, 0 );
	}

	cv::Rect& leftRect = eyeRectVec[0];
	cv::Rect& rightRect = eyeRectVec[1];
	cv::Mat leftROI = frame_img( leftRect );
	cv::Mat rightROI = frame_img( rightRect );
	/*
	line( leftROI, cv::Point( 0, leftRect.height/2 ), cv::Point( leftRect.width, leftRect.height/2 ),
		CV_RGB( 255, 255, 255 ), 1, 8, 0 );
	line( leftROI, cv::Point( 0, leftRect.height/2+3 ), cv::Point( leftRect.width, leftRect.height/2+3 ),
		CV_RGB( 255, 255, 255 ), 1, 8, 0 );
	line( leftROI, cv::Point( 0, leftRect.height/2-3 ), cv::Point( leftRect.width, leftRect.height/2-3 ),
		CV_RGB( 255, 255, 255 ), 1, 8, 0 );
	line( leftROI, cv::Point( leftRect.width/2, 0 ), cv::Point( leftRect.width/2, leftRect.height ),
		CV_RGB( 255, 255, 255 ), 1, 8, 0 );
	line( leftROI, cv::Point( leftRect.width/2+3, 0 ), cv::Point( leftRect.width/2+3, leftRect.height ),
		CV_RGB( 255, 255, 255 ), 1, 8, 0 );
	line( leftROI, cv::Point( leftRect.width/2-3, 0 ), cv::Point( leftRect.width/2-3, leftRect.height ),
		CV_RGB( 255, 255, 255 ), 1, 8, 0 );*/

	line( rightROI, cv::Point( 0, rightRect.height/2-3 ), cv::Point( rightRect.width, rightRect.height/2-3 ),
		CV_RGB( 255, 255, 255 ), 1, 8, 0 );
	line( rightROI, cv::Point( 0, rightRect.height/2 ), cv::Point( rightRect.width, rightRect.height/2 ),
		CV_RGB( 255, 255, 255 ), 1, 8, 0 );
	line( rightROI, cv::Point( 0, rightRect.height/2+3 ), cv::Point( rightRect.width, rightRect.height/2+3 ),
		CV_RGB( 255, 255, 255 ), 1, 8, 0 );
	line( rightROI, cv::Point( rightRect.width/2, 0 ), cv::Point( rightRect.width/2, rightRect.height ),
		CV_RGB( 255, 255, 255 ), 1, 8, 0 );
	line( rightROI, cv::Point( rightRect.width/2+3, 0 ), cv::Point( rightRect.width/2+3, rightRect.height ),
		CV_RGB( 255, 255, 255 ), 1, 8, 0 );
	line( rightROI, cv::Point( rightRect.width/2-3, 0 ), cv::Point( rightRect.width/2-3, rightRect.height ),
		CV_RGB( 255, 255, 255 ), 1, 8, 0 );

	// Show img
	//imshow( "result", frame_img );
	//imshow( "result", leftROI );
	imshow( "result", rightROI );


	//cv::Mat circleLeftROI;
	//HoughCircles( leftROI, circleLeftROI, CV_HOUGH_GRADIENT,2,3,140,118,2,120);
	//Canny( leftROI, cannyLeftROI, 80, 150, 3 );
	//imshow( "result", circleLeftROI );












	cvWaitKey( 0 );

	// Free the memory
	frame_gray.release();
	frame_img.release();
	return 0;
}