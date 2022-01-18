

#define _USE_MATH_DEFINES

#include "fastRect.h"






#include <Eigen/Core>
#include <Eigen/Dense>




std::vector<std::vector<fast::calPoint>> fast::rectROI(int center_x, int center_y, double angle, int range, int distance, bool direction, int skip_pixels) {



    if (range == 0) {
        HVERROR(error, "Invalid box roi range");
    }

    if (distance == 0) {
        HVERROR(error, "Invalid box distance");
    }

    if (skip_pixels == 0) {
        HVERROR(error, "Invalid search step");
    }


    int dimension = range * distance;

    int roi_mid_width = range / 2;
    int roi_mid_height = distance / 2;


    int start_x1 = 0;
    int start_y1 = 0;
    int end_x1 = 0;
    int end_y1 = 0;


    int start_x2 = 0;
    int start_y2 = 0;
    int end_x2 = 0;
    int end_y2 = 0;


    if (direction == false) {
        start_x1 = center_x - roi_mid_width;
        start_y1 = center_y + roi_mid_height;

        start_x2 = center_x + roi_mid_width;
        start_y2 = center_y + roi_mid_height;

        end_x1 = center_x - roi_mid_width;
        end_y1 = center_y - roi_mid_height;

        end_x2 = center_x + roi_mid_width;
        end_y2 = center_y - roi_mid_height;
    }
    else {
        start_x1 = center_x - roi_mid_width;
        start_y1 = center_y - roi_mid_height;

        start_x2 = center_x + roi_mid_width;
        start_y2 = center_y - roi_mid_height;

        end_x1 = center_x - roi_mid_width;
        end_y1 = center_y + roi_mid_height;

        end_x2 = center_x + roi_mid_width;
        end_y2 = center_y + roi_mid_height;
    }




    int size = 4;

    Eigen::MatrixXd trigonometric_matrix(2, 2);

    //trigonometric_matrix
    trigonometric_matrix(0, 0) = cos(angle * M_PI / 180);
    trigonometric_matrix(0, 1) = -sin(angle * M_PI / 180);
    trigonometric_matrix(1, 0) = sin(angle * M_PI / 180);
    trigonometric_matrix(1, 1) = cos(angle * M_PI / 180);

    //rotation_matrix
    Eigen::MatrixXd rotation_matrix(2, size);

    //base_matrix
    Eigen::MatrixXd base_matrix(2, size);

    rotation_matrix(0, 0) = start_x1 - center_x; // x           Start X1
    rotation_matrix(1, 0) = start_y1 - center_y; // y

    rotation_matrix(0, 1) = start_x2 - center_x; // x
    rotation_matrix(1, 1) = start_y2 - center_y; // y

    rotation_matrix(0, 2) = end_x1 - center_x;// x
    rotation_matrix(1, 2) = end_y1 - center_y; // y

    rotation_matrix(0, 3) = end_x2 - center_x; // x
    rotation_matrix(1, 3) = end_y2 - center_y; /// y

    // No problem
    for (int index = 0; index < 4; index++) {
        base_matrix(0, index) = center_x;
        base_matrix(1, index) = center_y;
    }

    Eigen::MatrixXd result = trigonometric_matrix * rotation_matrix + base_matrix;


    double rotated_start_x1 = result(0, 0);
    double rotated_start_y1 = result(1, 0);

    double rotated_start_x2 = result(0, 1);
    double rotated_start_y2 = result(1, 1);

    double rotated_end_x1 = result(0, 2);
    double rotated_end_y1 = result(1, 2);

    double rotated_end_x2 = result(0, 3);
    double rotated_end_y2 = result(1, 3);


    //startx1 = rotated_start_x1;
    //starty1 = rotated_start_y1;
    //startx2 = rotated_start_x2;
    //starty2 = rotated_start_y2;
    //endx1 = rotated_end_x1;
    //endy1 = rotated_end_y1;
    //endx2 = rotated_end_x2;
    //endy2 = rotated_end_y2;

    double diff_start_x = rotated_start_x2 - rotated_start_x1;
    double diff_start_y = rotated_start_y2 - rotated_start_y1;

    double increase_rate_x = diff_start_x / range;
    double increase_rate_y = diff_start_y / range;


    //Start Line Points
    //Start Line Points
    //Start Line Points
    //Start Line Points
    int range_align_size = (range)+(range % 4);
    std::vector<double> range_vec(range_align_size * 2);
    for (int index = 0; index < range * 2; index += 2) {
        int current_index = index;
        range_vec[index] = current_index / 2;
        range_vec[index + 1] = current_index / 2;
    }

    std::vector<fast::calPoint> start_line_xy(range_align_size);
    std::vector<fast::calPoint> end_line_xy(range_align_size);

    std::vector<double> increase_rate_xy_vec = { increase_rate_x, increase_rate_y,increase_rate_x, increase_rate_y };
    const __m256d simd_increase_rate_xy = _mm256_load_pd(increase_rate_xy_vec.data());

    std::vector<double> simd_start_xy_vec = { rotated_start_x1 , rotated_start_y1, rotated_start_x1 , rotated_start_y1 };
    const __m256d simd_start_xy = _mm256_load_pd(simd_start_xy_vec.data());

    std::vector<double> simd_end_xy_vec = { rotated_end_x1 , rotated_end_y1, rotated_end_x1 , rotated_end_y1 };
    const __m256d simd_end_xy = _mm256_load_pd(simd_end_xy_vec.data());


    const double* range_ptr = &range_vec[0];

    const fast::calPoint* start_result_xy_ptr = start_line_xy.data();
    const fast::calPoint* end_result_xy_ptr = end_line_xy.data();

    int chunk_size = sizeof(double) * 4;
    for (int index = 0; index < range * 2; index += 4) {

        __m256d range_chunk = _mm256_load_pd(range_ptr + index);

        __m256d chunk_mul = _mm256_mul_pd(simd_increase_rate_xy, range_chunk);

        // Start Line
        __m256d start_result = _mm256_add_pd(chunk_mul, simd_start_xy);

        // End Line
        __m256d end_result = _mm256_add_pd(chunk_mul, simd_end_xy);
        memcpy(((double*)start_result_xy_ptr + index), &start_result, chunk_size);
        memcpy(((double*)end_result_xy_ptr + index), &end_result, chunk_size);
    }

    //Start Line Points
    //Start Line Points
    //Start Line Points
    //Start Line Points

    int distance_align_size = (distance)+(distance % 4);
    std::vector<double> distance_vec(distance_align_size * 2);
    for (int index = 0; index < distance * 2; index += 2) {
        int current_index = index;
        distance_vec[index] = current_index / 2;
        distance_vec[index + 1] = current_index / 2;
    }

    std::vector<std::vector<fast::calPoint>> combine_vertical_xy;

    double diff_virtical_start_x = rotated_end_x1 - rotated_start_x1;
    double diff_virtical_start_y = rotated_end_y1 - rotated_start_y1;

    double increase_virtical_rate_x = diff_virtical_start_x / distance;
    double increase_virtical_rate_y = diff_virtical_start_y / distance;

    std::vector<double> increase_vertical_rate_xy_vec = { increase_virtical_rate_x, increase_virtical_rate_y,increase_virtical_rate_x, increase_virtical_rate_y };
    const __m256d simd_increase_vertical_rate_xy = _mm256_load_pd(increase_vertical_rate_xy_vec.data());

    for (int range_index = 0; range_index < range; range_index += skip_pixels) {

        auto start_vertical_point = start_line_xy[range_index];
        std::vector<double> simd_vertical_xy_vec = { start_vertical_point.x , start_vertical_point.y, start_vertical_point.x , start_vertical_point.y };
        const __m256d simd_vertical_xy = _mm256_load_pd(simd_vertical_xy_vec.data());

        const double* distance_ptr = &distance_vec[0];

        std::vector<fast::calPoint> vertical_xy;
        vertical_xy.resize(distance_align_size);
        const fast::calPoint* start_vertical_xy_ptr = vertical_xy.data();

        for (int index = 0; index < distance * 2; index += 4) {
            __m256d distance_chunk = _mm256_load_pd(distance_ptr + index);
            __m256d chunk_mul = _mm256_mul_pd(simd_increase_vertical_rate_xy, distance_chunk);
            __m256d start_result = _mm256_add_pd(chunk_mul, simd_vertical_xy);
            memcpy((((double*)start_vertical_xy_ptr) + index), &start_result, chunk_size);
        }

        combine_vertical_xy.push_back(vertical_xy);
    }

    return combine_vertical_xy;
}