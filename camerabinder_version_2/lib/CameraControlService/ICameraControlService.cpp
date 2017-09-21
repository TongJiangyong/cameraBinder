/**
���ʹ��˵����
�������˾���ʵ�ֵĴ���ӿڣ�����Ϊ�ͻ�������ʵ��������ȡ����
**/
#ifndef I_CAMERA_CONTROL_SERVICE_CPP
#define I_CAMERA_CONTROL_SERVICE_CPP

#define LOG_NDEBUG 0
#define LOG_TAG "I_CAMERA_CONTROL_SERVICE_CPP"

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


#include <binder/IMemory.h>

//��debug���
#include <media/stagefright/foundation/AString.h>
#include <media/stagefright/foundation/ADebug.h>
#include <media/stagefright/foundation/ABuffer.h>
#include <media/stagefright/foundation/AHandler.h>
#include <media/stagefright/foundation/AMessage.h>
#include "CameraControlService/ICameraControlService.h"
namespace android{	

// function call index
enum {
    CREATE = IBinder::FIRST_CALL_TRANSACTION,
    HELLOWORLD,
};

class BpCameraControlService: public BpInterface<ICameraControlService>
{
public:
    BpCameraControlService(const sp<IBinder>& impl):BpInterface<ICameraControlService>(impl)
    {
    }

    virtual sp<ICameraControlServiceClient> create()
    {
        Parcel data, reply;
        data.writeInterfaceToken(ICameraControlService::getInterfaceDescriptor());
        remote()->transact(CREATE, data, &reply);
        return interface_cast<ICameraControlServiceClient>(reply.readStrongBinder());
    }

    virtual status_t helloWorld(const char *str)
    {
		Parcel data,reply;
		//������ӿ����Ʊ��浽����������
		data.writeInterfaceToken(
				ICameraControlService::getInterfaceDescriptor());
		//��������ַ������浽����������
		data.writeCString(str);
		//����BpBinder���transact��������
		status_t status =remote()->transact(HELLOWORLD,data,&reply);
		if(status!= NO_ERROR){
			printf("Print helloworld error");
		}else{
			//��ȡhelloWorld(0�����ĵ��ý��
			status = reply.readInt32();
		}
		return status;
    }
};

IMPLEMENT_META_INTERFACE(CameraControlService, "realtek.cameracontrolservice");

status_t BnCameraControlService::onTransact(
    uint32_t code,
    const Parcel& data,
    Parcel* reply,
    uint32_t flags)
{
    switch(code){
		
        case CREATE:{
            CHECK_INTERFACE(ICameraControlService, data, reply);
            sp<ICameraControlServiceClient> st = create();
            sp<IBinder> b = st->asBinder(st);
            reply->writeStrongBinder(b);
            return NO_ERROR;
        }break;
		
        case HELLOWORLD:{
			CHECK_INTERFACE(ICameraControlService,data,reply);
			const char *str;
			str = data.readCString();
			reply->writeInt32(helloWorld(str));
			return NO_ERROR;
        }break;
        default:
            return BBinder::onTransact(code, data, reply, flags);
    }
};
} //end of namespace
#endif