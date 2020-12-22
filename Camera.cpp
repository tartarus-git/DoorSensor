#include "Camera.h"

#include <opencv2/opencv.hpp>
#include <thread>
#include <sys/stat.h> // Use this for the file size measurements.
#include <chrono>

#define PLAYBACK_FPS 25
#define CAPTURE_INTERVAL 40

bool Camera::failure;

bool Camera::init() {
	// Create VideoCapture object and set the source to the camera.
	cap = cv::VideoCapture(0);
	if (cap.isOpened()) {
        	// Create VideoWriter object and set codec, fps, etc...
        	output = cv::VideoWriter("output.avi", cv::VideoWriter::fourcc('X', '2', '6', '4'), PLAYBACK_FPS, 
			cv::Size(cap.get(cv::CAP_PROP_FRAME_WIDTH), cap.get(cv::CAP_PROP_FRAME_HEIGHT)));
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
	recordThread = std::thread([]() {
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
