LOCAL_PATH:=$(call my-dir)
include $(CLEAR_VARS)
LOCAL_SRC_FILES:=\
	main_cameraservice.cpp
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
	libbinder \
	libcameralearn
	
LOCAL_C_INCLUDES += \
	system/media/private/camera/include \
	system/media/camera/tests \
	frameworks/av/services/camera/libcameraservice \
	frameworks/av/media/libstagefright \
	
LOCAL_CFLAGS += -Wall -Wextra -Werror
base:=$(LOCAL_PATH)/../..
LOCAL_MODULE:=libcameraservice
include $(BUILD_EXECUTABLE)
#********************************************************************
include $(CLEAR_VARS)
LOCAL_SRC_FILES:=\
	main_cameraclient.cpp
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
	libbinder \
	libcameralearn
	
LOCAL_C_INCLUDES += \
	system/media/private/camera/include \
	system/media/camera/tests \
	frameworks/av/services/camera/libcameraservice \
	frameworks/av/media/libstagefright \
	
LOCAL_CFLAGS += -Wall -Wextra -Werror
base:=$(LOCAL_PATH)/../..
LOCAL_MODULE:=libcameraclient
include $(BUILD_EXECUTABLE)








