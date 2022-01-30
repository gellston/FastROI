#include <iostream>
#include <stack>
#include <chrono>
#include <opencv2/opencv.hpp>
#include <fastBlob.h>

int main()
{
    cv::namedWindow("binary", cv::WINDOW_NORMAL);
    cv::namedWindow("result_image", cv::WINDOW_NORMAL);
    cv::resizeWindow("binary", cv::Size(600, 1200));
    cv::resizeWindow("result_image", cv::Size(600, 1200));
    cv::VideoCapture cap("d://test_video.mp4");
    system("pause");
    while (cap.isOpened()) {
        cv::Mat current_frame;
        if (cap.read(current_frame) == false)
            return 0;
        cv::Mat gray_image;
        cv::cvtColor(current_frame, gray_image, cv::COLOR_BGR2GRAY);
        cv::Mat binary;
        cv::threshold(gray_image, binary, 250, 255, cv::THRESH_BINARY + cv::THRESH_OTSU);
        std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
        //Blob Detection 
        //Blob Detection 
        fast::fastBlob blobDetector;
        blobDetector.compute(binary.data, binary.cols, binary.rows);
        auto blobs = blobDetector.blobInfo();
        //Blob Detection 
        //Blob Detection 

        for (auto& info : blobs) {
            cv::rectangle(current_frame, cv::Rect(info.rectX(), info.rectY(), info.rectWidth(), info.rectHeight()), cv::Scalar(0, 255, 0), 5);
        }
        std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
        std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << "[ms]" << std::endl;

        cv::imshow("binary", binary);
        cv::imshow("result_image", current_frame);
        cv::waitKey(11);
    }
}