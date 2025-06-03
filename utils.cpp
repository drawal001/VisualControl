#include "utils.h"

namespace d5vs {
	/**
	 * @brief 获取相机映射参数
	 * @param img 带有棋盘标定板的图片
	 * @param size 棋盘规格
	 * @param realDis 棋盘格边长，单位mm
	 * @return
	 */
	float GetMapParam(cv::Mat img, cv::Size size, float realDis) {
		std::string winName = "Calibration_broard";
		std::vector<cv::Point2f> corner;

		cv::Mat gray;
		if (img.channels() == 3)
			cv::cvtColor(img, gray, cv::COLOR_BGR2GRAY);
		else
			gray = img.clone();

		// 棋盘标定

		if (!cv::findChessboardCorners(gray, size, corner)) {
			throw std::runtime_error("无法找到棋盘角点");
		}
		const cv::TermCriteria criteria{ cv::TermCriteria::EPS | cv::TermCriteria::COUNT, 30, 0.001 };
		cv::cornerSubPix(gray, corner, cv::Size(7, 7), cv::Size(-1, -1), criteria);
		cv::drawChessboardCorners(gray, size, corner, true);
		cv::imshow(winName, gray);
		cv::waitKey(0);

		// 映射参数计算
		float sum = 0.0f;
		cv::Point2f lastPoint;
		for (int i = 0; i < size.width; ++i) {
			for (int j = 0; j < size.height; ++j) {
				if (j == 0) {
					lastPoint = corner[j * size.width + i];
					continue;
				}
				auto a = sqrt(powf(corner[j * size.width + i].x - lastPoint.x, 2) + powf(corner[j * size.width + i].y - lastPoint.y, 2));
				sum += a;
				lastPoint = corner[j * size.width + i];
			}
		}
		return realDis * size.width * (size.height - 1) / sum;
	}

	/**
	 * @brief 将OpenCV Mat格式的图片转换成Halcon HObject格式
	 * @param img
	 * @return
	 */
	HalconCpp::HObject Mat2HImage(cv::Mat img) {
		if (img.empty() || img.channels() != 1) {
			throw std::runtime_error("图片格式错误");
		}
		int width = img.cols, height = img.rows;
		uchar* temp = new uchar[height * width];
		memcpy(temp, img.data, height * width);
		HalconCpp::HObject ho_img;
		HalconCpp::GenImage1(&ho_img, "byte", width, height, (Hlong)(temp));
		delete[] temp;
		return ho_img;
	}

	/**
	 * @brief 将Halcon HObject格式转换为OpenCV Mat格式
	 * @param img
	 * @return
	 */
	cv::Mat HImage2Mat(HalconCpp::HObject img) {
		HalconCpp::HTuple channels;
		HalconCpp::CountChannels(img, &channels);
		if (channels.I() != 1) {
			throw std::runtime_error("图片格式错误");
		}
		HalconCpp::HTuple hv_Pointer, hv_type, width, height;
		HalconCpp::GetImagePointer1(img, &hv_Pointer, &hv_type, &width, &height);
		int w = width.I();
		int h = height.I();
		int size = w * h;
		cv::Mat cv_img = cv::Mat::zeros(h, w, CV_8UC1);
		memcpy(cv_img.data, (void*)(hv_Pointer.L()), size);
		return cv_img;
	}

	/**
	 * @brief 相机图片显示与保存
	 *        key: Esc  -- 退出
	 *		       空格 -- 保存单帧图片
	 *             回车 -- 开始记录视频，再按一次停止记录
	 * @param camera
	 */
	void GetAndSaveImg(GxCamera& camera) {
		std::string winName = "camera";
		cv::namedWindow(winName, cv::WINDOW_NORMAL);
		auto now = std::chrono::system_clock::now();
		auto ms = std::chrono::duration_cast<std::chrono::microseconds>(now.time_since_epoch()).count();
		std::string path = "./images/" + std::to_string(ms);
		if (!std::filesystem::exists(path)) {
			std::filesystem::create_directories(path + "/imgs");
			std::filesystem::create_directories(path + "/videos");
		}
		cv::Mat img;
		int key;
		while (1) {
			img = camera.Read();
			cv::imshow(winName, img);
			cv::resizeWindow(winName, img.cols / 2, img.rows / 2);
			key = cv::waitKey(1);
			if (key == 27) break;
			if (key == 32) {
				auto now1 = std::chrono::system_clock::now();
				ms = std::chrono::duration_cast<std::chrono::microseconds>(now1.time_since_epoch()).count();
				cv::imwrite(path + "/imgs/" + std::to_string(ms) + ".png", img);
			}
			if (key == 13) {
				auto now2 = std::chrono::system_clock::now();
				ms = std::chrono::duration_cast<std::chrono::microseconds>(now2.time_since_epoch()).count();
				std::string subDir = path + "/videos/" + std::to_string(ms);
				if (!std::filesystem::exists(subDir)) {
					std::filesystem::create_directories(subDir);
				}
				int count = 0;
				while (1) {
					if (cv::waitKey(1) == 13) break;
					img = camera.Read();
					cv::imshow(winName, img);
					cv::imwrite(subDir + "/" + std::to_string(count++) + ".png", img);
				}
			}
		}
	}


}