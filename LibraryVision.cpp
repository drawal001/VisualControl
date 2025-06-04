#include "LibraryVision.h"

namespace d5vs {

	LibraryVision::LibraryVision() : _libWidth(450), _libHigh(650), _mapParam(0.0115369) {
		// posTemp
		_libMin.posTempImg = cv::imread("./models/posTemp/postempmin.png", cv::IMREAD_GRAYSCALE);
		_libMid.posTempImg = cv::imread("./models/posTemp/postempmid.png", cv::IMREAD_GRAYSCALE);
		_libMax.posTempImg = cv::imread("./models/posTemp/postempmax.png", cv::IMREAD_GRAYSCALE);

		// jawTempMid ���ܿ���ͨ�� ������
		HalconCpp::ReadShapeModel("./models/jawTemp/Temp_DL.shm", &_jawTemp.tempL);
		HalconCpp::ReadShapeModel("./models/jawTemp/Temp_DR.shm", &_jawTemp.tempR);
	}
	LibraryVision::~LibraryVision() {}

	/**
	 * @brief ͨ��ģ��ƥ��ָ�ǯ�ڿ⣬����¼��λ��
	 *		  �ú����ڳ���ʼ�Լ�����ƶ������Ҫ����ʹ��
	 * @param img
	 */
	void LibraryVision::JawLibSegmentation(cv::Mat img) {
		if (img.empty()) {
			throw std::runtime_error("����Ϊ��");
		}
		cv::Mat gray;
		if (img.channels() == 3) {
			cv::cvtColor(img, gray, cv::COLOR_BGR2GRAY);
		}
		else gray = img.clone();

		cv::Mat res;
		cv::Point minLoc, maxLoc;
		double minVal, maxVal;

		// min
		cv::matchTemplate(gray, _libMin.posTempImg, res, cv::TM_CCOEFF_NORMED);
		cv::minMaxLoc(res, &minVal, &maxVal, &minLoc, &maxLoc);
		_libMin.p = cv::Point(maxLoc.x + 65, maxLoc.y + 65);

		// mid
		cv::matchTemplate(gray, _libMid.posTempImg, res, cv::TM_CCOEFF_NORMED);
		cv::minMaxLoc(res, &minVal, &maxVal, &minLoc, &maxLoc);
		_libMid.p = cv::Point(maxLoc.x + 65, maxLoc.y + 65);

		// max
		cv::matchTemplate(gray, _libMax.posTempImg, res, cv::TM_CCOEFF_NORMED);
		cv::minMaxLoc(res, &minVal, &maxVal, &minLoc, &maxLoc);
		_libMax.p = cv::Point(maxLoc.x + 65, maxLoc.y + 65);
	}

