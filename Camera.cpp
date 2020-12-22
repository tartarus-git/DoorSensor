#include "Camera.h"

#include <opencv2/opencv.hpp>
#include <thread>
#include <sys/stat.h>
#include <chrono>

#define PLAYBACK_FPS 25
#define CAPTURE_INTERVAL 40

#define OUTPUT_PATH "../output.avi"
#define OUTPUT_MAX_BLOCKS 50000

cv::VideoCapture Camera::cap;

cv::VideoWriter Camera::output;

bool Camera::failure;

bool Camera::init() {
	// Create VideoCapture object and set the source to the camera.
	cap = cv::VideoCapture(0);
	if (cap.isOpened()) {
        	// Create VideoWriter object and set codec, fps, etc...
        	output = cv::VideoWriter(OUTPUT_PATH, cv::VideoWriter::fourcc('X', '2', '6', '4'), PLAYBACK_FPS, 
			cv::Size(cap.get(cv::CAP_PROP_FRAME_WIDTH), cap.get(cv::CAP_PROP_FRAME_HEIGHT)));
		if (output.isOpened()) {
			failure = false;
			return true;
		}
		cap.release();
	}
	failure = true;
	return false;
}

bool Camera::isAlive;

bool Camera::overflowed = false;

std::thread Camera::recordThread;

void Camera::record() {
	if (failure) { return; }

	isAlive = true;
	recordThread = std::thread([]() {
		// Stream video into file.
		cv::Mat frame;
		struct stat statBuf;
		while (isAlive) {
			// Capture and write 1 frame.
                	cap >> frame;
                	output.write(frame);

			// Make sure the file size doesn't get out of hand.
			// TODO: If file size is proportional to number of frames and bytes and such, because of possible lack of compression, then just count frames 
			// instead of checking file size.
			stat(OUTPUT_PATH, &statBuf);
			printf("%d\n", (int)statBuf.st_blocks);
			if (statBuf.st_blocks > OUTPUT_MAX_BLOCKS) {
				overflowed = true;
				failure = true;
				output.release();
				cap.release();
				break;
			}

                	std::this_thread::sleep_for(std::chrono::milliseconds(CAPTURE_INTERVAL));
        	}
	});
}

void Camera::stop() {
	if (isAlive) {
		isAlive = false;
		recordThread.join();
	}
}

void Camera::dispose() {
	if (failure) { return; }

	output.release();
	cap.release();
}
