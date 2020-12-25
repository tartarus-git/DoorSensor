#include "Camera.h"

#include <opencv2/opencv.hpp>
#include <thread>
#include <sys/stat.h>
#include <chrono>

#define PLAYBACK_FPS 10
#define CAPTURE_INTERVAL 100000

#define OUTPUT_PATH "../output.avi"
#define OUTPUT_MAX_BLOCKS 10485760 // output.avi has exactly 5 GB of space for itself.

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
			auto start = std::chrono::high_resolution_clock::now();

			// Capture and write 1 frame.
                	cap >> frame;
                	output.write(frame);

			// Make sure the file size doesn't get out of hand.
			// TODO: If file size is proportional to number of frames and bytes and such, because of possible lack of compression, then just count frames 
			// instead of checking file size.
			stat(OUTPUT_PATH, &statBuf);
			if (statBuf.st_blocks > OUTPUT_MAX_BLOCKS) {
				overflowed = true;
				failure = true;
				output.release();
				cap.release();
				break;
			}

			long long duration = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - start).count();
			if (duration < CAPTURE_INTERVAL) { std::this_thread::sleep_for(std::chrono::microseconds(CAPTURE_INTERVAL - duration)); }
			//else { printf("This sux."); } TODO: Make some sort of log entry that alerts when this happens. Obviously not simple because seperate thread.
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
	output.release();
	cap.release();
}
