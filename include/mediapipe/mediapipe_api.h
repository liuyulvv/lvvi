#ifndef LLVI_MEDIAPIPE_API_H_
#define LLVI_MEDIAPIPE_API_H_

#include "mediapipe_interface.h"

#ifndef DllExport
#define DllExport __declspec(dllexport)
#endif

DllExport MediapipeInterface* CreateMediapipeInterface();

#endif