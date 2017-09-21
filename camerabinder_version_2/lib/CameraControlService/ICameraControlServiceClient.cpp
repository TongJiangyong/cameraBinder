/**
类的使用说明：
客户端借助binder可使用的类，即服务器端功能的代理类的接口
**/
#ifndef I_CAMERA_CONTROL_SERVICE_CLIENT_CPP
#define I_CAMERA_CONTROL_SERVICE_CLIENT_CPP
#define LOG_TAG "I_CAMERA_CONTROL_SERVICE_CLIENT_CPP"

#include <utils/Errors.h>
#include <utils/String8.h>
#include <utils/Log.h>
#include <binder/Parcel.h>
#include <binder/IMemory.h>
#include "CameraControlService/ICameraControlServiceClient.h"
//这个类主要是客户端，会使用的类，可以看做cameraControlClient能被客户端代理使用的代理类.....

namespace android {
	/**
	bool initCameraService();
	int getNumOfCamera();
	bool isSupportedApi2(int cameraNum);
	bool initCamera(int cameraNum);
	//在这里设置surface和request的相关信息
	binder::Status initRequest(sp<IGraphicBufferProducer> &gbProducer,int type,sp<Surface> &surface);
    binder::Status submitRequest(hardware::camera2::utils::SubmitInfo &info);
	binder::Status stopRequest(hardware::camera2::utils::SubmitInfo &info);
	binder::Status disconnectCamera();
	binder::Status deleteListner();
	**/
//枚举的命令集合：
enum {
    INIT_CAMERA_SERVICE = IBinder::FIRST_CALL_TRANSACTION,
    GET_NUM_OF_CAMERA,
    IS_SUPPORT_API_2,
    INIT_CAMERA,
    INIT_REQUEST,
    SUBMIT_REQUEST,
    STOP_REQUEST,
    DISCONNECT_CAMERA,
    DELETE_LISTNER,
};

// BpInterface
class BpCameraControlServiceClient: public BpInterface<ICameraControlServiceClient>
{
public:
    BpCameraControlServiceClient(const sp<IBinder>& impl)
        : BpInterface<ICameraControlServiceClient>(impl)
    {
        ALOGD("Create BpCameraControlServiceClient");
    }

    ~BpCameraControlServiceClient()
    {
        ALOGD("~BpCameraControlServiceClient");
    }

    virtual bool initCameraService()
    {
        Parcel data, reply;
        data.writeInterfaceToken(ICameraControlServiceClient::getInterfaceDescriptor());
        remote()->transact(INIT_CAMERA_SERVICE, data, &reply);
        return (bool)reply.readInt32();
    }

    virtual int getNumOfCamera()
    {
        Parcel data, reply;
        data.writeInterfaceToken(ICameraControlServiceClient::getInterfaceDescriptor());
        remote()->transact(GET_NUM_OF_CAMERA, data, &reply);
        return reply.readInt32();
    }

    virtual bool isSupportedApi2(int cameraNum)
    {
        Parcel data, reply;
        data.writeInterfaceToken(ICameraControlServiceClient::getInterfaceDescriptor());
		data.writeInt32(cameraNum);
        remote()->transact(IS_SUPPORT_API_2, data, &reply);
        return (bool)reply.readInt32();
    }

    virtual bool initCamera(int cameraNum)
    {
        Parcel data, reply;
        data.writeInterfaceToken(ICameraControlServiceClient::getInterfaceDescriptor());
        data.writeInt32(cameraNum);
        remote()->transact(INIT_CAMERA, data, &reply);
        return (bool)reply.readInt32();
    }

    virtual status_t initRequest(sp<IGraphicBufferProducer> &gbProducer,int type)
    {
        Parcel data, reply;
        data.writeInterfaceToken(ICameraControlServiceClient::getInterfaceDescriptor());
		sp<IBinder> b(gbProducer->asBinder(gbProducer));
		data.writeStrongBinder(b);
		data.writeInt32(type);
		//Todo
		//这里可能出错.....
        remote()->transact(INIT_REQUEST, data, &reply);
        return (status_t)reply.readInt32();
    }

    virtual status_t submitRequest()
    {
        Parcel data, reply;
        data.writeInterfaceToken(ICameraControlServiceClient::getInterfaceDescriptor());
		//Todo
		//这里可能出错.....
		//data.write(&info, sizeof(hardware::camera2::utils::SubmitInfo));
        remote()->transact(SUBMIT_REQUEST, data, &reply);
        return (status_t)reply.readInt32();
    }

