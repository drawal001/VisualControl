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
		double			_mapParam;						//���ӳ�����
		int				_libWidth;						//���
		int				_libHigh;						//���
		Jaw				_jawTemp;						//ǯ������ģ�壬TODO���������Ƿ������ģ�����Ч
		// topC
		PosTemp			_libMin;						//Сǯ�ڿⶨλ��ʶ
		PosTemp			_libMid;						//��ǯ�ڿⶨλ��ʶ
		PosTemp			_libMax;						//��ǯ�ڿⶨλ��ʶ
		// botC

	// �����ӿ�
	public:
		cv::Point GetMinLibP();
		cv::Point GetMidLibP();
		cv::Point GetMaxLibP();
		int GetLibWidth();
		int GetLibHigh();

	};
}

