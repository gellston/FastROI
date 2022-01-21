#pragma once

#ifndef FAST_DOUNUT
#define FAST_DOUNUT

#include <vector>
#include <memory>

#include "macro.h"
#include "calPoint.h"

namespace fast {
	FAST_API_EXPORT std::vector<std::vector<fast::calPoint>> fastDonut(int center_x, int center_y, int radius, double start_ratio, double end_ratio, double step_angle);
}




#endif