/**
类的使用说明：
服务器端功能的代理类
**/
#ifndef CAMERA_CONTROL_SERVICE_CLIENT_H
#define CAMERA_CONTROL_SERVICE_CLIENT_H
#define LOG_NDEBUG 0
#define LOG_TAG "CAMERA_CONTROL_SERVICE_CLIENT_H"

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

#include "CameraControlService/ICameraControlServiceClient.h"
#include "CameraControlService/CameraControl.h"
using namespace android;
class CameraControlServiceClient:public BnCameraControlServiceClient{
	
private:
sp<CameraControl> cameraControler;

public:
CameraControlServiceClient();
~CameraControlServiceClient();
bool initCameraService();
int getNumOfCamera();
bool isSupportedApi2(int cameraNum);
bool initCamera(int cameraNum);
//在这里设置surface和request的相关信息
status_t initRequest(sp<IGraphicBufferProducer> &gbProducer,int type);
status_t submitRequest();
status_t stopRequest();
status_t disconnectCamera();
status_t deleteListner();

};
#endif