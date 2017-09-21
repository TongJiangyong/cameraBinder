#include <binder/IPCThreadState.h>
#include <binder/ProcessState.h>
#include <binder/IServiceManager.h>
#include "CameraControlService/CameraControlService.h"
using namespace android;
int main()
{
    CameraControlService::instantiate();
    ProcessState::self()->startThreadPool();
    printf("CameraService is starting now ");
    IPCThreadState::self()->joinThreadPool();
    return 0;
}
