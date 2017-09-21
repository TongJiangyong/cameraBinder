/**
类的使用说明：
服务器端功能的代理类
**/
#ifndef CAMERA_CONTROL_CLIENT_CPP
#define CAMERA_CONTROL_CLIENT_CPP
#define LOG_NDEBUG 0
#define LOG_TAG "CAMERA_CONTROL_CLIENT_CPP"

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

#include <algorithm>
#include "CameraControlService/CameraControlServiceClient.h"

using namespace android;

CameraControlServiceClient::CameraControlServiceClient()
{	
	cameraControler = new CameraControl();
}
CameraControlServiceClient::~CameraControlServiceClient()
{
    pid_t id = gettid();
    ALOGD("~CameraControlServiceClient() tid %d",(int)id);
    cameraControler =NULL;
}

bool CameraControlServiceClient::initCameraService()
{
	return cameraControler->initCameraService();
}
int CameraControlServiceClient::getNumOfCamera()
{
	return cameraControler->getNumOfCamera();
}
bool CameraControlServiceClient::isSupportedApi2(int cameraNum)
{
	return cameraControler->isSupportedApi2(cameraNum);
}
bool CameraControlServiceClient::initCamera(int cameraNum)
{
	return cameraControler->initCamera(cameraNum);
}
//在这里设置surface和request的相关信息
status_t CameraControlServiceClient::initRequest(sp<IGraphicBufferProducer> &gbProducer,int type)
{
	return cameraControler->initRequest(gbProducer,type);
}
status_t CameraControlServiceClient::submitRequest()
{
	return cameraControler->submitRequest();
}
status_t CameraControlServiceClient::stopRequest()
{
	return cameraControler->stopRequest();
}
status_t CameraControlServiceClient::disconnectCamera()
{
	return cameraControler->disconnectCamera();
}
status_t CameraControlServiceClient::deleteListner()
{
	return cameraControler->deleteListner();
}

#endif