	/**
	 * @brief ��λǯ����������
	 * @param ho_img halcon����ͼƬ
	 * @param lib ǯ�ڿ�����
	 * @return {x, y, angle, flag}
	 *			flag = 0 �� ����ǯ�ھ�ʶ��
	 *			flag = 1 �� �޷�ʶ��һ��  ���Խ���
	 *			flag = 2 �� �޷�ʶ������  �޷�����  ����ֹ����
	 */
	JawPos LibraryVision::GetJawPos(HalconCpp::HObject ho_img, Lib lib) {
		/*
			1��Ҫ�ж����ĸ�ǯ��
			2���Ƿ���֮ǰ��ǯ��
			3������ǣ���������ʷ����
			4��������ǣ������ǯ�ڳ�ʼ��������Χ
			5����ʷǯ�ڱ���Ϊ����ʼ�� -1����ʾ����ʷ��
								min 0��
								mid 1��
								max 2��
		*/
		using namespace HalconCpp;
		HObject ho_search_ROI_DL, ho_search_ROI_DR, ho_ROI_DL, ho_ROI_DR, ho_init_search_rect, ho_ImageReduced;
		HTuple hv_start, hv_range, hv_Height_DT, hv_Width_DT, hv_Height_DS, hv_Width_DS;

		//��������
		hv_start = -0.131;
		hv_range = 0.262;

		//ģ��size
		hv_Height_DT = 160;
		hv_Width_DT = 45;

		//����size
		hv_Height_DS = 300;
		hv_Width_DS = 150;

		//������ʷ����
		static HTuple hv_Last_Row_DL = 1000.0, hv_Last_Col_DL = 1000.0, hv_Last_Angle_DL = 0.0;
		static HTuple hv_Last_Row_DR = 1000.0, hv_Last_Col_DR = 1000.0, hv_Last_Angle_DR = 0.0;
		static int lastLib = -1;

		HTuple hv_Row_DL, hv_Col_DL, hv_Angle_DL, hv_Score_DL;
		HTuple hv_Row_DR, hv_Col_DR, hv_Angle_DR, hv_Score_DR;

		int flag = 0;

		if (lib != lastLib) {
			if (lib == Lib::minLib) {
				GenRectangle1(&ho_init_search_rect, 0, _libMin.p.x - _libWidth / 2, _libHigh, _libMin.p.x + _libWidth / 2);
			}
			else if (lib == Lib::midLib) {
				GenRectangle1(&ho_init_search_rect, 0, _libMid.p.x - _libWidth / 2, _libHigh, _libMid.p.x + _libWidth / 2);
			}
			else if (lib == Lib::maxLib) {
				GenRectangle1(&ho_init_search_rect, 0, _libMax.p.x - _libWidth / 2, _libHigh, _libMax.p.x + _libWidth / 2);
			}
			else throw std::runtime_error("�޷�ȷ����ǯ��");

			ReduceDomain(ho_img, ho_init_search_rect, &ho_ImageReduced);
			FindShapeModel(ho_ImageReduced, _jawTemp.tempL, hv_start, hv_range, 0.7, 1, 0.5,
				(HTuple("least_squares").Append("max_deformation 2")), 0, 0.9, &hv_Row_DL,
				&hv_Col_DL, &hv_Angle_DL, &hv_Score_DL);
			ReduceDomain(ho_img, ho_init_search_rect, &ho_ImageReduced);
			FindShapeModel(ho_ImageReduced, _jawTemp.tempR, hv_start, hv_range, 0.7, 1, 0.5,
				(HTuple("least_squares").Append("max_deformation 2")), 0, 0.9, &hv_Row_DR,
				&hv_Col_DR, &hv_Angle_DR, &hv_Score_DR);

			if (hv_Score_DL.Length() > 0 && hv_Score_DL.D() >= 0.7)
			{
				hv_Last_Row_DL = hv_Row_DL;
				hv_Last_Col_DL = hv_Col_DL;
				hv_Last_Angle_DL = hv_Angle_DL;
			}
			else throw std::runtime_error("�޷�ʶ��ǯ��");
			if (hv_Score_DR.Length() > 0 && hv_Score_DR.D() >= 0.7)
			{
				hv_Last_Row_DR = hv_Row_DR;
				hv_Last_Col_DR = hv_Col_DR;
				hv_Last_Angle_DR = hv_Angle_DR;
			}
			else throw std::runtime_error("�޷�ʶ��ǯ��");

			lastLib = lib;
		}
		else {
			GenRectangle2(&ho_search_ROI_DL, hv_Last_Row_DL, hv_Last_Col_DL, hv_Last_Angle_DL,
				hv_Width_DS / 2, hv_Height_DS / 2);
			GenRectangle2(&ho_search_ROI_DR, hv_Last_Row_DR, hv_Last_Col_DR, hv_Last_Angle_DR,
				hv_Width_DS / 2, hv_Height_DS / 2);
			ReduceDomain(ho_img, ho_search_ROI_DL, &ho_ROI_DL);
			ReduceDomain(ho_img, ho_search_ROI_DR, &ho_ROI_DR);
			FindShapeModel(ho_ROI_DL, _jawTemp.tempL, hv_start, hv_range, 0.7, 1, 0.5, (HTuple("least_squares").Append("max_deformation 2")),
				0, 0.9, &hv_Row_DL, &hv_Col_DL, &hv_Angle_DL, &hv_Score_DL);
			FindShapeModel(ho_ROI_DR, _jawTemp.tempR, hv_start, hv_range, 0.7, 1, 0.5, (HTuple("least_squares").Append("max_deformation 2")),
				0, 0.9, &hv_Row_DR, &hv_Col_DR, &hv_Angle_DR, &hv_Score_DR);

			if (hv_Score_DL.Length() > 0 && hv_Score_DL.D() >= 0.7)
			{
				hv_Last_Row_DL = hv_Row_DL;
				hv_Last_Col_DL = hv_Col_DL;
				hv_Last_Angle_DL = hv_Angle_DL;
			}
			else flag++;
			if (hv_Score_DR.Length() > 0 && hv_Score_DR.D() >= 0.7)
			{
				hv_Last_Row_DR = hv_Row_DR;
				hv_Last_Col_DR = hv_Col_DR;
				hv_Last_Angle_DR = hv_Angle_DR;
			}
			else flag++;
		}

		HTuple hv_Angle = (hv_Last_Angle_DR + hv_Last_Angle_DL) * 0.5;
		HTuple hv_Row = (hv_Last_Row_DL + hv_Last_Row_DR) * 0.5;
		HTuple hv_Col = (hv_Last_Col_DL + hv_Last_Col_DR) * 0.5;

		return { hv_Col.D(), hv_Row.D(),hv_Angle.D(), flag };
	}

	// �����ӿ�
	cv::Point LibraryVision::GetMaxLibP() { return _libMax.p; }
	cv::Point LibraryVision::GetMidLibP() { return _libMid.p; }
	cv::Point LibraryVision::GetMinLibP() { return _libMin.p; }
	int LibraryVision::GetLibWidth() { return _libWidth; }
	int LibraryVision::GetLibHigh() { return _libHigh; }
}