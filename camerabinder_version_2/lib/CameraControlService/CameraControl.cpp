// Test fixture for client focused binder tests
#ifndef CAMERA_CONTROL_CPP
#define CAMERA_CONTROL_CPP
#define LOG_NDEBUG 0
#define LOG_TAG "CAMERA_CONTROL_CPP"

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

#include <unistd.h>
#include <stdint.h>
#include <utility>
#include <vector>
#include <map>
#include <algorithm>
#include "CameraControlService/CameraControl.h"
using namespace android;
    bool CameraControl::initCameraService() {
        ProcessState::self()->startThreadPool();
        sm = defaultServiceManager();
		binder::Status res;
        binder = sm->getService(String16("media.camera"));
        service = interface_cast<hardware::ICameraService>(binder);
        serviceListener = new TestCameraServiceListener();
        service->addListener(serviceListener);
		res = service->getNumberOfCameras(hardware::ICameraService::CAMERA_TYPE_ALL, &numCameras);
		if(res.isOk()){
			ALOGD("TJY:connectService successed and num of device is:%d",numCameras);	
			return true;
		}else{
			ALOGD("TJY:connectService failed and num of device is error");	
			return false;
		}
    }
	int CameraControl::getNumOfCamera(){
		return numCameras;
	}
	bool CameraControl::isSupportedApi2(int cameraNum){
		bool isSupported = false;
		cameraId = cameraNum;
		binder::Status res;
		res = service->supportsCameraApi(cameraId,
				hardware::ICameraService::API_VERSION_2, &isSupported);
		// We only care about binder calls for the Camera2 API.  Camera1 is deprecated.
		if (!isSupported) {
			ALOGD("TJY: %d not Supported_1",cameraId);	
			return false;
		}	
		return true;
	}
	
	bool CameraControl::initCamera(int cameraNum){
			// Check metadata binder call
		binder::Status res;
		cameraId = cameraNum;
		res = service->getCameraCharacteristics(cameraId, &metadata);	
		// Make sure we're available, or skip device tests otherwise
		int32_t s = serviceListener->getStatus(cameraId);
		if (s != ::android::hardware::ICameraServiceListener::STATUS_PRESENT) {
			ALOGD("TJY: %d not Supported_2 ",cameraId);	
			return false;
		}
		// Check connect binder calls
		callbacks = new TestCameraDeviceCallbacks();
		ALOGD("TJY:into connectDevice : %d",cameraId);
		res = service->connectDevice(callbacks, cameraId, String16("testCamera!"),
				hardware::ICameraService::USE_CALLING_UID, /*out*/&device);
		if(res.isOk()){
			ALOGD("TJY:connectDevice : %d successed",cameraId);
			return true;
		}else{
			ALOGD("TJY:connectDevice : %d error",cameraId);
			return false;
		}
	}
	


	status_t CameraControl::initRequest(sp<IGraphicBufferProducer> &gbProducer,int type) {
				//这一句大概是用来配置outputstreame？
		sp<Surface> surface(new Surface(gbProducer, false)); 		
		OutputConfiguration output(gbProducer, /*rotation*/0);
		binder::Status res;
		res = device->beginConfigure();
		if(res.isOk()){
			ALOGD("TJY:device %d configure   successed",cameraId);	
		}else{
			return UNKNOWN_ERROR ;
		}	
		status_t streamId;
		res = device->createStream(output, &streamId);
		if(res.isOk()){
			ALOGD("TJY:device %d createStream  successed",cameraId);	
		}else{
			return UNKNOWN_ERROR ;
		}	
		res = device->endConfigure(/*isConstrainedHighSpeed*/ false);
		if(res.isOk()){
			ALOGD("TJY:device %d endConfigure  successed",cameraId);	
		}else{
			return UNKNOWN_ERROR ;	
		}
		if(callbacks->hadError()){
			ALOGD("TJY:device %d callbacks  has error_1",cameraId);			
		}
		callbacks->clearStatus();
		//下面用来make request
		CameraMetadata requestTemplate;	
		//创建request的类型，preview 1；capture 2；record 3，snapshot 4......
		res = device->createDefaultRequest(/*preview template*/type,
				/*out*/&requestTemplate);
		if(res.isOk()){
			ALOGD("TJY:device %d create request successed",cameraId);	
		}	
		request.mMetadata = requestTemplate;
		request.mSurfaceList.add(surface);
		request.mIsReprocess = false;
		//lastFrameNumber = 0;
		callbacks->clearStatus();
		return OK;
    }
	
	status_t CameraControl::submitRequest() {
				//下面是提交request
		binder::Status res;
		ALOGD("TJY:device %d into submitinfos",cameraId);	
		res = device->submitRequest(request, /*streaming*/true, /*out*/&info);
		if(res.isOk()){
			ALOGD("TJY:device %d create submitRequest successed",cameraId);	
		}else{
			ALOGD("TJY:device %d  submitRequest error",cameraId);	
			return UNKNOWN_ERROR ;
		}
		if(callbacks->waitForStatus(TestCameraDeviceCallbacks::SENT_RESULT)){
			ALOGD("TJY:device %d waitForStatus is true",cameraId);	
		}
		
		if(info.mRequestId==0){
			ALOGD("TJY:device %d mRequestId is 0",cameraId);	
		}
		return OK;
	}
	
	status_t CameraControl::stopRequest(){
				// 停掉request
		binder::Status res;
		res = device->cancelRequest(info.mRequestId, /*out*/&lastFrameNumber);
		if(res.isOk()){
			ALOGD("TJY:device %d create cancelRequest successed",cameraId);	
		}else{
			ALOGD("TJY:device %d  cancelRequest error",cameraId);	
			return UNKNOWN_ERROR ;
		}
		callbacks->waitForIdle();
		if(callbacks->hadError()){
			ALOGD("TJY:device %d callbacks  has error_3",cameraId);			
		}	
		callbacks->clearStatus();
		return OK;
	}
	
	status_t CameraControl::disconnectCamera(){
		binder::Status res;
		//卸载所有的config？？？	
		res = device->deleteStream(streamId);
		if(res.isOk()){
			ALOGD("TJY:device %d create deleteStream successed",cameraId);	
		}else{
			ALOGD("TJY:device %d  deleteStream error",cameraId);
			return UNKNOWN_ERROR ;
		}
		res = device->endConfigure(/*isConstrainedHighSpeed*/ false);
		if(res.isOk()){
			ALOGD("TJY:device %d create endConfigure successed",cameraId);	
		}else{
			ALOGD("TJY:device %d  endConfigure error",cameraId);
			return UNKNOWN_ERROR ;
		}	
		if(callbacks->hadError()){
			ALOGD("TJY:device %d callbacks  has error_2",cameraId);			
		}		
		callbacks->clearStatus();
		ALOGD("TJY:into disconnectDevice : %d",cameraId);
		//关闭连接
		device->disconnect();
		return OK;
	}
	
	status_t CameraControl::deleteListner(){
		binder::Status res;
		res = service->removeListener(serviceListener);
		if(res.isOk()){
			ALOGD("TJY:service remove lisener successed");	
		}else{
			ALOGD("TJY:device %d  remove lisener successederror",cameraId);
			return UNKNOWN_ERROR ;
		}	
		return OK;
	}

#endif