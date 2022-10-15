#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include <opencv2/opencv.hpp>
#include "mediapipe.hpp"

class Logger : public MediapipeLogger {
public:
    virtual void Log(const std::string& content) const override {
        std::cout << content << std::endl;
    }
};

int main() {
    // init mediapipe and set logger
    auto logger = std::make_shared<Logger>();
    auto interface = CreateMediapipeInterface();
    interface->SetLogger(logger);

    cv::namedWindow("MediaPipe");
    cv::VideoCapture capture;
    capture.open(0);
    bool isCamera = true;

    cv::Mat outputBGRFrame;
    cv::Mat cameraBGRFrame;
    bool grabFrames = true;
    if (!capture.isOpened()) {
        logger->Log("VideoCapture is not open");
        return -1;
    }
    interface->SetResourceDir("");
    interface->SetGraph("hand_tracking_desktop_live.pbtxt");

    auto matCallback = [&](const cv::Mat& frame) {
        cv::cvtColor(frame, outputBGRFrame, cv::COLOR_RGB2BGR);
    };

    auto landmarkCallback = [&](const std::map<std::string, std::vector<std::vector<double>>>& data) {
        auto left = data.find("Left");
        auto right = data.find("Right");
        if (left != data.end()) {
            for (const auto& landmark : left->second) {
                std::cout << "Left: " << landmark[0] << " " << landmark[1] << " " << landmark[2] << " | ";
            }
            std::cout << std::endl;
        }
        if (right != data.end()) {
            for (const auto& landmark : right->second) {
                std::cout << "Right: " << landmark[0] << " " << landmark[1] << " " << landmark[2] << " | ";
            }
            std::cout << std::endl;
        }
    };

    interface->OpenPreview(matCallback);
    interface->CreateObserver("handedness", nullptr);
    interface->CreateObserver("landmarks", landmarkCallback);
    interface->Start();

    while (grabFrames) {
        capture >> cameraBGRFrame;
        if (isCamera) {
            cv::flip(cameraBGRFrame, cameraBGRFrame, 1);
        }
        if (cameraBGRFrame.empty()) {
            logger->Log("Empty frame.");
            break;
        }
        cv::Mat cameraRGBFrame;
        cv::cvtColor(cameraBGRFrame, cameraRGBFrame, cv::COLOR_BGR2RGB);
        interface->Detect(cameraRGBFrame);
        if (outputBGRFrame.cols > 0) {
            cv::imshow("MediaPipe", outputBGRFrame);
        }
        int pressed_key = cv::waitKey(30);
        if (pressed_key >= 0 && pressed_key != 255) grabFrames = false;
    }
    interface->Stop();
    delete interface;
    return 0;
}
