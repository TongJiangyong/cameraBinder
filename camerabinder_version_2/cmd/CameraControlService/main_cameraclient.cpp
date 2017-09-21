#define LOG_TAG "main_helloworldclient"
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
#include <signal.h>  
#include <stdio.h>  
#include <stdlib.h>  
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
using namespace android;
//如果开启，则，只进行和preview相关的操作.......
#define PREVIEW_TEST
#define USB_CAMERA 0
#define HDMI 1
#include "CameraControlService/ICameraControlService.h"
//#include "CameraControlService/ICameraControlServiceClient.h"
using namespace android;
#define USE_CLIENT
#define PREVIEW_TEST

#define ASSERT_NOT_NULL(x) \
    ASSERT_TRUE((x) != nullptr)
status_t setupSurface(sp<SurfaceComposerClient> *composerClient, sp<SurfaceControl> *control, sp<Surface> *surface)
{
    ALOGD("TJY：setupSurface");

    // setup surface
    *composerClient = new SurfaceComposerClient;
    //CHECK_EQ((*composerClient)->initCheck(), (status_t)OK);
	status_t result;
	result = (*composerClient)->initCheck();
	CHECK_EQ(result, (status_t)OK);
	
    sp<IBinder> display(SurfaceComposerClient::getBuiltInDisplay(ISurfaceComposer::eDisplayIdMain));
    DisplayInfo info;
    SurfaceComposerClient::getDisplayInfo(display, &info);
    ssize_t displayWidth = info.w;
    ssize_t displayHeight = info.h;
    ALOGD("display is %d x %d\n", displayWidth, displayHeight); //1280x720

    *control = (*composerClient)->createSurface(String8("RtkTranscode"), displayWidth, displayHeight, PIXEL_FORMAT_RGB_565, 0);
    //CHECK(*control != NULL);
	if(*control != NULL){
		ALOGD("TJY:control != NULL");
	}
    //CHECK((*control)->isValid());
	if((*control)->isValid()){
		ALOGD("TJY:control isValid");
	}
    SurfaceComposerClient::openGlobalTransaction();
    //CHECK_EQ((*control)->setLayer(INT_MAX), (status_t)OK);
	result = (*control)->setLayer(INT_MAX);
	CHECK_EQ(result, (status_t)OK);
    //CHECK_EQ((*control)->show(), (status_t)OK);
	result = (*control)->show();
	CHECK_EQ(result, (status_t)OK);
    SurfaceComposerClient::closeGlobalTransaction();

    *surface = (*control)->getSurface();
    //CHECK(*surface != NULL);
	if(*surface != NULL){
		ALOGD("TJY:surface != NULL");
	}
    return OK;
}


//用于捕捉信号量
bool mQuit = false;
void sighandler(int num)
{
    ALOGW("got signal:%d SIGHUP:%d SIGUSR1:%d SIGUSR2:%d SIGINT :%d \n", num, SIGHUP, SIGUSR1, SIGUSR2,SIGINT);
    if(num == SIGINT || num == SIGPIPE){
        mQuit = true;
    }
}
bool checkQuitStatus()
{
	return mQuit;
}


static sp<ICameraControlServiceClient> mCameraControlClient;
int main()
{
    signal(SIGINT, sighandler);
    signal(SIGPIPE, sighandler);
    printf("CameraService client  is starting now ");
    sp<IServiceManager> sm =defaultServiceManager();
    sp<IBinder> b;
    sp<ICameraControlService> mCameraService;
    do{
        b = sm->getService(String16("realtek.cameracontrolservice"));
        if(b!=0)
            break;
        printf("CameraService is not working ,waiting...\n");
        usleep(500000);
    }while(true);
    mCameraService = interface_cast<ICameraControlService>(b);
    mCameraService -> helloWorld("hello, world");
#ifdef USE_CLIENT	
    mCameraService -> helloWorld("hello, world test with USE_CLIENT");
	mCameraControlClient = mCameraService->create();
	mCameraControlClient->initCameraService();
	int numOfCamera;
	numOfCamera = mCameraControlClient->getNumOfCamera();
	if(numOfCamera<=0){
		printf("TJY:no camera connected\n");
		return 0;
	}
	printf("TJY: numOfCamera is :%d \n",numOfCamera);	
	//初始化camera
	if(!mCameraControlClient->initCamera(USB_CAMERA))
	{
		ALOGD("TJY:init camera failed");
		return 0;	
	}
#ifndef PREVIEW_TEST
	/**
		return  suerface相关的处理******
	**/				
	sp<IGraphicBufferProducer> gbProducer;
	sp<IGraphicBufferConsumer> gbConsumer;
	BufferQueue::createBufferQueue(&gbProducer, &gbConsumer);
	sp<BufferItemConsumer> opaqueConsumer = new BufferItemConsumer(gbConsumer,
			GRALLOC_USAGE_SW_READ_NEVER, 2/*maxImages*/, true);//(transcodePlayer值为16)
	opaqueConsumer->setName(String8("bufferConsumer"));		
	// Set to VGA dimens for default, as that is guaranteed to be present		
	gbConsumer->setDefaultBufferSize(640, 480);
	gbConsumer->setDefaultBufferFormat(HAL_PIXEL_FORMAT_IMPLEMENTATION_DEFINED);
	
	//设定consumer的线程：
	sp<BufferQueueListener> mBufferQueueListener;
	mBufferQueueListener = new BufferQueueListener(opaqueConsumer);
	status_t result = mBufferQueueListener->run("TJY-BufferQueueListener");
	CHECK_EQ(result, (status_t)OK);
	
	
	//这里是设定productor？？
	//sp<Surface> surface(new Surface(gbProducer, false)); 			
#else		
	/**
		view suerface相关的处理******
	**/
	sp<Surface> surface = NULL;
	sp<SurfaceControl> control;
	sp<SurfaceComposerClient> composerClient;
	status_t result = setupSurface(&composerClient, &control, &surface);
	CHECK_EQ(result, (status_t)OK);
	ALOGD("TJY:setupSurface successed");
	sp<IGraphicBufferProducer> gbProducer;
	gbProducer = surface->getIGraphicBufferProducer();
#endif		
	//定义request的类型，暂时不做record
	int requestType = hardware::camera2::ICameraDeviceUser::TEMPLATE_PREVIEW;
	status_t res;
	res = mCameraControlClient->initRequest(gbProducer,requestType);
	CHECK_EQ(res, (status_t)OK);
	ALOGD("TJY:initRequest successed");
	res = mCameraControlClient->submitRequest();
	CHECK_EQ(res, (status_t)OK);
	ALOGD("TJY:submitRequest successed");
		//休眠线程
	while(!checkQuitStatus()){
		usleep(/*second*/100000); // allow some time for errors to show up, if any			
	}
	res = mCameraControlClient->stopRequest();
	
	CHECK_EQ(res, (status_t)OK);	
	ALOGD("TJY:stopRequest successed");
	res = mCameraControlClient->disconnectCamera();	
	CHECK_EQ(res, (status_t)OK);
	ALOGD("TJY:disconnectCamera successed");
		//删除surface
#ifndef PREVIEW_TEST
		//删除监听
			if (mBufferQueueListener != nullptr) {
				mBufferQueueListener->requestExit();
				mBufferQueueListener->join();
				mBufferQueueListener.clear();
				mBufferQueueListener = nullptr;
			}
#else
	//删除mBufferQueueListener
			composerClient->dispose();
#endif		
	res = mCameraControlClient->deleteListner();	
	CHECK_EQ(res, (status_t)OK);
	ALOGD("TJY:cameraBinderTests over");

	
#endif
    return 0;
}
