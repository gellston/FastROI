#pragma once

#ifndef HV_BLOB_INFO
#define HV_BLOB_INFO

#include <type_traits>



namespace fast {
    struct blobInfo {
    private:

        int _id;
        int _rect_x;
        int _rect_y;
        int _rect_width;
        int _rect_height;

        double _centeriod_x;
        double _centeriod_y;

        std::vector<fast::calPoint> _blob_points;
        
    public :

        blobInfo(int id, int rect_x, int rect_y, int rect_width, int rect_height, double centeroid_x, double centeroid_y) {
            this->_id = id;
            this->_rect_x = rect_x;
            this->_rect_y = rect_y;
            this->_rect_width = rect_width;
            this->_rect_height = rect_height;

            this->_centeriod_x = centeroid_x;
            this->_centeriod_y = centeroid_y;
        }

        int id() {
            return _id;
        }
        int rectX() {
            return _rect_x;
        }
        int rectY() {
            return _rect_y;
        }
        int rectWidth() {
            return _rect_width;
        }
        int rectHeight() {
            return _rect_height;
        }

        void setBlobPoints(std::vector<fast::calPoint>& points) {
            this->_blob_points = points;
        }
    };
}




#endif