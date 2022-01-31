

#include "fastBlob.h"



#include <string>
#include <algorithm>
#include <iterator>
#include <deque>
#include <iostream>


namespace fast {
	enum border_type {
		HOLE_BORDER = 1,
		OUTER_BORDER = 2
	};

	struct border {
		int seq_num;
		border_type border_type;
	};

	struct node {
		int parent;
		int first_child;
		int next_sibling;
		fast::border border;
		inline node(int p, int fc, int ns) {
			parent = p;
			first_child = fc;
			next_sibling = ns;
		}
		inline void reset() {
			parent = -1;
			first_child = -1;
			next_sibling = -1;
		}
	};

	struct blobPoint {
		int row;
		int col;

		inline blobPoint() {

		}

		inline blobPoint(int r, int c) {
			row = r;
			col = c;
		}

		inline void setPoint(int r, int c) {
			row = r;
			col = c;
		}

		inline bool samePoint(blobPoint p) {
			return row == p.row && col == p.col;
		}
	};


}


namespace fast {
	class impl_fastBlob {
	private:

		std::vector<fast::blobInfo> _blobInfo;
		
	public:
		impl_fastBlob() {

		}

		~impl_fastBlob() {

		}

		std::vector<fast::blobInfo> blobInfo() {
			return _blobInfo;
		}

		void compute(unsigned char * buffer, int width, int height) {

			if (buffer == nullptr) {
				HVERROR(error, "Invalid image buffer");
			}

			if (width <= 0 || height <= 0) {
				HVERROR(error, "Invalid image size");
			}

			try {
				int rows = height;
				int cols = width;

				std::vector<std::vector<int>> labelTable;
				labelTable.resize(rows);
				for (int row = 0; row < rows; row++) {
					labelTable[row].resize(cols);
				}
				for (int row = 0; row < rows; row++) {
					for (int col = 0; col < cols; col++) {
						if (buffer[cols * row + col] > 0)
							labelTable[row][col] = 1;
						else
							labelTable[row][col] = 0;
					}
				}

				std::vector<std::vector<fast::blobPoint>> contours;
				fast::border NBD, LNBD;

				LNBD.border_type = fast::border_type::HOLE_BORDER;
				NBD.border_type = fast::border_type::HOLE_BORDER;
				NBD.seq_num = 1;



				std::vector<fast::node> hierarchy;
				fast::node temp_node(-1, -1, -1);
				temp_node.border = NBD;
				hierarchy.push_back(temp_node);


				fast::blobPoint p2;
				bool border_start_found;
				for (int r = 0; r < rows; r++) {
					LNBD.seq_num = 1;
					LNBD.border_type = fast::border_type::HOLE_BORDER;
					for (int c = 0; c < cols; c++) {
						border_start_found = false;
	
						//Phase1
						if ((labelTable[r][c] == 1 && c - 1 < 0) || (labelTable[r][c] == 1 && labelTable[r][c - 1] == 0)) {
							NBD.border_type = fast::border_type::OUTER_BORDER;
							NBD.seq_num += 1;
							p2.setPoint(r, c - 1);
							border_start_found = true;
						}
						else if (c + 1 < cols && (labelTable[r][c] >= 1 && labelTable[r][c + 1] == 0)) {
							NBD.border_type = fast::border_type::HOLE_BORDER;
							NBD.seq_num += 1;
							if (labelTable[r][c] > 1) {
								LNBD.seq_num = labelTable[r][c];
								LNBD.border_type = hierarchy[LNBD.seq_num - 1].border.border_type;
							}
							p2.setPoint(r, c + 1);
							border_start_found = true;
						}

						if (border_start_found) {
							//Phase2
							temp_node.reset();
							if (NBD.border_type == LNBD.border_type) {
								temp_node.parent = hierarchy[LNBD.seq_num - 1].parent;
								temp_node.next_sibling = hierarchy[temp_node.parent - 1].first_child;
								hierarchy[temp_node.parent - 1].first_child = NBD.seq_num;
								temp_node.border = NBD;
								hierarchy.push_back(temp_node);
							}
							else {
								if (hierarchy[LNBD.seq_num - 1].first_child != -1) {
									temp_node.next_sibling = hierarchy[LNBD.seq_num - 1].first_child;
								}

								temp_node.parent = LNBD.seq_num;
								hierarchy[LNBD.seq_num - 1].first_child = NBD.seq_num;
								temp_node.border = NBD;
								hierarchy.push_back(temp_node);
							}

							//Phase2
							followBorder(labelTable, r, c, p2, NBD, contours);
						}

						//Phase 4
						if (abs(labelTable[r][c]) > 1) {
							LNBD.seq_num = abs(labelTable[r][c]);
							LNBD.border_type = hierarchy[LNBD.seq_num - 1].border.border_type;
						}
					}

				}


				//데이터 추출 및 정리
				int id = 1;
				this->_blobInfo.clear();
				for (auto& contour : contours) {

					int min_x = 99999; 
					int min_y = 99999;
					int max_x = 0;
					int max_y = 0;
					double count = (double)contour.size();
					double centeroid_x = 0;
					double centeroid_y = 0;
					std::vector<fast::calPoint> features;
					for (auto& point : contour) {
						
						if (min_x > point.col)
							min_x = point.col;
						else if (max_x <= point.col)
							max_x = point.col;

						if (min_y > point.row)
							min_y = point.row;
						else if (max_y <= point.row)
							max_y = point.row;

						centeroid_x += ((double)point.col / count);
						centeroid_y += ((double)point.row / count);
						fast::calPoint calPoint;
						calPoint.x = point.col;
						calPoint.y = point.row;
						features.push_back(calPoint);
					}
					int width = max_x - min_x;
					int height = max_y - min_y;

				
					
					fast::blobInfo info(id, min_x, min_y, width, height, centeroid_x, centeroid_y);
					info.setBlobPoints(features);
					_blobInfo.push_back(info);
				}
			}
			catch (std::exception e) {
				HVERROR(error, e.what());
				
			}
		}


