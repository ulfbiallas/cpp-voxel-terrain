#ifndef VOXELMAP_H
#define VOXELMAP_H

#include <cstdlib>
#include <iostream>
#include <vector>
#include "lodepng.h"
#include "HeightMap.h"


class VoxelMap {

    public:      

		VoxelMap(HeightMap *heightMap);
		~VoxelMap();

		float getDensity(int w, int h, int l);
		int getWidth();
		int getHeight();
		int getLength();



	private:

		int width, height, length;
		float *data;

		int index(int w, int h, int l);

};



#endif