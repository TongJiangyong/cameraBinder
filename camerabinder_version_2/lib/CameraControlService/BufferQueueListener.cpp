//BufferQueueListener的线程使用方法
#ifndef BUFFER_QUEUE_LISTENER_CPP
#define BUFFER_QUEUE_LISTENER_CPP
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
#include <gui/BufferItemConsumer.h>
#include <CameraControlService/BufferQueueListener.h>
using namespace android;
BufferQueueListener::BufferQueueListener(const sp<BufferItemConsumer>& consumer) {
    mConsumer = consumer;
    mConsumer->setFrameAvailableListener(this);
}

void BufferQueueListener::onFrameAvailable(const BufferItem& /*item*/) {
    ALOGV("%s: onFrameAvailable", __FUNCTION__);

    Mutex::Autolock l(mLock);

    if (!mFrameAvailable) {
        mFrameAvailable = true;
        mFrameAvailableSignal.signal();
    }
}

void BufferQueueListener::initFrameCounter()
{
    mFrameReceived = 0;
    if(0 == mDivide) {
        mDivide = 1;
    }

}

bool BufferQueueListener::threadLoop() {
    if (mConsumer == nullptr) {
        return false;
    }

    {
        Mutex::Autolock l(mLock);
        while (!mFrameAvailable) {
            if (mFrameAvailableSignal.waitRelative(mLock, kFrameAvailableTimeout) == TIMED_OUT) {
                return true;
            }
        }
        mFrameAvailable = false;
    }

    BufferItem buffer;
	// loop to acquire buffer from consumer
	while (mConsumer->acquireBuffer(&buffer, 0) == OK) {
		ALOGV("gotFrame -> processBufferQueueFrame ");
		mFrameReceived++;
		//ALOGD("mod %d mDivide %d", (mFrameReceived%mDivide),mDivide);
		if(0 != (mFrameReceived%mDivide)){
			ALOGD("TJY:return buffer %lld",mFrameReceived);
			mConsumer->releaseBuffer(buffer);
		}else{
			ALOGD("*********TJY: mConsumerBuffer pts %.4f ************",((float)buffer.mTimestamp)/(float)1E9);
			//buffer如何给入surface中？
			//在这里模拟nativeWindow queue/deaueue buffer试试
			mConsumer->releaseBuffer(buffer);
		}
	}

    return true;
}
#endif