		//step around a pixel CCW
		inline void stepCCW(fast::blobPoint& current, fast::blobPoint pivot) {
			if (current.col > pivot.col)
				current.setPoint(pivot.row - 1, pivot.col);
			else if (current.col < pivot.col)
				current.setPoint(pivot.row + 1, pivot.col);
			else if (current.row > pivot.row)
				current.setPoint(pivot.row, pivot.col + 1);
			else if (current.row < pivot.row)
				current.setPoint(pivot.row, pivot.col - 1);
		}

		//step around a pixel CW
		inline void stepCW(fast::blobPoint& current, fast::blobPoint pivot) {
			if (current.col > pivot.col)
				current.setPoint(pivot.row + 1, pivot.col);
			else if (current.col < pivot.col)
				current.setPoint(pivot.row - 1, pivot.col);
			else if (current.row > pivot.row)
				current.setPoint(pivot.row, pivot.col - 1);
			else if (current.row < pivot.row)
				current.setPoint(pivot.row, pivot.col + 1);
		}

		//step around a pixel CCW in the 8-connect neighborhood.
		inline void stepCCW8(fast::blobPoint& current, fast::blobPoint pivot) {
			if (current.row == pivot.row && current.col > pivot.col)
				current.setPoint(pivot.row - 1, pivot.col + 1);
			else if (current.col > pivot.col && current.row < pivot.row)
				current.setPoint(pivot.row - 1, pivot.col);
			else if (current.row < pivot.row && current.col == pivot.col)
				current.setPoint(pivot.row - 1, pivot.col - 1);
			else if (current.row < pivot.row && current.col < pivot.col)
				current.setPoint(pivot.row, pivot.col - 1);
			else if (current.row == pivot.row && current.col < pivot.col)
				current.setPoint(pivot.row + 1, pivot.col - 1);
			else if (current.row > pivot.row && current.col < pivot.col)
				current.setPoint(pivot.row + 1, pivot.col);
			else if (current.row > pivot.row && current.col == pivot.col)
				current.setPoint(pivot.row + 1, pivot.col + 1);
			else if (current.row > pivot.row && current.col > pivot.col)
				current.setPoint(pivot.row, pivot.col + 1);
		}

		//step around a pixel CW in the 8-connect neighborhood.
		inline void stepCW8(fast::blobPoint& current, fast::blobPoint pivot) {
			if (current.row == pivot.row && current.col > pivot.col)
				current.setPoint(pivot.row + 1, pivot.col + 1);
			else if (current.col > pivot.col && current.row < pivot.row)
				current.setPoint(pivot.row, pivot.col + 1);
			else if (current.row < pivot.row && current.col == pivot.col)
				current.setPoint(pivot.row - 1, pivot.col + 1);
			else if (current.row < pivot.row && current.col < pivot.col)
				current.setPoint(pivot.row - 1, pivot.col);
			else if (current.row == pivot.row && current.col < pivot.col)
				current.setPoint(pivot.row - 1, pivot.col - 1);
			else if (current.row > pivot.row && current.col < pivot.col)
				current.setPoint(pivot.row, pivot.col - 1);
			else if (current.row > pivot.row && current.col == pivot.col)
				current.setPoint(pivot.row + 1, pivot.col - 1);
			else if (current.row > pivot.row && current.col > pivot.col)
				current.setPoint(pivot.row + 1, pivot.col);
		}

		//checks if a given pixel is out of bounds of the image
		inline bool pixelOutOfBounds(fast::blobPoint p, int numrows, int numcols) {
			return (p.col >= numcols || p.row >= numrows || p.col < 0 || p.row < 0);
		}

