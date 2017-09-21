LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)
LOCAL_SRC_FILES:=\
	CameraControlService.cpp\
	ICameraControlService.cpp\
	CameraControl.cpp\
	BufferQueueListener.cpp\
	CameraCallback.cpp\
	CameraListener.cpp\
	CameraControlServiceClient.cpp\
	ICameraControlServiceClient.cpp	
	
LOCAL_SHARED_LIBRARIES :=\
	libcutils\
	libutils\
	libstagefright \
	libcamera_metadata \
	libcamera_client \
	libstagefright_foundation\
	libgui \
	libsync \
	libui \
	libdl \
	libbinder
	
LOCAL_C_INCLUDES += \
	system/media/private/camera/include \
	system/media/camera/tests \
	frameworks/av/services/camera/libcameraservice \
	frameworks/av/media/libstagefright \
#LOCAL_PRELINK_MODULE :=false
LOCAL_MODULE:=libcameralearn
LOCAL_MODULE_TAGS := optional
include $(BUILD_SHARED_LIBRARY)
