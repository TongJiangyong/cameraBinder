// Test fixture for client focused binder tests
/**
类的使用说明：
服务器端具体实现，功能为主要用来返回服务器端的代理类的对象
**/
#ifndef CAMERA_CONTROL_SERVICE_CPP
#define CAMERA_CONTROL_SERVICE_CPP
#define LOG_NDEBUG 0
#define LOG_TAG "CAMERA_CONTROL_SERVICE_CPP"

#include <binder/IInterface.h>
#include <binder/IServiceManager.h>
#include <binder/Parcel.h>
#include <binder/ProcessState.h>
#include <utils/Errors.h>
#include <utils/Log.h>
#include <utils/List.h>
#include <utils/String8.h>
#include <utils/String16.h>
#include <utils/Condition.h>
#include <utils/Mutex.h>
#include <system/graphics.h>
#include <hardware/gralloc.h>

//与debug相关
#include <media/stagefright/foundation/AString.h>
#include <media/stagefright/foundation/ADebug.h>
#include <media/stagefright/foundation/ABuffer.h>
#include <media/stagefright/foundation/AHandler.h>
#include <media/stagefright/foundation/AMessage.h>


#include <unistd.h>
#include <stdint.h>
#include <utility>
#include <vector>
#include <map>
#include <algorithm>
#include "CameraControlService/CameraControlService.h"
#include "CameraControlService/ICameraControlServiceClient.h"
#include "CameraControlService/CameraControlServiceClient.h"
namespace android{
using namespace android;

//private:
//强构造函数设置为私有函数，防止只用new运算符直接创建对象
CameraControlService::CameraControlService()
{
	ALOGD("CameraControlService()");	
}
CameraControlService::~CameraControlService()
{
	ALOGD("~CameraControlService()");
}

//初始化HelloWorldService 向系统注册服务
//具体实现服务接口的方法
//测试连接service是否可行的方法.....
void CameraControlService::instantiate()
{
    defaultServiceManager()->addService(String16("realtek.cameracontrolservice"),
                    new CameraControlService());
}
	


//定义返回client的方法，这个是很重要的
sp<ICameraControlServiceClient> CameraControlService::create()
{
	// TODOLATER: check reach limit or not..
	sp<ICameraControlServiceClient> bClient;
	sp<CameraControlService> instance = this;
	sp<CameraControlServiceClient> client(new CameraControlServiceClient());
    // refer to SurfaceFlinger.cpp SurfaceFlinger::createConnection
	bClient = client;
	return bClient;
}

status_t CameraControlService::helloWorld(const char* str){
	//LOGI("%s\n",str);
	printf("%s\n",str);
	return NO_ERROR;
}

} //namespace
#endif