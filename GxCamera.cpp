#include "GxCamera.h"


namespace d5vs {
	// 相机库初始化标识
	bool libIsInit = false;

	GxCamera::GxCamera(GxIAPICPP::gxstring sn) : _sn(sn), _deviceIsOpen(false), _deviceIsSnap(false) {
		_pSampleCaptureEventHandle = std::make_unique<CSampleCaptureEventHandler>();

		OpenDevice();
	}
	GxCamera::~GxCamera() {
		CloseDevice();
	}

	/**
	 * @brief 打开相机
	 */
	void GxCamera::OpenDevice() {
		if (_deviceIsOpen) {
			return;
		}

		try
		{
			//相机库初始化
			if (!libIsInit) {
				IGXFactory::GetInstance().Init();
				libIsInit = true;
			}

			// 枚举设备
			GxIAPICPP::gxdeviceinfo_vector vecDeviceInfo;
			IGXFactory::GetInstance().UpdateAllDeviceList(1000, vecDeviceInfo);
			std::set< GxIAPICPP::gxstring> snSet;
			if (vecDeviceInfo.empty()) {
				throw std::runtime_error("未检测到任何相机设备");
			}
			else {
				for (uint32_t i = 0; i < vecDeviceInfo.size(); i++) {
					snSet.insert(vecDeviceInfo[i].GetSN());
					std::cout << vecDeviceInfo[i].GetSN() << std::endl;
				}
			}

			// 打开设备
			if (_sn.empty()) {
				_deviceHandlePtr = IGXFactory::GetInstance().OpenDeviceBySN(vecDeviceInfo[0].GetSN(), GX_ACCESS_EXCLUSIVE);
			}
			else {
				if (snSet.find(_sn) != snSet.end()) {
					_deviceHandlePtr = IGXFactory::GetInstance().OpenDeviceBySN(_sn, GX_ACCESS_EXCLUSIVE);
					_deviceIsOpen = true;
				}
				else {
					throw std::runtime_error("无法打开该设备_SN：");
				}
			}

			// 获取属性控制器
			_deviceFeatureControlPtr = _deviceHandlePtr->GetRemoteFeatureControl();

			// 设置采集模式-连续采集
			_deviceFeatureControlPtr->GetEnumFeature("AcquisitionMode")->SetValue("Continuous");

			// 设置触发模式-关
			_deviceFeatureControlPtr->GetEnumFeature("TriggerMode")->SetValue("Off");
			//_deviceFeatureControlPtr->GetEnumFeature("TriggerSource")->SetValue("Software");

			int nCount = _deviceHandlePtr->GetStreamCount();
			if (nCount > 0) {
				// 开启数据流
				_deviceStreamPtr = _deviceHandlePtr->OpenStream(0);
				_deviceStreamFeatureControlPtr = _deviceStreamPtr->GetFeatureControl();
			}
			else {
				throw std::runtime_error("未发现数据流");
			}

			_deviceIsOpen = true;
		}
		catch (CGalaxyException& e)
		{
			std::cout << "error code: " << e.GetErrorCode() << std::endl;
			std::cout << "error info: " << e.what() << std::endl;
			throw e;
		}
	}

	/**
	 * @brief 关闭相机
	 */
	void GxCamera::CloseDevice() {
		if (!_deviceIsOpen) {
			return;
		}

		try
		{
			if (_deviceIsSnap) {
				// 停止采集
				_deviceFeatureControlPtr->GetCommandFeature("AcquisitionStop")->Execute();
				_deviceStreamPtr->StopGrab();
				_deviceStreamPtr->UnregisterCaptureCallback();
			}
		}
		catch (CGalaxyException& e)
		{
			std::cout << "error code: " << e.GetErrorCode() << std::endl;
			std::cout << "error info: " << e.what() << std::endl;
			throw e;
		}
		// 关闭流对象
		try
		{
			_deviceStreamPtr->Close();
		}
		catch (CGalaxyException& e)
		{
			std::cout << "error code: " << e.GetErrorCode() << std::endl;
			std::cout << "error info: " << e.what() << std::endl;
			throw e;
		}
		// 关闭设备
		try
		{
			_deviceHandlePtr->Close();
			_deviceIsOpen = false;
		}
		catch (CGalaxyException& e)
		{
			std::cout << "error code: " << e.GetErrorCode() << std::endl;
			std::cout << "error info: " << e.what() << std::endl;
			throw e;
		}

	}

