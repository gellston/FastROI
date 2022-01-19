#pragma once

#ifndef FAST_RECT
#define FAST_RECT

#include <vector>
#include <memory>

#include "macro.h"
#include "calPoint.h"

namespace fast {
	FAST_API_EXPORT std::vector<std::vector<fast::calPoint>> rectROI(int centerx, int centery, double angle, int range, int distance, bool direction, int skip_pixels);
}




#endif