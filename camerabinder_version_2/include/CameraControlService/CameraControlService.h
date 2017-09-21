#ifndef CAMERA_CONTROL_SERVICE_H
#define CAMERA_CONTROL_SERVICE_H

#include <camera/android/hardware/ICamera.h>
#include <gui/IGraphicBufferProducer.h>
#include <binder/IInterface.h>
#include <binder/Parcel.h>
#include <CameraControlService/ICameraControlService.h>


namespace android{
class CameraControlService:public BnCameraControlService
{
public:
	//ǿ���캯������Ϊ˽�к�������ֹֻ��new�����ֱ�Ӵ�������
	//��ʼ��HelloWorldService ��ϵͳע�����
	static void instantiate();
	//����ʵ�ַ���ӿڵķ���
	//��������service�Ƿ���еķ���.....
	virtual status_t helloWorld(const char *str);
	//���巵��client�ķ���������Ǻ���Ҫ��
	sp<ICameraControlServiceClient> create();
	private:
	//ǿ���캯������Ϊ˽�к�������ֹֻ��new�����ֱ�Ӵ�������
	CameraControlService();
	virtual ~CameraControlService();
};

}; // namespace android

#endif