#include "GxCamera.h"
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include "utils.h"
int main() {

	d5vs::GxCamera topCamera("FDD24020064");

	topCamera.StartSnap();

	d5vs::GetAndSaveImg(topCamera);


	return 0;
}