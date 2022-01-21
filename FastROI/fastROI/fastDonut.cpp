#define _USE_MATH_DEFINES

#include "fastDonut.h"

#include <Eigen/Core>
#include <Eigen/Dense>




std::vector<std::vector<fast::calPoint>> fast::fastDonut(int center_x, int center_y, int radius, double start_ratio, double end_ratio, double step_angle) {

	if (radius == 0) {
		HVERROR(error, "Invalid box roi range");
	}

	if (step_angle <= 0) {
		HVERROR(error, "Invalid step angle");
	}

	if (start_ratio <= 0 && end_ratio <= 0) {
		HVERROR(error, "Invalid ratio");
	}



	int start_radius = (int)((double)radius * start_ratio);
	int end_radius = (int)((double)radius * end_ratio);

	int distance_radius = (int)fabs(start_radius - end_radius);
	if (distance_radius == 0) {
		HVERROR(error, "Invalid start ratio and end ratio");
	}

	int aligned_distance_radius = distance_radius + (distance_radius % 4);
	int min_radius = start_radius > end_radius ? end_radius : start_radius;
	int max_radius = start_radius > end_radius ? start_radius : end_radius;


	/// Radius SIMD 
	std::vector<double> vector_radius; // Radius vector
	vector_radius.resize(aligned_distance_radius * 2);


	int sign = 1;
	if (start_radius > end_radius)
		sign = -1;

	int current_radius = start_radius;
	for (int radius = 0; radius < distance_radius * 2;) {
		vector_radius[radius] = current_radius;
		vector_radius[radius + 1] = current_radius;
		radius += 2;
		current_radius += sign;
	}

	/// Coordinate SIMD 
	std::vector<double> vector_cordinate_table = { (double)center_x , (double)center_y, (double)center_x , (double)center_y }; // cordinate vector
	const __m256d simd_coordinate = _mm256_load_pd(vector_cordinate_table.data()); // cordinate simd



	std::vector<std::vector<fast::calPoint>> combine_vertical_xy;
	int doubleDistance = distance_radius * 2;
	int chunk_size = sizeof(double) * 4;

	for (double angle = 0; angle < 360;) {

		/// trigonometric SIMD 
		double x_cos = sin(angle * M_PI / 180);
		double y_sine = cos(angle * M_PI / 180);
		std::vector<double> vector_trigonometric_table = { x_cos ,y_sine, x_cos ,y_sine }; // trigonometric vector
		const __m256d simd_trigonometric = _mm256_load_pd(vector_trigonometric_table.data()); // trigonometric simd


		std::vector<fast::calPoint> vertical_xy;
		vertical_xy.resize(aligned_distance_radius);
		void* vertical_xy_ptr = vertical_xy.data();
		for (int simd_skip = 0; simd_skip < doubleDistance;) {

			const __m256d simd_radius = _mm256_load_pd(vector_radius.data() + simd_skip); // radius simd
			__m256d chunk_mul = _mm256_mul_pd(simd_radius, simd_trigonometric);
			__m256d start_result = _mm256_add_pd(chunk_mul, simd_coordinate);
			memcpy((((double*)vertical_xy_ptr) + simd_skip), &start_result, chunk_size);
			simd_skip += 4;
		}
		vertical_xy.resize(distance_radius);
		combine_vertical_xy.push_back(vertical_xy);

		angle += step_angle;
	}

	return combine_vertical_xy;
}