	/**
	 * @brief 开启采集流
	 */
	void GxCamera::StartSnap() {
		if (!_deviceIsOpen) {
			throw std::runtime_error("设备未打开");
		}

		if (_deviceIsSnap) {
			return;
		}

		try
		{
			// 待设置，优先最新图像
			_deviceStreamFeatureControlPtr->GetEnumFeature("StreamBufferHandlingMode")->SetValue("OldestFirst");

			_deviceStreamPtr->RegisterCaptureCallback(_pSampleCaptureEventHandle.get(), this);
		}
		catch (CGalaxyException& e)
		{
			std::cout << "error code: " << e.GetErrorCode() << std::endl;
			std::cout << "error info: " << e.what() << std::endl;
			throw e;
		}

		try
		{
			// 开启流层采集
			_deviceStreamPtr->StartGrab();
		}
		catch (CGalaxyException& e)
		{
			_deviceStreamPtr->UnregisterCaptureCallback();

			std::cout << "error code: " << e.GetErrorCode() << std::endl;
			std::cout << "error info: " << e.what() << std::endl;
			throw e;
		}

		try
		{
			// 发送采集命令
			_deviceFeatureControlPtr->GetCommandFeature("AcquisitionStart")->Execute();
			_deviceIsSnap = true;
			Sleep(100);
		}
		catch (CGalaxyException& e)
		{
			_deviceStreamPtr->StopGrab();
			_deviceStreamPtr->UnregisterCaptureCallback();

			std::cout << "error code: " << e.GetErrorCode() << std::endl;
			std::cout << "error info: " << e.what() << std::endl;
			throw e;
		}
	}

	/**
	 * @brief 关闭采集流
	 */
	void GxCamera::StopSnap() {
		if (!_deviceIsOpen || !_deviceIsSnap) {
			return;
		}

		try
		{
			// 发送停止采集命令
			_deviceFeatureControlPtr->GetCommandFeature("AcquisitionStop")->Execute();
			// 关闭流层采集
			_deviceStreamPtr->StopGrab();
			// 注销回调函数
			_deviceStreamPtr->UnregisterCaptureCallback();
			// 采集标识符-关
			_deviceIsSnap = false;
		}
		catch (CGalaxyException& e)
		{
			std::cout << "error code: " << e.GetErrorCode() << std::endl;
			std::cout << "error info: " << e.what() << std::endl;
			throw e;
		}
	}

	bool GxCamera::IsOpen() const {
		return _deviceIsOpen;
	}

	bool GxCamera::IsSnap() const {
		return _deviceIsSnap;
	}

	/**
	 * @brief 采集最后一帧
	 * @return 灰度图
	 */
	cv::Mat GxCamera::Read() {
		std::lock_guard<std::mutex> lock(frameMutex);

		if (_lastFrame.empty())
			throw std::runtime_error("未读取到图片");

		return _lastFrame;
	}

	/**
	 * @brief 相机回调采集函数
	 * @param imgPtr
	 */
	void GxCamera::_SaveLastPictureToMat(CImageDataPointer& imgPtr) {
		if (isProcessing.exchange(true)) return;
		std::lock_guard<std::mutex> lock(frameMutex);

		if (GX_FRAME_STATUS_SUCCESS == imgPtr->GetStatus()) {
			int width = static_cast<int>(imgPtr->GetWidth());
			int height = static_cast<int>(imgPtr->GetHeight());
			void* imgBuffer = imgPtr->GetBuffer();
			if (imgBuffer != nullptr) {
				cv::Mat temp = cv::Mat(height, width, CV_8U, imgBuffer);
				_lastFrame = temp.clone();
			}
		}
	}
}