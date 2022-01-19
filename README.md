Fast ROI
=======================
The FAST ROI library is useful for quickly extracting the coordinates of a rotating rectangular ROI

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
### Baseline = Searching gap : 1

|Name|fps|
|------|---|
|fast::fastRect ***(500x500 roi size)***|2100 fps|
|fast::fastRect ***(1000x1000 roi size)***|550 fps|
|fast::fastRect ***(2000x2000 roi size)***|35 fps|

How to use?
=======================
#### fastRect
<center>
<img src="https://raw.githubusercontent.com/gellston/FastROI/main/images/fastRect.png">
</center>


```cpp
#include <fastRect.h>

int centerX = 2000;     // rect center y
int centerY = 2000;     // rect center y
double angle = 40;      // rotation angle
int range = 1000;       // same as width
int distance = 1000;    // same as height 
bool search_direction = false //search direction (false : foward direction, true : backward direction)
int searching gap = 10;   //gap pixels

std::vector<std::vector<fast::calPoint>> result = fast::rectROI(centerX, centerY, angle, range, distance, false, skip_pixels);

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
<img src="https://github.com/gellston/FastROI/blob/main/images/fastRect_rotation.gif?raw=true">
</center>


<div style="text-align: right; margin-right:30px;"> 

[TOP](#fast-roi) 


