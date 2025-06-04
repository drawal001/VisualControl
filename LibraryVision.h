#pragma once
#include "utils.h"

namespace d5vs {

	enum Lib {
		minLib = 0,
		midLib = 1,
		maxLib = 2,
	};

	struct Jaw {
		HalconCpp::HTuple tempL, tempR;
	};

	struct PosTemp {
		cv::Mat posTempImg;
		cv::Point p;
	};

	struct JawPos {
		double x;
		double y;
		double angle;
		int flag;
	};

	class LibraryVision
	{
	public:
		LibraryVision();
		~LibraryVision();

		void JawLibSegmentation(cv::Mat img);
		JawPos GetJawPos(HalconCpp::HObject img, Lib lib);


	private:
		double			_mapParam;						//相机映射参数
		int				_libWidth;						//库宽
		int				_libHigh;						//库高
		Jaw				_jawTemp;						//钳口左右模板，TODO：待测试是否对所有模板均有效
		// topC
		PosTemp			_libMin;						//小钳口库定位标识
		PosTemp			_libMid;						//中钳口库定位标识
		PosTemp			_libMax;						//大钳口库定位标识
		// botC

	// 参数接口
	public:
		cv::Point GetMinLibP();
		cv::Point GetMidLibP();
		cv::Point GetMaxLibP();
		int GetLibWidth();
		int GetLibHigh();

	};
}

