#pragma once
#include <GalaxyIncludes.h>
#include <opencv2/core.hpp>
#include <set>
#include <iostream>
#include <mutex>

namespace d5vs {

	class GxCamera
	{
	public:
		GxCamera(GxIAPICPP::gxstring sn);
		~GxCamera();

		class CSampleCaptureEventHandler :public ICaptureEventHandler
		{

			void DoOnImageCaptured(CImageDataPointer& imgPtr, void* pUserParam)
			{
				try
				{
					GxCamera* camera = (GxCamera*)pUserParam;
					camera->_SaveLastPictureToMat(imgPtr);
				}
				catch (CGalaxyException& ex)
				{
					//do nothing
					std::cerr << ex.what() << std::endl;
					std::cerr << "Error Code: " << ex.GetErrorCode() << std::endl;
				}
				catch (const std::exception& ex)
				{
					//do nothing
					std::cerr << ex.what() << std::endl;
				}
			}
		};

		void OpenDevice();
		void CloseDevice();
		void StartSnap();
		void StopSnap();

		bool IsOpen() const;
		bool IsSnap() const;

		cv::Mat Read();

	private:

		void _SaveLastPictureToMat(CImageDataPointer& imgPtr);

		CGXDevicePointer _deviceHandlePtr;  // 设备句柄
		CGXStreamPointer _deviceStreamPtr;  // 设备流对象

		CGXFeatureControlPointer _deviceFeatureControlPtr; //设备属性控制器
		CGXFeatureControlPointer _deviceStreamFeatureControlPtr;  //设备流属性控制器
		std::unique_ptr<CSampleCaptureEventHandler> _pSampleCaptureEventHandle;  //回调指针

		bool _deviceIsOpen;  //设备是否打开
		bool _deviceIsSnap;  //设备是否开始采集

		GxIAPICPP::gxstring _sn;  //设备序列号

		//std::unique_ptr<void> _pFrameBuffer;
		cv::Mat _lastFrame{};  //最后一帧图片

		std::mutex frameMutex{};  // lastFrame 的读写锁
	};
}

