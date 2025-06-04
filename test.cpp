#include "GxCamera.h"
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include "LibraryVision.h"
#include "utils.h"
#include "modify.hpp"
int main() {

	//d5vs::GxCamera topCamera("FDD24020064");

	//topCamera.StartSnap();

	//d5vs::GetAndSaveImg(topCamera);

	d5vs::LibraryVision vc;
	cv::Mat img = cv::imread("./images/1748922812361619/imgs/1748922829267547.png");
	M_GetJawTemp(img);
	//vc.JawLibSegmentation(img);
	//std::cout << vc.GetMidLibP().x << "   " << vc.GetMidLibP().y << std::endl;
	return 0;
}