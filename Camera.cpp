#include "Camera.h"

#include <opencv2/opencv.hpp>
#include <thread>
#include <sys/stat.h>
#include <chrono>

#define PLAYBACK_FPS 25
#define CAPTURE_INTERVAL 40

#define OUTPUT_PATH "../output.avi"

bool Camera::failure;

bool Camera::init() {
	// Create VideoCapture object and set the source to the camera.
	cap = cv::VideoCapture(0);
	if (cap.isOpened()) {
        	// Create VideoWriter object and set codec, fps, etc...
        	output = cv::VideoWriter(OUTPUT_PATH, cv::VideoWriter::fourcc('X', '2', '6', '4'), PLAYBACK_FPS, 
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
		struct stat statBuf;
		while (isAlive) {
                	cap >> frame;
                	output.write(frame);

			// Make sure the file size doesn't get out of hand.
			stat(OUTPUT_PATH, &statBuf);
			printf("%d", (int)statBuf.st_blocks);

                	std::this_thread::sleep_for(std::chrono::milliseconds(10));
        	}
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
