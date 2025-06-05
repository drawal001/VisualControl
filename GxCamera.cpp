#include "GxCamera.h"


namespace d5vs {
	// ������ʼ����ʶ
	bool libIsInit = false;

	GxCamera::GxCamera(GxIAPICPP::gxstring sn) : _sn(sn), _deviceIsOpen(false), _deviceIsSnap(false) {
		_pSampleCaptureEventHandle = std::make_unique<CSampleCaptureEventHandler>();

		OpenDevice();
	}
	GxCamera::~GxCamera() {
		CloseDevice();
	}

	/**
	 * @brief �����
	 */
	void GxCamera::OpenDevice() {
		if (_deviceIsOpen) {
			return;
		}

		try
		{
			//������ʼ��
			if (!libIsInit) {
				IGXFactory::GetInstance().Init();
				libIsInit = true;
			}

			// ö���豸
			GxIAPICPP::gxdeviceinfo_vector vecDeviceInfo;
			IGXFactory::GetInstance().UpdateAllDeviceList(1000, vecDeviceInfo);
			std::set< GxIAPICPP::gxstring> snSet;
			if (vecDeviceInfo.empty()) {
				throw std::runtime_error("δ��⵽�κ�����豸");
			}
			else {
				for (uint32_t i = 0; i < vecDeviceInfo.size(); i++) {
					snSet.insert(vecDeviceInfo[i].GetSN());
					std::cout << vecDeviceInfo[i].GetSN() << std::endl;
				}
			}

			// ���豸
			if (_sn.empty()) {
				_deviceHandlePtr = IGXFactory::GetInstance().OpenDeviceBySN(vecDeviceInfo[0].GetSN(), GX_ACCESS_EXCLUSIVE);
			}
			else {
				if (snSet.find(_sn) != snSet.end()) {
					_deviceHandlePtr = IGXFactory::GetInstance().OpenDeviceBySN(_sn, GX_ACCESS_EXCLUSIVE);
					_deviceIsOpen = true;
				}
				else {
					throw std::runtime_error("�޷��򿪸��豸_SN��");
				}
			}

			// ��ȡ���Կ�����
			_deviceFeatureControlPtr = _deviceHandlePtr->GetRemoteFeatureControl();

			// ���òɼ�ģʽ-�����ɼ�
			_deviceFeatureControlPtr->GetEnumFeature("AcquisitionMode")->SetValue("Continuous");

			// ���ô���ģʽ-��
			_deviceFeatureControlPtr->GetEnumFeature("TriggerMode")->SetValue("Off");
			//_deviceFeatureControlPtr->GetEnumFeature("TriggerSource")->SetValue("Software");

			int nCount = _deviceHandlePtr->GetStreamCount();
			if (nCount > 0) {
				// ����������
				_deviceStreamPtr = _deviceHandlePtr->OpenStream(0);
				_deviceStreamFeatureControlPtr = _deviceStreamPtr->GetFeatureControl();
			}
			else {
				throw std::runtime_error("δ����������");
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
	 * @brief �ر����
	 */
	void GxCamera::CloseDevice() {
		if (!_deviceIsOpen) {
			return;
		}

		try
		{
			if (_deviceIsSnap) {
				// ֹͣ�ɼ�
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
		// �ر�������
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
		// �ر��豸
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
	 * @brief �����ɼ���
	 */
	void GxCamera::StartSnap() {
		if (!_deviceIsOpen) {
			throw std::runtime_error("�豸δ��");
		}

		if (_deviceIsSnap) {
			return;
		}

		try
		{
			// �����ã���������ͼ��
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
			// ��������ɼ�
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
			// ���Ͳɼ�����
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
	 * @brief �رղɼ���
	 */
	void GxCamera::StopSnap() {
		if (!_deviceIsOpen || !_deviceIsSnap) {
			return;
		}

		try
		{
			// ����ֹͣ�ɼ�����
			_deviceFeatureControlPtr->GetCommandFeature("AcquisitionStop")->Execute();
			// �ر�����ɼ�
			_deviceStreamPtr->StopGrab();
			// ע���ص�����
			_deviceStreamPtr->UnregisterCaptureCallback();
			// �ɼ���ʶ��-��
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
	 * @brief �ɼ����һ֡
	 * @return �Ҷ�ͼ
	 */
	cv::Mat GxCamera::Read() {
		std::lock_guard<std::mutex> lock(frameMutex);

		if (_lastFrame.empty())
			throw std::runtime_error("δ��ȡ��ͼƬ");

		return _lastFrame;
	}

	/**
	 * @brief ����ص��ɼ�����
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