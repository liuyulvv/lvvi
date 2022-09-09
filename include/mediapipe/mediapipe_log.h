#ifndef LLVI_MEDIAPIPE_LOG_H_
#define LLVI_MEDIAPIPE_LOG_H_

#include <string>

class MediapipeLogger {
public:
    MediapipeLogger() = default;
    virtual ~MediapipeLogger() = default;

    virtual void Log(const std::string& content) const = 0;
};

#endif