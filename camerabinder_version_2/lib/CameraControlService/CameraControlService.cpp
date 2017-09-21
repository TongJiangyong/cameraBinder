// Test fixture for client focused binder tests
/**
���ʹ��˵����
�������˾���ʵ�֣�����Ϊ��Ҫ�������ط������˵Ĵ�����Ķ���
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

//��debug���
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
//ǿ���캯������Ϊ˽�к�������ֹֻ��new�����ֱ�Ӵ�������
CameraControlService::CameraControlService()
{
	ALOGD("CameraControlService()");	
}
CameraControlService::~CameraControlService()
{
	ALOGD("~CameraControlService()");
}

//��ʼ��HelloWorldService ��ϵͳע�����
//����ʵ�ַ���ӿڵķ���
//��������service�Ƿ���еķ���.....
void CameraControlService::instantiate()
{
    defaultServiceManager()->addService(String16("realtek.cameracontrolservice"),
                    new CameraControlService());
}
	


//���巵��client�ķ���������Ǻ���Ҫ��
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