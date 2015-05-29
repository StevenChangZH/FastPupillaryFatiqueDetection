#include "includes.h"
#include "CVEx.h"

float ProcessSingleEye(cv::Mat& eyeROI);
void DetectChord(cv::Mat& row, float& chord, const int& criticalValue = 18);
float CalculateDiameter(float c1, float c2, float c3, float dx);

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
	/*
	t = (double)cvGetTickCount() - t;
	double t0=t/((double)cvGetTickFrequency()*1000.);
	// Output
	std::cout << "Detection time = " << t0 << " ms, avg thread FPS= " <<
		1000/t0 << ". eyeRectVec: " << eyeRectVec.size() << std::endl;
	*/
	
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

	cv::Rect& leftRect = eyeRectVec[0];
	cv::Rect& rightRect = eyeRectVec[1];
	cv::Mat leftROI = frame_img( leftRect );
	cv::Mat rightROI = frame_img( rightRect );
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
	float diameterL = ProcessSingleEye(leftROI);
	float diameterR = ProcessSingleEye(rightROI);

	//int q = 3;
	










	t = (double)cvGetTickCount() - t;
	double t0 = t / ((double)cvGetTickFrequency()*1000.);
	// Output
	std::cout << "Processing time = " << t0 << " ms, avg thread FPS= " <<
		1000 / t0 << ". eyeRectVec: " << eyeRectVec.size() << std::endl;

	cvWaitKey(0);

	// Free the memory
	frame_gray.release();
	frame_img.release();
	return 0;
}



float ProcessSingleEye(cv::Mat& eyeROI)
{
	// Preprocessing
	cv::Mat eyeGray(eyeROI);
	cvtColor(eyeROI, eyeGray, CV_BGR2GRAY);
	cvEx::mat_foreach(eyeGray, [](uchar& c, int&) {
		if (c > 130)c = 20;
	});
	//imshow("result", eyeGray);
	// Eliminate catchlights
	cv::Mat rightGrayAdjused(eyeGray);
	cv::Mat kernel = cv::getGaussianKernel(3, 0.05);
	erode(eyeGray, rightGrayAdjused, kernel, cv::Point(-1, -1), 1);
	dilate(rightGrayAdjused, rightGrayAdjused, kernel, cv::Point(-1, -1), 1);
	//imshow("result2", rightGrayAdjused);

	float chord1, chord2, chord3;
	float dx = 3;
	int criticalValue = 19;

	// Split center lines and get chords
	cv::Mat row1(rightGrayAdjused.row(static_cast<int>(rightGrayAdjused.rows / 2)));
	DetectChord(row1, chord1, criticalValue);
	cv::Mat row2(rightGrayAdjused.row(static_cast<int>(rightGrayAdjused.rows / 2 + 3)));
	DetectChord(row2, chord2, criticalValue);
	cv::Mat row3(rightGrayAdjused.row(static_cast<int>(rightGrayAdjused.rows / 2 + 6)));
	DetectChord(row3, chord3, criticalValue);

	// Calculate diamter
	float diameter =  CalculateDiameter(chord1, chord2, chord3, dx);
	return diameter;
}



void DetectChord(cv::Mat& row, float& chord, const int& criticalValue)
{
	// Find the pupillary Chord
	// Obtain the uchar data of this row

	std::vector<uchar> dataVecGray;
	cvEx::mat_foreach(row, [&dataVecGray](uchar& c, int&){
		dataVecGray.push_back(c); });

	// Left bounds

	auto it = dataVecGray.begin() + static_cast<int>(dataVecGray.size() / 2);
	int centerCrValue = static_cast<int>(*it);
	int colorSum = centerCrValue;
	int colorCount = 1;
	for (; it != dataVecGray.begin(); --it) {

		++colorCount;
		int tpValue = static_cast<int>(*it);
		colorSum += tpValue;
		if (centerCrValue - criticalValue < tpValue
			&& tpValue < centerCrValue + criticalValue) {
		}
		else {
			break;
		}
	}
	int lBound = it - dataVecGray.begin();

	// right bound
	
	colorSum = centerCrValue;
	colorCount = 1;
	for (it = dataVecGray.begin() + static_cast<int>(dataVecGray.size() / 2);
		it != dataVecGray.end(); ++it) {

		++colorCount;
		int tpValue = static_cast<int>(*it);
		colorSum += tpValue;
		if (centerCrValue - criticalValue < tpValue
			&& tpValue < centerCrValue + criticalValue) {
		}
		else {
			break;
		}
	}
	int rBound = it - dataVecGray.begin();
	chord = static_cast<float>(rBound - lBound)/2;
}

float CalculateDiameter(float c1, float c2, float c3, float dx)
{
	// Get the diameter
	float c1_sqr = c1*c1;
	float c3_sqr = c3*c3;
	float deltad = abs(((c1_sqr - c3_sqr) / 4 / dx - dx));
	//// Difference for delta d
	//std::cout << abs(c2*c2 * 2 / (c1*c1 - c3*c3 - 2 * dx*dx) / 4 / dx / dx);
	//std::cout << std::endl;

	// First judge c1>=c3?
	float diameter = 0;
	if (c1 - c3>=0) {
		// 1,2
		// Use c2 to adjust the deltad
		deltad += abs((c2*c2 - c3_sqr) / 2 / dx - 3 * dx / 2);
		deltad /= 2;
		diameter = sqrt(c1_sqr + deltad*deltad);
	} else {
		// 3,4
		// Use c2 to adjust the deltad
		deltad += abs((c2*c2 - c1_sqr) / 2 / dx - 3 * dx / 2);
		deltad /= 2;
		diameter = sqrt(c3_sqr + deltad*deltad);
	}
	
	return diameter;
}