    virtual status_t stopRequest()
    {
        Parcel data, reply;
        data.writeInterfaceToken(ICameraControlServiceClient::getInterfaceDescriptor());
		//data.write(&info, sizeof(hardware::camera2::utils::SubmitInfo));
        remote()->transact(STOP_REQUEST, data, &reply);
        return (status_t)reply.readInt32();
    }

    virtual status_t disconnectCamera()
    {
        Parcel data, reply;
        data.writeInterfaceToken(ICameraControlServiceClient::getInterfaceDescriptor());
        remote()->transact(DISCONNECT_CAMERA, data, &reply);
        return (status_t)reply.readInt32();
    }

    virtual status_t deleteListner()
    {
        Parcel data, reply;
        data.writeInterfaceToken(ICameraControlServiceClient::getInterfaceDescriptor());
        remote()->transact(DELETE_LISTNER, data, &reply);
        return (status_t)reply.readInt32();
    }
};

IMPLEMENT_META_INTERFACE(CameraControlServiceClient, "realtek.CameraControlServiceClient");
/*
    INIT_CAMERA_SERVICE = IBinder::FIRST_CALL_TRANSACTION,
    GET_NUM_OF_CAMERA,
    IS_SUPPORT_API_2,
    INIT_CAMERA,
    INIT_REQUEST,
    SUBMIT_REQUEST,
    STOP_REQUEST,
    DISCONNECT_CAMERA,
    DELETE_LISTNER,
	*/
	/**
	virtual bool initCameraService();
	virtual int getNumOfCamera();
	virtual bool isSupportedApi2(int cameraNum);
	virtual bool initCamera(int cameraNum);
	//在这里设置surface和request的相关信息
	virtual binder::Status initRequest(sp<IGraphicBufferProducer> &gbProducer,int type,sp<Surface> &surface);
    virtual binder::Status submitRequest(hardware::camera2::utils::SubmitInfo &info);
	virtual binder::Status stopRequest(hardware::camera2::utils::SubmitInfo &info);
	virtual binder::Status disconnectCamera();
	virtual binder::Status deleteListner();
	**/
// BnInterface
status_t BnCameraControlServiceClient::onTransact(
    uint32_t code,
    const Parcel& data,
    Parcel* reply,
    uint32_t flags)
{
    switch(code){
        case INIT_CAMERA_SERVICE:{
            CHECK_INTERFACE(ICameraControlClient, data, reply);
            reply->writeInt32(initCameraService());
            return NO_ERROR;
        }break;
        case GET_NUM_OF_CAMERA:{
            CHECK_INTERFACE(ICameraControlClient, data, reply);
            reply->writeInt32(getNumOfCamera());
            return NO_ERROR;
        }break;
        case IS_SUPPORT_API_2:{
            CHECK_INTERFACE(ICameraControlClient, data, reply);
			int32_t cameraNum = data.readInt32();
            reply->writeInt32(isSupportedApi2(cameraNum));
            return NO_ERROR;
        }break;
        case INIT_CAMERA:{
            CHECK_INTERFACE(ICameraControlClient, data, reply);
			int32_t cameraNum = data.readInt32();
            reply->writeInt32(initCamera(cameraNum));
            return NO_ERROR;
        }break;
        case INIT_REQUEST:{
            CHECK_INTERFACE(ICameraControlClient, data, reply);
            sp<IGraphicBufferProducer> gbProducer =interface_cast<IGraphicBufferProducer>(data.readStrongBinder());
			int32_t type = data.readInt32();		
            reply->writeInt32(initRequest(gbProducer,type));
            return NO_ERROR;
        }break;
        case SUBMIT_REQUEST:{
            CHECK_INTERFACE(ICameraControlClient, data, reply);
			//hardware::camera2::utils::SubmitInfo info;
            //data.read(&info, sizeof(hardware::camera2::utils::SubmitInfo));
            reply->writeInt32(submitRequest());
            return NO_ERROR;
        }break;
        case STOP_REQUEST:{
            // TODO: check different parameter correctness.
            CHECK_INTERFACE(ICameraControlClient, data, reply);
			//hardware::camera2::utils::SubmitInfo info;
            //data.read(&info, sizeof(hardware::camera2::utils::SubmitInfo));
            reply->writeInt32(stopRequest());
            return NO_ERROR;
        }break;
        case DISCONNECT_CAMERA:{
            CHECK_INTERFACE(ICameraControlClient, data, reply);
            reply->writeInt32(disconnectCamera());
            return NO_ERROR;
        }break;
        case DELETE_LISTNER:{
            CHECK_INTERFACE(ICameraControlClient, data, reply);
            reply->writeInt32(deleteListner());
            return NO_ERROR;
        }break;
        default:
            return BBinder::onTransact(code, data, reply, flags);
    }
}


}// namespace android
#endif