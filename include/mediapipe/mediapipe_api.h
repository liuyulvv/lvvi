#ifndef MEDIAPIPE_API_H_
#define MEDIAPIPE_API_H_

#include "mediapipe_interface.h"

#ifdef __WIN32__

#ifndef DllExport
#define DllExport __declspec(dllexport)
#endif

DllExport MediapipeInterface* CreateMediapipeInterface();

#else

MediapipeInterface* CreateMediapipeInterface();

#endif

#endif