		//marks a pixel as examined after passing through
		inline void markExamined(fast::blobPoint mark, fast::blobPoint center, bool checked[4]) {
			//p3.row, p3.col + 1
			int loc = -1;
			//    3
			//  2 x 0
			//    1
			if (mark.col > center.col)
				loc = 0;
			else if (mark.col < center.col)
				loc = 2;
			else if (mark.row > center.row)
				loc = 1;
			else if (mark.row < center.row)
				loc = 3;

			if (loc == -1)
				throw std::exception("Error: markExamined Failed");

			checked[loc] = true;
			return;
		}

		//marks a pixel as examined after passing through in the 8-connected case
		inline void markExamined8(fast::blobPoint mark, fast::blobPoint center, bool checked[8]) {
			//p3.row, p3.col + 1
			int loc = -1;
			//  5 6 7
			//  4 x 0
			//  3 2 1
			if (mark.row == center.row && mark.col > center.col)
				loc = 0;
			else if (mark.col > center.col && mark.row < center.row)
				loc = 7;
			else if (mark.row < center.row && mark.col == center.col)
				loc = 6;
			else if (mark.row < center.row && mark.col < center.col)
				loc = 5;
			else if (mark.row == center.row && mark.col < center.col)
				loc = 4;
			else if (mark.row > center.row && mark.col < center.col)
				loc = 3;
			else if (mark.row > center.row && mark.col == center.col)
				loc = 2;
			else if (mark.row > center.row && mark.col > center.col)
				loc = 1;

			if (loc == -1)
				throw std::exception("Error: markExamined Failed");

			checked[loc] = true;
			return;
		}

		//checks if given pixel has already been examined
		inline bool isExamined(bool checked[4]) {
			//p3.row, p3.col + 1
			return checked[0];
		}

		inline bool isExamined8(bool checked[8]) {
			//p3.row, p3.col + 1
			return checked[0];
		}


		//follows a border from start to finish given a starting point
		inline void followBorder(std::vector<std::vector<int>>& image, int row, int col, fast::blobPoint p2, fast::border NBD, std::vector<std::vector<fast::blobPoint>>& contours) {
			int numrows = (int)image.size();
			int numcols = (int)image[0].size();
			fast::blobPoint current(p2.row, p2.col);
			fast::blobPoint start(row, col);
			std::vector<fast::blobPoint> point_storage;

			//(3.1)
			//Starting from (i2, j2), look around clockwise the pixels in the neighborhood of (i, j) and find a nonzero pixel.
			//Let (i1, j1) be the first found nonzero pixel. If no nonzero pixel is found, assign -NBD to fij and go to (4).
			do {
				stepCW(current, start);
				if (current.samePoint(p2)) {
					image[start.row][start.col] = -NBD.seq_num;
					point_storage.push_back(start);
					contours.push_back(point_storage);
					return;
				}
			} while (pixelOutOfBounds(current, numrows, numcols) || image[current.row][current.col] == 0);
			fast::blobPoint p1 = current;

			//(3.2)
			//(i2, j2) <- (i1, j1) and (i3, j3) <- (i, j).

			fast::blobPoint p3 = start;
			fast::blobPoint p4;
			p2 = p1;
			bool checked[4];
			//	bool checked[8];
			while (true) {
				//(3.3)
				//Starting from the next element of the pixel(i2, j2) in the counterclockwise order, examine counterclockwise the pixels in the
				//neighborhood of the current pixel(i3, j3) to find a nonzero pixel and let the first one be(i4, j4).
				current = p2;

				for (int i = 0; i < 4; i++)
					checked[i] = false;

				do {
					markExamined(current, p3, checked);
					stepCCW(current, p3);
				} while (pixelOutOfBounds(current, numrows, numcols) || image[current.row][current.col] == 0);
				p4 = current;

				//Change the value fi3, j3 of the pixel(i3, j3) as follows :
				//	If the pixel(i3, j3 + 1) is a 0 - pixel examined in the substep(3.3) then fi3, j3 <- - NBD.
				//	If the pixel(i3, j3 + 1) is not a 0 - pixel examined in the substep(3.3) and fi3, j3 = 1, then fi3, j3 ←NBD.
				//	Otherwise, do not change fi3, j3.

				if ((p3.col + 1 >= numcols || image[p3.row][p3.col + 1] == 0) && isExamined(checked)) {
					image[p3.row][p3.col] = -NBD.seq_num;
				}
				else if (p3.col + 1 < numcols && image[p3.row][p3.col] == 1) {
					image[p3.row][p3.col] = NBD.seq_num;
				}

				point_storage.push_back(p3);
				//printImage(image, image.size(), image[0].size());
				//(3.5)
				//If(i4, j4) = (i, j) and (i3, j3) = (i1, j1) (coming back to the starting point), then go to(4);
				//otherwise, (i2, j2) <- (i3, j3), (i3, j3) <- (i4, j4), and go back to(3.3).
				if (p4.samePoint(start) && p3.samePoint(p1)) {
					contours.push_back(point_storage);
					return;
				}

				p2 = p3;
				p3 = p4;
			}
		}
	};
}


