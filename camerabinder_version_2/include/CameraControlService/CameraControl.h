// Test fixture for client focused binder tests
#ifndef CAMERA_CONTROL_H
#define CAMERA_CONTROL_H
#define LOG_NDEBUG 0

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

#include <camera/CameraMetadata.h>
#include <android/hardware/ICameraService.h>
#include <android/hardware/ICameraServiceListener.h>
#include <android/hardware/BnCameraServiceListener.h>
#include <android/hardware/camera2/ICameraDeviceUser.h>
#include <android/hardware/camera2/ICameraDeviceCallbacks.h>
#include <android/hardware/camera2/BnCameraDeviceCallbacks.h>
#include <camera/camera2/CaptureRequest.h>
#include <camera/camera2/OutputConfiguration.h>
#include <camera/camera2/SubmitInfo.h>

//与surface相关
#include <gui/BufferItemConsumer.h>
#include <gui/IGraphicBufferProducer.h>
#include <gui/Surface.h>
#include <gui/ISurfaceComposer.h>
#include <gui/SurfaceComposerClient.h>
#include <ui/DisplayInfo.h>
//与debug相关
#include <media/stagefright/foundation/AString.h>
#include <media/stagefright/foundation/ADebug.h>
#include <media/stagefright/foundation/ABuffer.h>
#include <media/stagefright/foundation/AHandler.h>
#include <media/stagefright/foundation/AMessage.h>

#include "CameraControlService/CameraCallback.cpp"
#include "CameraControlService/CameraListener.cpp"
#include "CameraControlService/BufferQueueListener.h"
using namespace android;

class CameraControl:
public RefBase{
private:
    sp<hardware::ICameraService> service;
	sp<hardware::camera2::ICameraDeviceUser> device;
	sp<IServiceManager> sm;
	sp<IBinder> binder;
	//设置传入的camera
    int32_t numCameras;
	//流id
	status_t streamId;
	//相关信息
	hardware::camera2::utils::SubmitInfo info;
	int32_t cameraId;
    sp<TestCameraServiceListener> serviceListener;
	CameraMetadata metadata;
	sp<TestCameraDeviceCallbacks> callbacks;
	hardware::camera2::CaptureRequest request;
	int64_t lastFrameNumber = 0;
public:
	CameraControl()
	{
		//ALOGD(" CameraControl creater");
		//resetVariables();
	}
	~CameraControl()
	{
		//ALOGD("CameraControl delete");
	}

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