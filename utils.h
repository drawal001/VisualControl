#pragma once
#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>
#include "halconcpp/HalconCpp.h"
#include "GxCamera.h"
#include <chrono>
#include <filesystem>

namespace d5vs {
	float GetMapParam(cv::Mat img, cv::Size size, float realDis);
	HalconCpp::HObject Mat2HImage(cv::Mat img);
	cv::Mat HImage2Mat(HalconCpp::HObject img);
	void GetAndSaveImg(GxCamera& camera);
}
