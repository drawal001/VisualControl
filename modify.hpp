#pragma once
#include "LibraryVision.h"

void M_GetPosTemp(cv::Mat img) {
	std::string winName = "Modify";
	cv::namedWindow(winName, cv::WINDOW_NORMAL);
	cv::resizeWindow(winName, cv::Size(1920 / 2, 600));
	if (img.empty()) {
		throw std::runtime_error("sss");
	}
	cv::Rect rect1 = cv::Rect(330, 630, 130, 130);
	cv::Rect rect2 = cv::Rect(330 + 604, 630, 130, 130);
	cv::Rect rect3 = cv::Rect(330 + 604 * 2, 630, 130, 130);
	cv::Mat roi1 = img(rect1).clone();
	cv::Mat roi2 = img(rect2).clone();
	cv::Mat roi3 = img(rect3).clone();
	cv::imwrite("./images/output/postempmin.png", roi1);
	cv::imwrite("./images/output/postempmid.png", roi2);
	cv::imwrite("./images/output/postempmax.png", roi3);
	cv::rectangle(img, rect1, cv::Scalar(0, 0, 255), 2);
	cv::rectangle(img, rect2, cv::Scalar(0, 0, 255), 2);
	cv::rectangle(img, rect3, cv::Scalar(0, 0, 255), 2);
	cv::imshow(winName, img);
	cv::waitKey(0);
}

void T_LibSegmentation(cv::Mat img, d5vs::LibraryVision vc) {
	std::string winName = "Test";
	cv::namedWindow(winName, cv::WINDOW_NORMAL);
	cv::resizeWindow(winName, cv::Size(1920 / 2, 600));
	vc.JawLibSegmentation(img);
	cv::rectangle(img, cv::Rect(vc.GetMinLibP().x - vc.GetLibWidth() / 2, 0, vc.GetLibWidth(), vc.GetLibHigh()), cv::Scalar(0, 0, 255), 2);
	cv::rectangle(img, cv::Rect(vc.GetMidLibP().x - vc.GetLibWidth() / 2, 0, vc.GetLibWidth(), vc.GetLibHigh()), cv::Scalar(0, 0, 255), 2);
	cv::rectangle(img, cv::Rect(vc.GetMaxLibP().x - vc.GetLibWidth() / 2, 0, vc.GetLibWidth(), vc.GetLibHigh()), cv::Scalar(0, 0, 255), 2);
	cv::imshow(winName, img);
	cv::waitKey(0);
}

void M_GetJawTemp(cv::Mat img) {
	std::string winName = "Modify";
	cv::namedWindow(winName, cv::WINDOW_NORMAL);
	cv::resizeWindow(winName, cv::Size(1920 / 2, 600));
	if (img.empty()) {
		throw std::runtime_error("Í¼Æ¬Îª¿Õ");
	}
	cv::Mat gray;
	if (img.channels() == 3) {
		cv::cvtColor(img, gray, cv::COLOR_BGR2GRAY);
	}
	else gray = img.clone();

	cv::Rect rect1 = cv::Rect(999 - 160, 900, 45, 160);
	cv::Rect rect2 = cv::Rect(999 + 160 - 45, 335, 45, 160);
	cv::Mat roi1 = img(rect1).clone();
	cv::Mat roi2 = img(rect2).clone();
	cv::imwrite("./images/output/jawtempL.png", roi1);
	cv::imwrite("./images/output/jawtempR.png", roi2);

	cv::rectangle(img, rect1, cv::Scalar(0, 0, 255), 2);
	cv::rectangle(img, rect2, cv::Scalar(0, 0, 255), 2);
	cv::imshow(winName, img);
	cv::waitKey(0);


}