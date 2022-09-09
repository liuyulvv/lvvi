#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "spdlog/spdlog.h"
#include "opencv.hpp"
#include "mediapipe.hpp"
#include "GL/glew.h"
#include "GLFW/glfw3.h"

class Logger : public MediapipeLogger {
public:
    virtual void Log(const std::string& content) const override {
        spdlog::info(content);
    }
};

void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
}

void FrameBufferResizeCallback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

const unsigned WINDOW_WIDTH = 640;
const unsigned WINDOW_HEIGHT = 480;
const std::string& WINDOW_NAME = "lvvi";

const char* vertexShaderSource =
    "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
    "}\0";

const char* fragmentShaderSource =
    "#version 330 core\n"
    "out vec4 FragColor;\n"
    "void main()\n"
    "{\n"
    "   FragColor = vec4(1.0f, 0.46f, 0.51f, 1.0f);\n"
    "}\n\0";

int main() {
    // GLFW GLEW init
    if (!glfwInit()) {
        spdlog::error("GLFW init failed");
        return -1;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    auto window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_NAME.c_str(), nullptr, nullptr);
    if (window == nullptr) {
        spdlog::error("GLFW create window failed");
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    if (glewInit() != GLEW_OK) {
        spdlog::error("GLEW init failed");
        glfwTerminate();
        return -1;
    }
    glfwSetKeyCallback(window, KeyCallback);
    glfwSetFramebufferSizeCallback(window, FrameBufferResizeCallback);
    auto vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    glCompileShader(vertexShader);

    int success;
    char infoLog[512];

    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog);
        spdlog::error("Shader vertex compilation failed: {}", infoLog);
    }

    auto fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShader, 512, nullptr, infoLog);
        spdlog::error("Shader fragment compilation failed: {}", infoLog);
    }

    auto shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
        spdlog::error("Shader program link failed: {}", infoLog);
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    glPointSize(2);

    // init mediapipe and set logger
    auto logger = std::make_shared<Logger>();
    auto interface = CreateMediapipeInterface();
    interface->SetLogger(logger);

    cv::VideoCapture capture;
    // capture.open("bjs.mp4");
    // bool isCamera = false;
    capture.open(0);
    bool isCamera = true;

    cv::Mat outputBGRFrame;
    cv::Mat cameraBGRFrame;
    bool grabFrames = true;
    if (!capture.isOpened()) {
        logger->Log("VideoCapture is not open");
    }
    interface->SetResourceDir("");
    interface->SetGraph("face_mesh_desktop_live.pbtxt");

    std::vector<float> vertex(478 * 3, 0.0);

    unsigned int VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    unsigned int VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glBufferData(GL_ARRAY_BUFFER, vertex.size() * sizeof(float), vertex.data(), GL_STREAM_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    auto landmarkCallback = [&](const std::vector<std::vector<double>>& data) {
        vertex.clear();
        for (const auto& d : data) {
            vertex.push_back(d[0] - 0.5);
            vertex.push_back(-d[1] + 0.5);
            vertex.push_back(d[2]);
        }
    };

    interface->CreateObserver("face_landmarks", landmarkCallback);
    interface->Start();

    while (grabFrames && !glfwWindowShouldClose(window)) {
        glfwPollEvents();
        glClearColor(0.14f, 0.16f, 0.18f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glUseProgram(shaderProgram);

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

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertex.size() * sizeof(float), vertex.data(), GL_STREAM_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glDrawArrays(GL_POINTS, 0, 478);
        glfwSwapBuffers(window);
    }
    interface->Stop();
    delete interface;
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);
    glfwTerminate();
    return 0;
}
