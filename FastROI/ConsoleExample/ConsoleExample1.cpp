// ConsoleExample.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
//

#include <iostream>
#include <opencv2/opencv.hpp>

#include "fastRect.h"

int main()
{

	cv::namedWindow("result", cv::WINDOW_NORMAL);

	double angle = 0;
	cv::Mat result(cv::Size(4000, 4000), CV_8UC3);
	while (true) {
		
		
		result = cv::Scalar(0, 0, 0);

		std::vector<std::vector<fast::calPoint>> vertical_lines = fast::rectROI(2000, 2000, angle, 1000, 1000, false, 40);
		for (auto vertical_line : vertical_lines) {
			for (auto calPoint : vertical_line) {
				if (calPoint.x < 0 || calPoint.x >= 4000 || calPoint.y < 0 || calPoint.y >= 4000)
					continue;

				cv::circle(result, cv::Point((int)calPoint.x, (int)calPoint.y), 3, cv::Scalar(0, 255, 0), -1);
			}
		}

		cv::imshow("result", result);
		if (cv::waitKey(1) == 27)break;
		if (angle++ > 360) angle = 0;
	}
}

