#include "Camera.h"

#include <opencv2/opencv.h>
#include <thread>
#include <chrono>

#define PLAYBACK_FPS 25
#define CAPTURE_INTERVAL 40

bool Camera::isOpen;

bool Camera::init() {
	// Create VideoCapture object and set the source to the camera.
	cap = cv::VideoCapture(0);
	if (cap.isOpened()) {
        	int frameWidth = cap.get(cv::CAP_PROP_FRAME_WIDTH);
        	int frameHeight = cap.get(cv::CAP_PROP_FRAME_HEIGHT);

        	// Create VideoWriter object and set codec, fps, etc...
        	output = cv::VideoWriter("output.avi", cv::VideoWriter::fourcc('X', '2', '6', '4'), PLAYBACK_FPS, 
			cv::Size(cap.get(cv::CAP_PROP_FRAME_WIDTH), cap.get(cv::CAP_PROP_FRAME_HEIGHT));
		// Find a way to validate the VideoWriter.
		failure = false;
		return true;
	}
	failure = true;
	return false;
}

bool Camera::isAlive;

void Camera::record() {
	if (failure) { return; }

	isAlive = true;
	thread = std::thread([]() {
		// Stream video into file.
		cv::Mat frame;
		while (isAlive) {
                	cap >> frame;
                	output.write(frame);
                	std::this_thread::sleep_for(std::chrono::milliseconds(10));
        	}
		std::this_thread::sleep_for(std::chrono::milliseconds(CAPTURE_INTERVAL));
	});
}

void Camera::stop() {
	if (failure) { return; }

	isAlive = false;
	recordThread.join();
}

void Camera::dispose() {
	if (failure) { return; }

	output.release();
	cap.release();
}
