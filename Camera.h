#pragma once

#include <opencv2/opencv.hpp>
#include <thread>

class Camera {
	static cv::VideoCapture cap;
	static cv::VideoWriter output;
	static bool failure;

	static bool isAlive;
	static std::thread recordThread;
public:
	Camera() = delete;
	static bool init();

	static bool overflowed;
	static void record();
	static void stop();

	static void dispose();
};
