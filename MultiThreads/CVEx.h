//
//  CVEx.h
//  FPFWD Project
//
//	Extend OpenCV algorithms
//	Operations on cv::Mat
//
//  Created by Steven Chang on 15/6/5.
//  Copyright (c) 2015 Feathergames. All rights reserved.
//

#pragma once
#include "includes.h"

namespace cvEx{

	template <typename DataElem_ = uchar, class FUNC_>
	// Retrieve data elements for cv::Mat
	// Default DataElem_ type is uchar
	// FUNC_: function object FUNC_, e.g.func(uchar&, int&)
	// caputure each element and index counter(from 0)
	inline void mat_foreach(cv::Mat& mat, FUNC_& func_)
	{
		int size = static_cast<int>(mat.rows * mat.cols * 
			( mat.elemSize() / sizeof(DataElem_) ));
		DataElem_* dPtr = const_cast<uchar*>( mat.ptr() );
		for (int i = 0; i < size; ++i) {
			func_( *(dPtr + i), i );
		}
	}


	template <typename Pixeltype = cv::Vec3b, 
			typename DataElem_ = uchar, 
			class FUNC_>
	// Using this template to operate on each pixel and return the data
	// Pixeltype is the set of DataElem_ and must responds to one pixel
	// Pixeltype must overload the subscripting and assignment operators
	// FUNC_: function object FUNC_, e.g.func(Pixeltype&, int&)
	// caputure each pixel and pixel index counter(from 0)
	inline std::vector<Pixeltype> vec_mat_foreach(cv::Mat& mat, FUNC_& func_)
	{
		std::vector<Pixeltype> vec;
		Pixeltype pixel;
		int size = mat.rows * mat.cols;
		int elemSize = mat.elemSize() / sizeof(DataElem_);
		DataElem_* dPtr = const_cast<uchar*>( mat.ptr() );
		DataElem_* tpPtr = dPtr;
		for ( int i = 0; i < size; ++i ) {
			for ( int j = 0; j < elemSize; ++j, ++tpPtr ) {
				pixel[j] = *tpPtr;
			}

			func_( pixel, i );
			vec.push_back( pixel );
		}
		return vec;
	}
}

/*
	Samples

	cvEx::mat_foreach
		cv::Mat mt;
		std::vector<uchar> dataVecGray;
		cvEx::mat_foreach( mt, [&dataVecGray](uchar& c, int){
			dataVecGray.push_back(c); } );

	cvEX::vec_mat_foreach:
		cv::Mat mt;
		auto dataVec = cvEx::vec_mat_foreach( mt, [](cv::Vec3b&, int){} );
*/