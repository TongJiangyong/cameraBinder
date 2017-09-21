/**
类的使用说明：
服务器端具体实现的代理接口，功能为客户端用来实例化，获取服务
**/
#ifndef I_CAMERA_CONTROL_SERVICE_H
#define I_CAMERA_CONTROL_SERVICE_H

#include <camera/android/hardware/ICamera.h>
#include <gui/IGraphicBufferProducer.h>
#include <binder/IInterface.h>
#include <binder/Parcel.h>
#include <media/mediarecorder.h>
#include <media/stagefright/foundation/AHandler.h>
#include <media/stagefright/foundation/AMessage.h>
#include "CameraControlService/ICameraControlServiceClient.h"

namespace android{
class ICameraControlService:public IInterface
{
public:
	DECLARE_META_INTERFACE(CameraControlService);
	virtual sp<ICameraControlServiceClient> create()=0;
	/***
		注意这里的虚函数一定要加 = 0否则会报编译错误.....日了狗了.....耗费了很多时间在这里....
	***/
	virtual status_t helloWorld(const char *str)=0;
};

// BnInterface.
class BnCameraControlService:public BnInterface<ICameraControlService>
{
public:
	virtual status_t onTransact(
		uint32_t code,
		const Parcel& data,
		Parcel* reply,
		uint32_t flags = 0);
};

}; // namespace android

#endif