fast::fastBlob::fastBlob() : impl(new impl_fastBlob()) {

}

fast::fastBlob::~fastBlob() {

}

std::vector<fast::blobInfo> fast::fastBlob::blobInfo() {

	return this->impl->blobInfo();
}

void fast::fastBlob::compute(unsigned char * buffer ,int width, int height) {
	try {
		this->impl->compute(buffer, width, height);
	}
	catch (std::exception e) {
		throw e;
	}
}


std::vector<fast::blobInfo> fast::fastBlob::rectFilter(std::vector<fast::blobInfo> blobInfo, int minWidth, int maxWidth, int minHeight, int maxHeight) {

	std::vector<fast::blobInfo> result;

	for (auto& info : blobInfo) {
		if (info.rectWidth() > minWidth && info.rectWidth() < maxWidth && info.rectHeight() > minHeight && info.rectHeight() < maxHeight)
			result.push_back(info);
	}

	return result;
	
}


void fast::fastBlob::blobFill(unsigned char* buffer, int width, int height, unsigned char pixel, fast::blobInfo info) {
	
	auto points = info.getBlobPoints();

	if (buffer == nullptr) {
		HVERROR(error, "Invalid image buffer");
	}

	if (width <= 0 || height <= 0) {
		HVERROR(error, "Invalid image size");
	}

	if (info.rectX() < 0 || info.rectX() > width || info.rectY() < 0 || info.rectY() > height) {
		HVERROR(error, "Invalid blob info");
	}

	if (info.rectWidth() + info.rectX() > width ||  info.rectHeight() + info.rectY() > height) {
		HVERROR(error, "Invalid blob info");
	}

	int start_y = info.rectY();
	int end_Y = info.rectHeight() + info.rectY();



	//first line drawing and temp_top fill
	std::vector<fast::calPoint> temp_top;
	for (auto& element : points) {
		if (element.y == start_y) {
			temp_top.push_back(element);
			buffer[(int)element.y * width + (int)element.x] = pixel;
		}
			
	}
	//first line drawing and temp_top fill


	
	for (int row = start_y+1; row < end_Y + 1; row++) {

		std::deque<fast::calPoint> columns;
		for (auto& element : points) {
			if (element.y == row)
				columns.push_back(element);
		}
		
		
		
		
		if (columns.size() == 0) continue;


		
		
		
		std::sort(columns.begin(), columns.end(), [&](fast::calPoint & first, fast::calPoint& second) {
			return first.x < second.x;
		});

		
		
		
		
		std::vector<fast::calPoint> current_top = temp_top;
		temp_top.clear();

		
		
		
		
		std::deque<fast::calPoint> active_columns;
		while (!columns.empty()) {
			auto current_element = columns.front();
			columns.pop_front();

			//std::cout << "current x :" << current_element.x << std::endl;
			//std::cout << "current y :" << current_element.y << std::endl;

			int start_dy = current_element.y - 1;
			int end_dy = current_element.y + 1;
			int start_dx = current_element.x - 1;
			int end_dx = current_element.x + 1;


			bool found = false;
			for (auto& top_element : current_top) {
				for (int dy = start_dy; dy <= end_dy && found == false; dy++) {
					for (int dx = start_dx; dx<= end_dx && found == false; dx++) {
					

						//std::cout << "dx :" << dx << std::endl;
						//std::cout << "dy :" << dy << std::endl;

						if (top_element.x == dx && top_element.y == dy)
							found = true;
					}
				}
			}

			if (found) {
				active_columns.push_back(current_element);
			}
			else {
				buffer[(int)current_element.y * width + (int)current_element.x] = pixel;
			}
			temp_top.push_back(current_element);
		}

		//std::cout << "active line check done" << std::endl;

		if (active_columns.size() == 0) {
			//std::cout << "something wrong!!!" << std::endl;
			//std::cout << "current size = " << active_columns.size() << std::endl;
			continue;
		}

		//std::cout << "drawing start" << std::endl;
		//std::cout << "current active line count = " << active_columns.size() << std::endl;

		std::sort(active_columns.begin(), active_columns.end(), [&](fast::calPoint& first, fast::calPoint& second) {
			return first.x < second.x;
		});


		fast::calPoint first_element = active_columns.front();
		fast::calPoint second_element = active_columns.back();
		for (int x = first_element.x; x <= second_element.x; x++)
			buffer[(int)row * width + (int)x] = pixel;


	}


}