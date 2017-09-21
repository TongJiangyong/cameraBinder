    /**
     * The class for listening to BufferQueue's onFrameAvailable. This is used to receive video
     * buffers in VIDEO_BUFFER_MODE_BUFFER_QUEUE mode. When a frame is available,
     * CameraSource::processBufferQueueFrame() will be called.
     */
#ifndef BUFFER_QUEUE_LISTENER_H
#define BUFFER_QUEUE_LISTENER_H
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
#include <gui/BufferItemConsumer.h>
#include <hardware/gralloc.h>
using namespace android;
class BufferQueueListener : public Thread,  public BufferItemConsumer::FrameAvailableListener {
    public:
        BufferQueueListener(const sp<BufferItemConsumer> &consumer);
        virtual void onFrameAvailable(const BufferItem& item);
        virtual bool threadLoop();
        virtual void onFirstRef() {
            initFrameCounter();
        }
    private:
        static const nsecs_t kFrameAvailableTimeout = 50000000; // 50ms

        sp<BufferItemConsumer> mConsumer;

        Mutex mLock;
        Condition mFrameAvailableSignal;
        bool mFrameAvailable;
        void initFrameCounter();
        int64_t mFrameReceived;
        int32_t mDivide;
};
#endif