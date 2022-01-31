#include <iostream>
#include <stack>
#include <chrono>
#include <opencv2/opencv.hpp>
#include <fastBlob.h>

int main()
{
    cv::namedWindow("binary", cv::WINDOW_NORMAL);
    cv::namedWindow("result_image", cv::WINDOW_NORMAL);
    cv::namedWindow("filter", cv::WINDOW_NORMAL);


    cv::resizeWindow("binary", cv::Size(800, 600));
    cv::resizeWindow("result_image", cv::Size(800, 600));
    cv::resizeWindow("filter", cv::Size(800, 600));


    cv::VideoCapture cap("d://test_video.mp4");
    system("pause");
    while (true) {
        cv::Mat current_frame;
        if (cap.read(current_frame) == false)
            return 0;

        cv::Mat gray;

        cv::cvtColor(current_frame, gray, cv::COLOR_BGR2GRAY);


        cv::Mat binary;
        cv::threshold(gray, binary, 250, 255, cv::THRESH_BINARY + cv::THRESH_OTSU);
        std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();


        cv::Mat filter(cv::Size(binary.cols, binary.rows), CV_8UC1);
        memset(filter.data, 0, filter.cols * filter.rows);

        //Blob Detection 
        //Blob Detection 
        fast::fastBlob blobDetector;
        blobDetector.compute(binary.data, binary.cols, binary.rows);
        auto blobs = blobDetector.blobInfo();
        //blobs = fast::fastBlob::rectFilter(blobs, 100, 200, 100, 200);


        //Blob Detection 
        for (auto& blob : blobs) {
            fast::fastBlob::blobFill(filter.data, filter.cols, filter.rows, 255, blob);
            cv::rectangle(current_frame, cv::Rect(blob.rectX(), blob.rectY(), blob.rectWidth(), blob.rectHeight()), cv::Scalar(0, 255, 0), 5);
        }
        //Blob Detection 



        std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
        std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << "[ms]" << std::endl;

        cv::imshow("binary", binary);
        cv::imshow("filter", filter);
        cv::imshow("result_image", current_frame);
        cv::waitKey(11);
    }
}