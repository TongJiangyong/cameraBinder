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
	//强构造函数设置为私有函数，防止只用new运算符直接创建对象
	//初始化HelloWorldService 向系统注册服务
	static void instantiate();
	//具体实现服务接口的方法
	//测试连接service是否可行的方法.....
	virtual status_t helloWorld(const char *str);
	//定义返回client的方法，这个是很重要的
	sp<ICameraControlServiceClient> create();
	private:
	//强构造函数设置为私有函数，防止只用new运算符直接创建对象
	CameraControlService();
	virtual ~CameraControlService();
};

}; // namespace android

#endif