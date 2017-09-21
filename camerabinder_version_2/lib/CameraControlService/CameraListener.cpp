// Stub listener implementation
#ifndef CAMERA_LISTENER_CPP
#define CAMERA_LISTENER_CPP
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

#include <vector>
#include <map>
#define SETUP_TIMEOUT 2000000000 // ns
#define IDLE_TIMEOUT 2000000000 // ns
using namespace android;
class TestCameraServiceListener : public hardware::BnCameraServiceListener {
    std::map<String16, int32_t> mCameraTorchStatuses;
    std::map<int32_t, int32_t> mCameraStatuses;
    mutable Mutex mLock;
    mutable Condition mCondition;
    mutable Condition mTorchCondition;
public:
    virtual ~TestCameraServiceListener() {};

    virtual binder::Status onStatusChanged(int32_t status, int32_t cameraId) {
        Mutex::Autolock l(mLock);
        mCameraStatuses[cameraId] = status;
        mCondition.broadcast();
        return binder::Status::ok();
    };

    virtual binder::Status onTorchStatusChanged(int32_t status, const String16& cameraId) {
        Mutex::Autolock l(mLock);
        mCameraTorchStatuses[cameraId] = status;
        mTorchCondition.broadcast();
        return binder::Status::ok();
    };

    bool waitForNumCameras(size_t num) const {
        Mutex::Autolock l(mLock);

        if (mCameraStatuses.size() == num) {
            return true;
        }

        while (mCameraStatuses.size() < num) {
            if (mCondition.waitRelative(mLock, SETUP_TIMEOUT) != OK) {
                return false;
            }
        }
        return true;
    };

    bool waitForTorchState(int32_t status, int32_t cameraId) const {
        Mutex::Autolock l(mLock);

        const auto& iter = mCameraTorchStatuses.find(String16(String8::format("%d", cameraId)));
        if (iter != mCameraTorchStatuses.end() && iter->second == status) {
            return true;
        }

        bool foundStatus = false;
        while (!foundStatus) {
            if (mTorchCondition.waitRelative(mLock, SETUP_TIMEOUT) != OK) {
                return false;
            }
            const auto& iter =
                    mCameraTorchStatuses.find(String16(String8::format("%d", cameraId)));
            foundStatus = (iter != mCameraTorchStatuses.end() && iter->second == status);
        }
        return true;
    };

    int32_t getTorchStatus(int32_t cameraId) const {
        Mutex::Autolock l(mLock);
        const auto& iter = mCameraTorchStatuses.find(String16(String8::format("%d", cameraId)));
        if (iter == mCameraTorchStatuses.end()) {
            return hardware::ICameraServiceListener::TORCH_STATUS_UNKNOWN;
        }
        return iter->second;
    };

    int32_t getStatus(int32_t cameraId) const {
        Mutex::Autolock l(mLock);
        const auto& iter = mCameraStatuses.find(cameraId);
        if (iter == mCameraStatuses.end()) {
            return hardware::ICameraServiceListener::STATUS_UNKNOWN;
        }
        return iter->second;
    };
};

#endif