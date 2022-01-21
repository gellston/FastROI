Fast ROI v1.1
=======================
The FAST ROI library is useful for quickly extracting the coordinates of a rotating rectangular ROI and donut ROI

Development Environment
=======================
 - **Visual Studio 2022 C++17 (Latest Window10 SDK)**
 - **Need eigen3 library to compile**
 - **Need vcpkg tool to install eigen3 library** (<a href="https://github.com/microsoft/vcpkg" target="_blank">https://github.com/microsoft/vcpkg</a>)
```
// Eigen3 library installation via vcpkg
// Please paste the command below
vpckg install eigen3
vcpkg integrate install
```
Performance
=======================
### fastRect baseline = searching gap : 1


|Name|fps|
|------|---|
|fast::fastRect ***(500x500 roi size)***|2100 fps|
|fast::fastRect ***(1000x1000 roi size)***|550 fps|
|fast::fastRect ***(2000x2000 roi size)***|35 fps|
### fastDonut baseline = start_ratio : 1.3, end_ratio : 0.7, angle_step=1

|Name|fps|
|------|---|
|fast::fastDonut ***(800 radius)***|3560 fps|
|fast::fastDonut ***(1500 radius)***|666 fps|

How to use?
=======================
### fastRect

```cpp
#include <fastRect.h>

int centerX = 2000;     // rect center y
int centerY = 2000;     // rect center y
double angle = 40;      // rotation angle
int range = 1000;       // same as width
int distance = 1000;    // same as height 
bool search_direction = false //search direction (false : foward direction, true : backward direction)
int searching gap = 10;   //gap pixels

std::vector<std::vector<fast::calPoint>> result = fast::fastRect(centerX, centerY, angle, range, distance, false, skip_pixels);

//iteration of vertical line
for (auto vertical_line : vertical_lines) {

    //iteration of point in vertical line
    for (auto calPoint : vertical_line) {
    
        //Check current position is in image
        if (calPoint.x < 0 || calPoint.x >= 4000 || calPoint.y < 0 || calPoint.y >= 4000)
            continue;
    
        //Do whatever you want here
        
    }
}
		
```

#### DEMO
<center>

![fastRectDEMO](https://github.com/gellston/FastROI/blob/main/images/fastRect_rotation.gif?raw=true)

</center>

### fastDonut

```cpp
#include <fastDonut.h>

int centerX = 2000;        // rect center y
int centerY = 2000;        // rect center y
int radius = 1500;         // base radius
double start_ratio = 1.5;  // start radius ratio , start radius = start_ratio * base radius
double end_ratio = 0.5;    // end radius ratio , end radius = end_ratio * base radius
double step_angle = 1;     // step angle (angle will increase per step angle CCW)

std::vector<std::vector<fast::calPoint>> result = fast::fastDonut(centerX, centerY, radius, start_ratio, end_ratio, step_angle);

//iteration of vertical line
for (auto vertical_line : vertical_lines) {

    //iteration of point in vertical line
    for (auto calPoint : vertical_line) {
    
        //Check current position is in image
        if (calPoint.x < 0 || calPoint.x >= 4000 || calPoint.y < 0 || calPoint.y >= 4000)
            continue;
    
        //Do whatever you want here
        
    }
}

		
```

#### DEMO
<center>

<img src="https://github.com/gellston/FastROI/blob/main/images/fast_donut_demo.gif?raw=true" width=450>

</center>



