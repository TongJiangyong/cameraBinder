/**
类的使用说明：
客户端借助binder可使用的类，即服务器端功能的代理类的接口
**/
#ifndef I_CAMERA_CONTROL_SERVICE_CLIENT_H
#define I_CAMERA_CONTROL_SERVICE_CLIENT_H

#include <camera/android/hardware/ICamera.h>
#include <gui/IGraphicBufferProducer.h>
#include <binder/IInterface.h>
#include <binder/Parcel.h>
#include <media/mediarecorder.h>
#include <media/stagefright/foundation/AHandler.h>
#include <media/stagefright/foundation/AMessage.h>
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
#define STRING_BUFFER_LENGTH    4096

namespace android{

class ICameraControlServiceClient: public IInterface
{
public:
//这边这要继承和cameraControl相关的所有的东西......
//即，这一个DECLARE_META_INTERFACE非常重要，说明了需要实现的接口是哪一个....
	DECLARE_META_INTERFACE(CameraControlServiceClient);

	virtual bool initCameraService()=0;
	virtual int getNumOfCamera()=0;
	virtual bool isSupportedApi2(int cameraNum)=0;
	virtual bool initCamera(int cameraNum)=0;
	//在这里设置surface和request的相关信息
	virtual status_t initRequest(sp<IGraphicBufferProducer> &gbProducer,int type)=0;
    virtual status_t submitRequest()=0;
	virtual status_t stopRequest()=0;
	virtual status_t disconnectCamera()=0;
	virtual status_t deleteListner()=0;
};

// BnInterface.
class BnCameraControlServiceClient: public BnInterface<ICameraControlServiceClient>
{
public:

	//应该只要这个即可....
	virtual status_t onTransact(
		uint32_t code,
		const Parcel& data,
		Parcel* reply,
		uint32_t flags = 0);
};

}; // namespace android

#endif