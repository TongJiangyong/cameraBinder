// Callback implementation
#ifndef CAMERA_CALLBACK_CPP
#define CAMERA_CALLBACK_CPP
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
#define SETUP_TIMEOUT 2000000000 // ns
#define IDLE_TIMEOUT 2000000000 // ns
using namespace android;
class TestCameraDeviceCallbacks : public hardware::camera2::BnCameraDeviceCallbacks {
public:
    enum Status {
        IDLE,
        ERROR,
        PREPARED,
        RUNNING,
        SENT_RESULT,
        UNINITIALIZED,
        REPEATING_REQUEST_ERROR,
    };

protected:
    bool mError;
    int32_t mLastStatus;
    mutable std::vector<int32_t> mStatusesHit;
    mutable Mutex mLock;
    mutable Condition mStatusCondition;
public:
    TestCameraDeviceCallbacks() : mError(false), mLastStatus(UNINITIALIZED) {}

    virtual ~TestCameraDeviceCallbacks() {}

    virtual binder::Status onDeviceError(int errorCode,
            const CaptureResultExtras& resultExtras) {
        (void) resultExtras;
        ALOGE("%s: onDeviceError occurred with: %d", __FUNCTION__, static_cast<int>(errorCode));
        Mutex::Autolock l(mLock);
        mError = true;
        mLastStatus = ERROR;
        mStatusesHit.push_back(mLastStatus);
        mStatusCondition.broadcast();
        return binder::Status::ok();
    }

    virtual binder::Status onDeviceIdle() {
        Mutex::Autolock l(mLock);
        mLastStatus = IDLE;
        mStatusesHit.push_back(mLastStatus);
        mStatusCondition.broadcast();
        return binder::Status::ok();
    }

    virtual binder::Status onCaptureStarted(const CaptureResultExtras& resultExtras,
            int64_t timestamp) {
        (void) resultExtras;
        (void) timestamp;
        Mutex::Autolock l(mLock);
        mLastStatus = RUNNING;
        mStatusesHit.push_back(mLastStatus);
        mStatusCondition.broadcast();
        return binder::Status::ok();
    }


    virtual binder::Status onResultReceived(const CameraMetadata& metadata,
            const CaptureResultExtras& resultExtras) {
        (void) metadata;
        (void) resultExtras;
        Mutex::Autolock l(mLock);
        mLastStatus = SENT_RESULT;
        mStatusesHit.push_back(mLastStatus);
        mStatusCondition.broadcast();
        return binder::Status::ok();
    }

    virtual binder::Status onPrepared(int streamId) {
        (void) streamId;
        Mutex::Autolock l(mLock);
        mLastStatus = PREPARED;
        mStatusesHit.push_back(mLastStatus);
        mStatusCondition.broadcast();
        return binder::Status::ok();
    }

    virtual binder::Status onRepeatingRequestError(int64_t lastFrameNumber) {
        (void) lastFrameNumber;
        Mutex::Autolock l(mLock);
        mLastStatus = REPEATING_REQUEST_ERROR;
        mStatusesHit.push_back(mLastStatus);
        mStatusCondition.broadcast();
        return binder::Status::ok();
    }

    // Test helper functions:

    bool hadError() const {
        Mutex::Autolock l(mLock);
        return mError;
    }

    bool waitForStatus(Status status) const {
        Mutex::Autolock l(mLock);
        if (mLastStatus == status) {
            return true;
        }

        while (std::find(mStatusesHit.begin(), mStatusesHit.end(), status)
                == mStatusesHit.end()) {

            if (mStatusCondition.waitRelative(mLock, IDLE_TIMEOUT) != OK) {
                mStatusesHit.clear();
                return false;
            }
        }
        mStatusesHit.clear();

        return true;

    }

    void clearStatus() const {
        Mutex::Autolock l(mLock);
        mStatusesHit.clear();
    }

    bool waitForIdle() const {
        return waitForStatus(IDLE);
    }

};
#endif