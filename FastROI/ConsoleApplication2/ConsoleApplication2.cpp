// ConsoleExample.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
//

#include <iostream>
#include <opencv2/opencv.hpp>
#include <ctime>
#include <chrono>

#include "fastDonut.h"



int main()
{

	cv::namedWindow("result", cv::WINDOW_NORMAL);

	cv::Mat result(cv::Size(4000, 4000), CV_8UC3);

	while (true) {

		auto vertical_lines = fast::fastDonut(2000, 2000, 1000, 1, 0.5, 1);

		result = cv::Scalar(0, 0, 0);
		for (auto& vertical_line : vertical_lines) {
			for (auto& calPoint : vertical_line) {
				if (calPoint.x < 0 || calPoint.x >= 4000 || calPoint.y < 0 || calPoint.y >= 4000)
					continue;

				cv::circle(result, cv::Point((int)calPoint.x, (int)calPoint.y), 2, cv::Scalar(0, 255, 0), -1);
			}
		}
		cv::imshow("result", result);
		if (cv::waitKey(1) == 27)break;
	}
}

