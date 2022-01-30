#pragma once

#ifndef FAST_BLOB
#define FAST_BLOB

#include <vector>
#include <memory>

#include "macro.h"
#include "calPoint.h"
#include "blobInfo.h"

namespace fast {

	class impl_fastBlob;

	class FAST_API_EXPORT fastBlob {

	private:
		std::shared_ptr<impl_fastBlob> impl;
		
	public:

		fastBlob();
		~fastBlob();

		void compute(unsigned char * buffer, int width, int height);
		std::vector<fast::blobInfo> blobInfo();
		std::vector<fast::blobInfo> rectFilter(int minWidth, int maxWidth, int minHeight, int maxHeight);
		

		

	};
}




#endif