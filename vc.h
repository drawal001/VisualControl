#pragma once
#include "utils.h"

namespace d5vs {
	struct Jaw {
		HalconCpp::HTuple tempL, tempR;
	};
	struct PosTemp {
		cv::Mat posTempImg;
		cv::Point p;
	};
	class vc
	{
	public:
		vc();
		~vc();

		void JawLibSegmentation(cv::Mat img);

	private:
		double _mapParam;						//相机映射参数
		Jaw _jawTemp;							//钳口左右模板，TODO：待测试是否对所有模板均有效
		// topC
		PosTemp _libMin;						//小钳口库定位标识
		PosTemp _libMid;						//中钳口库定位标识
		PosTemp _libMax;						//大钳口库定位标识
		// botC
	};
}

