#include "GxCamera.h"
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
int main() {

	d5vs::GxCamera topCamera("FDD24020064");

	topCamera.StartSnap();
	//Sleep(1000);
	cv::Mat img;
	while (1) {
		img = topCamera.Read();
		cv::imshow("test", img);
		int key = cv::waitKey(1);
		if (key == 27) {
			break;
		}
	}
	return 0;
}