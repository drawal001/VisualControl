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

		CGXDevicePointer _deviceHandlePtr;  // �豸���
		CGXStreamPointer _deviceStreamPtr;  // �豸������

		CGXFeatureControlPointer _deviceFeatureControlPtr; //�豸���Կ�����
		CGXFeatureControlPointer _deviceStreamFeatureControlPtr;  //�豸�����Կ�����
		std::unique_ptr<CSampleCaptureEventHandler> _pSampleCaptureEventHandle;  //�ص�ָ��

		bool _deviceIsOpen;  //�豸�Ƿ��
		bool _deviceIsSnap;  //�豸�Ƿ�ʼ�ɼ�

		GxIAPICPP::gxstring _sn;  //�豸���к�

		//std::unique_ptr<void> _pFrameBuffer;
		cv::Mat _lastFrame{};  //���һ֡ͼƬ

		std::mutex frameMutex{};  // lastFrame �Ķ�д��
	};
}

