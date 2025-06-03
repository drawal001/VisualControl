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
		double _mapParam;						//���ӳ�����
		Jaw _jawTemp;							//ǯ������ģ�壬TODO���������Ƿ������ģ�����Ч
		// topC
		PosTemp _libMin;						//Сǯ�ڿⶨλ��ʶ
		PosTemp _libMid;						//��ǯ�ڿⶨλ��ʶ
		PosTemp _libMax;						//��ǯ�ڿⶨλ��ʶ
		// botC
	};
}

